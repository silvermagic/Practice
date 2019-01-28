#include <iostream>
#include <iomanip>
#include <cstring>
#include <amqp.h>
#include <amqp_framing.h>
#include <amqp_tcp_socket.h>

// 子频道标识，实现物理AMQP连接的逻辑复用，即基于一个物理AMQP连接，我们可以创建多个对应虚拟AMQP连接，用于区分不同业务，
// 默认情况下我们只需要一个虚拟AMQP连接，因为我们的业务一般只有接收和发送
const int SEND_CHANNEL_ID = 1;
const int RECV_CHANNEL_ID = 2;

void amqp_reply_error_string(const char *prefix, amqp_rpc_reply_t x) {
    switch (x.reply_type) {
        case AMQP_RESPONSE_NORMAL:
            break;

        case AMQP_RESPONSE_NONE:
            std::cerr << prefix << " missing rpc reply type!" << std::endl;
            break;

        case AMQP_RESPONSE_LIBRARY_EXCEPTION:
            std::cerr << prefix << " " << amqp_error_string2(x.library_error) << std::endl;
            break;

        case AMQP_RESPONSE_SERVER_EXCEPTION:
            switch (x.reply.id) {
                case AMQP_CONNECTION_CLOSE_METHOD: {
                    amqp_connection_close_t *m = (amqp_connection_close_t *) x.reply.decoded;
                    std::cerr << prefix << " server connection error: " << m->reply_code << ", message: "
                              << m->reply_text.bytes
                              << std::endl;
                    break;
                }
                case AMQP_CHANNEL_CLOSE_METHOD: {
                    amqp_channel_close_t *m = (amqp_channel_close_t *) x.reply.decoded;
                    std::cerr << prefix << " server channel error: " << m->reply_code << ", message: "
                              << m->reply_text.bytes
                              << std::endl;
                    break;
                }
                default:
                    std::cerr << prefix << " method(" << std::setw(8) << std::hex << x.reply.id
                              << ") unknown server error"
                              << std::endl;
                    break;
            }
            break;
    }
}

