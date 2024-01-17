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

    // 创建用于接收的子频道
    amqp_channel_open(conn, CHANNEL_ID);
    reply = amqp_get_rpc_reply(conn);
    if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
        amqp_reply_error_string("Open channel:", reply);
        return -1;
    }

    // 接收消息
    amqp_basic_consume(conn,
                       CHANNEL_ID, // 子频道ID
                       amqp_cstring_bytes("hello"),//队列名
                       amqp_empty_bytes, // 消费者标签
                       false, //
                       true, // 自动确认模式，自动确认并删除消息，否则需要调用amqp_basic_ack来确认删除消息
                       false, // 独占模式
                       amqp_empty_table);
    reply = amqp_get_rpc_reply(conn);
    if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
        amqp_reply_error_string("Consume:", reply);
        return -1;
    }
    amqp_envelope_t envelope;
    while (true) {
        // 手动释放内存，不过库内部已启用重用机制，此函数效果不大
        // amqp_maybe_release_buffers_on_channel(conn, CHANNEL_ID);

        // 接收消息
        reply = amqp_consume_message(conn,
                                     &envelope, // 消息载体
                                     nullptr, // 等待交付超时时间，NULL表示阻塞等待
                                     0); // 保留参数，暂时不用
        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
            amqp_reply_error_string("Consume:", reply);
            break;
        }

        // 打印消息
        std::cout << "Delivery " << (unsigned) envelope.delivery_tag << ", exchange "
                  << std::string(static_cast<char *>(envelope.exchange.bytes), envelope.exchange.len)
                  << " routing_key "
                  << std::string(static_cast<char *>(envelope.routing_key.bytes), envelope.routing_key.len)
                  << std::endl;
        if (envelope.message.properties._flags & AMQP_BASIC_CONTENT_TYPE_FLAG) {
            std::cout << "Content-type: "
                      << std::string(static_cast<char *>(envelope.message.properties.content_type.bytes),
                                     envelope.message.properties.content_type.len) << std::endl;
        }
        std::cout << std::string(static_cast<char *>(envelope.message.body.bytes), envelope.message.body.len)
                  << std::endl;

        // 释放消息
        amqp_destroy_envelope(&envelope);
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