#include <iostream>
#include <iomanip>
#include <amqp.h>
#include <amqp_framing.h>
#include <amqp_tcp_socket.h>

// 子频道标识，实现物理AMQP连接的逻辑复用，即基于一个物理AMQP连接，我们可以创建多个对应虚拟AMQP连接，用于区分不同业务，
// 默认情况下我们只需要一个虚拟AMQP连接，因为我们的业务一般只有接收和发送
const int CHANNEL_ID = 1;

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
    amqp_channel_open(conn, CHANNEL_ID);
    reply = amqp_get_rpc_reply(conn);
    if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
        amqp_reply_error_string("Open channel: ", reply);
        return -1;
    }

    // 创建消息队列
    amqp_queue_declare(conn,
                       CHANNEL_ID, // 子频道ID
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

    // 绑定消息队列
    amqp_queue_bind(conn,
                    CHANNEL_ID, // 子频道ID
                    amqp_cstring_bytes("hello"), // 队列名
                    amqp_cstring_bytes("amq.direct"), // 直接交付模式
                    amqp_empty_bytes, // 路由关键字
                    amqp_empty_table);
    reply = amqp_get_rpc_reply(conn);
    if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
        amqp_reply_error_string("Bind queue:", reply);
        return -1;
    }

    // 发送消息
    amqp_basic_properties_t props; // 用来保存发送消息的属性
    props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG; // 指明设置了哪些属性
    props.content_type = amqp_cstring_bytes("text/plain"); // 消息使用明文
    props.delivery_mode = 2; // 持久化交付模式，即使RabbitMQ重启消息依然不会丢失，需要交换器也为持久模式
    ret = amqp_basic_publish(conn,
                             CHANNEL_ID, // 子频道ID
                             amqp_cstring_bytes("amq.direct"), // 直接交付模式
                             amqp_empty_bytes, // 设置消息路由关键字，和队列绑定时的关键字保持一致
                             false, // 强制交付，表示如果RabbitMQ不能及时将消息路由给某个队列，那么将返回basic.reject方法
                             false, // 立即交付，表示如果RabbitMQ不能及时将消息路由给某个绑定在队列上的消费者，那么将返回basic.reject方法
                             &props,
                             amqp_cstring_bytes("hello world!")); // 消息内容
    if (ret != AMQP_STATUS_OK) {
        std::cerr << "Sending: " << amqp_error_string2(ret) << std::endl;
        return -1;
    }

    // 清理
    reply = amqp_channel_close(conn, CHANNEL_ID, AMQP_REPLY_SUCCESS); // 关闭发送子通道
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