int main() {
    // 创建RabbitMQ连接
    amqp_connection_state_t conn = amqp_new_connection();
    if (conn == nullptr) {
        std::cerr << "new connection failed!" << std::endl;
        return -1;
    }
    amqp_socket_t *socket = amqp_tcp_socket_new(conn);
    if (socket == nullptr) {
        std::cerr << "new tcp socket failed!" << std::endl;
        return -1;
    }
    int ret = amqp_socket_open(socket, "127.0.0.1", 5672);
    if (ret != AMQP_STATUS_OK) {
        std::cerr << "Open socket: " << amqp_error_string2(ret) << std::endl;
        return -1;
    }
    amqp_rpc_reply_t reply = amqp_login(conn, "/", AMQP_DEFAULT_MAX_CHANNELS, AMQP_DEFAULT_FRAME_SIZE, 0,
                                        AMQP_SASL_METHOD_PLAIN,
                                        "guest", "guest");
    if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
        amqp_reply_error_string("Login:", reply);
        return -1;
    }

    // 创建用于发送的子频道
    amqp_channel_open(conn, SEND_CHANNEL_ID);
    reply = amqp_get_rpc_reply(conn);
    if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
        amqp_reply_error_string("Open channel: ", reply);
        return -1;
    }

    // 创建消息队列（用于发送RPC消息）
    amqp_queue_declare(conn,
                       SEND_CHANNEL_ID, // 子频道ID
                       amqp_cstring_bytes("hello"), // 队列名
                       false, // 预建模式，队列必须已经存在，否则返回失败
                       false, // 持久化模式，即使RabbitMQ重启消息依然不会丢失，需要交换器也为持久模式
                       false, // 独占模式，不允许创建同名独占队列，独占队列在连接关闭后自动删除（不受持久化模式影响）
                       true, // 自动删除模式，如果队列没有被任何消费者订阅就会被自动删除
                       amqp_empty_table);
    reply = amqp_get_rpc_reply(conn);
    if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
        amqp_reply_error_string("Declare queue:", reply);
        return -1;
    }
    amqp_queue_bind(conn,
                    SEND_CHANNEL_ID, // 子频道ID
                    amqp_cstring_bytes("hello"), // 队列名
                    amqp_cstring_bytes("amq.direct"), // 直接交付模式
                    amqp_cstring_bytes("hello"), // 路由关键字
                    amqp_empty_table);
    reply = amqp_get_rpc_reply(conn);
    if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
        amqp_reply_error_string("Bind queue:", reply);
        return -1;
    }

    // 创建用于接收的子频道
    amqp_channel_open(conn, RECV_CHANNEL_ID);
    reply = amqp_get_rpc_reply(conn);
    if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
        amqp_reply_error_string("Open channel: ", reply);
        return -1;
    }

    // 创建消息队列（用于接收RPC返回消息）
    amqp_queue_declare_ok_t *r = amqp_queue_declare(conn,
                                                    RECV_CHANNEL_ID, // 子频道ID
                                                    amqp_empty_bytes, // 独占队列名称尽量由系统创建，避免重复
                                                    false, // 预建模式，队列必须已经存在，否则返回失败
                                                    false, // 持久化模式，即使RabbitMQ重启消息依然不会丢失，需要交换器也为持久模式
                                                    true, // 独占模式，不允许创建同名独占队列，独占队列在连接关闭后自动删除（不受持久化模式和自动删除模式影响）
                                                    false, // 自动删除模式，如果队列没有被任何消费者订阅就会被自动删除
                                                    amqp_empty_table);
    reply = amqp_get_rpc_reply(conn);
    if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
        amqp_reply_error_string("Declare queue:", reply);
        return -1;
    }
    amqp_queue_bind(conn,
                    RECV_CHANNEL_ID, // 子频道ID
                    r->queue, // 队列名
                    amqp_cstring_bytes("amq.direct"), // 直接交付模式
                    r->queue, // 路由关键字
                    amqp_empty_table);
    reply = amqp_get_rpc_reply(conn);
    if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
        amqp_reply_error_string("Bind queue:", reply);
        return -1;
    }

    // 绑定消费者到RPC返回消息队列
    amqp_basic_consume(conn,
                       RECV_CHANNEL_ID, // 子频道ID
                       r->queue, // 使用系统分配的队列名
                       amqp_empty_bytes, // 消费者标签
                       false,
                       true, // 自动确认模式，自动确认并删除消息，否则需要调用amqp_basic_ack来确认删除消息
                       true, // 独占模式，由于队列是独占的（其他消费者看不到，队列名字是随机的），所以这边设置消费者独占也没有影响
                       amqp_empty_table);
    reply = amqp_get_rpc_reply(conn);
    if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
        amqp_reply_error_string("consume:", reply);
        return -1;
    }
    amqp_envelope_t envelope;
    struct timeval wait_for = {10, 0}; // 等待超时时间

    // 发送RPC消息
    amqp_basic_properties_t props; // 用来保存发送消息的属性
    props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG | AMQP_BASIC_CORRELATION_ID_FLAG |
                   AMQP_BASIC_REPLY_TO_FLAG; // 指明设置了哪些属性
    props.content_type = amqp_cstring_bytes("text/plain"); // 消息使用明文
    props.delivery_mode = 2; // 持久化交付模式，即使RabbitMQ重启消息依然不会丢失，需要交换器也为持久模式
    props.reply_to = r->queue; // 服务端返回rpc消息的时候需要投递的队列名
    props.correlation_id = amqp_cstring_bytes("who"); // 关联ID，用于确认返回消息是哪个请求发送的
    ret = amqp_basic_publish(conn,
                             SEND_CHANNEL_ID, // 子频道ID
                             amqp_cstring_bytes("amq.direct"), // 直接交付模式
                             amqp_cstring_bytes("hello"), // 设置消息路由关键字，和队列绑定时的关键字保持一致
                             false, // 强制交付，表示如果RabbitMQ不能及时将消息路由给某个队列，那么将返回basic.reject方法
                             false, // 立即交付，表示如果RabbitMQ不能及时将消息路由给某个绑定在队列上的消费者，那么将返回basic.reject方法
                             &props,
                             amqp_cstring_bytes("who")); // 消息内容
    if (ret != AMQP_STATUS_OK) {
        std::cerr << "Sending: " << amqp_error_string2(ret) << std::endl;
        return -1;
    }
    // 接收RPC返回消息
    reply = amqp_consume_message(conn,
                                 &envelope, // 消息载体
                                 &wait_for,
                                 0); // 保留参数，暂时不用
    if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
        amqp_reply_error_string("consume:", reply);
    } else {
        if ((envelope.message.properties._flags & AMQP_BASIC_CORRELATION_ID_FLAG) &&
            std::memcmp(props.correlation_id.bytes, envelope.message.properties.correlation_id.bytes,
                       props.correlation_id.len < envelope.message.properties.correlation_id.len
                       ? props.correlation_id.len : envelope.message.properties.correlation_id.len) == 0) {
            std::cout << std::string(static_cast<char *>(envelope.message.body.bytes), envelope.message.body.len)
                      << " say: ";
        } else {
            std::cerr << "unknow rpc reponse" << std::endl;
        }
    }
    amqp_destroy_envelope(&envelope);

    // 发送RPC消息
    props.correlation_id = amqp_cstring_bytes("say"); // 关联ID，用于确认返回消息是哪个请求发送的
    ret = amqp_basic_publish(conn,
                             SEND_CHANNEL_ID, // 子频道ID
                             amqp_cstring_bytes("amq.direct"), // 直接交付模式
                             amqp_cstring_bytes("hello"), // 设置消息路由关键字，和队列绑定时的关键字保持一致
                             false, // 强制交付，表示如果RabbitMQ不能及时将消息路由给某个队列，那么将返回basic.reject方法
                             false, // 立即交付，表示如果RabbitMQ不能及时将消息路由给某个绑定在队列上的消费者，那么将返回basic.reject方法
                             &props,
                             amqp_cstring_bytes("say")); // 消息内容
    if (ret != AMQP_STATUS_OK) {
        std::cerr << "Sending: " << amqp_error_string2(ret) << std::endl;
        return -1;
    }
    // 接收RPC返回消息
    reply = amqp_consume_message(conn,
                                 &envelope, // 消息载体
                                 &wait_for,
                                 0); // 保留参数，暂时不用
    if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
        amqp_reply_error_string("consume:", reply);
    } else {
        if ((envelope.message.properties._flags & AMQP_BASIC_CORRELATION_ID_FLAG) &&
            std::memcmp(props.correlation_id.bytes, envelope.message.properties.correlation_id.bytes,
                       props.correlation_id.len < envelope.message.properties.correlation_id.len
                       ? props.correlation_id.len : envelope.message.properties.correlation_id.len) == 0) {
            std::cout << std::string(static_cast<char *>(envelope.message.body.bytes), envelope.message.body.len)
                      << std::endl;
        } else {
            std::cerr << "unknow rpc reponse" << std::endl;
        }
    }
    amqp_destroy_envelope(&envelope);

    // 清理
    reply = amqp_channel_close(conn, SEND_CHANNEL_ID, AMQP_REPLY_SUCCESS); // 关闭发送子通道
    if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
        amqp_reply_error_string("Close channel:", reply);
        return -1;
    }
    reply = amqp_channel_close(conn, RECV_CHANNEL_ID, AMQP_REPLY_SUCCESS); // 关闭发送子通道
    if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
        amqp_reply_error_string("Close channel:", reply);
        return -1;
    }
    reply = amqp_connection_close(conn, AMQP_REPLY_SUCCESS); // 关闭RabbitMQ连接
    if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
        amqp_reply_error_string("Close connection:", reply);
        return -1;
    }
    ret = amqp_destroy_connection(conn); // 销毁RabbitMQ连接
    if (ret != AMQP_STATUS_OK) {
        std::cerr << "Destroy Connection: " << amqp_error_string2(ret) << std::endl;
        return -1;
    }

    return 0;
}