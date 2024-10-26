#include "muduo/base/Logging.h"
#include "muduo/base/noncopyable.h"
#include "muduo/net/Buffer.h"
#include "muduo/net/Endian.h"
#include "muduo/net/TcpConnection.h"
#include <cstdint>

class ChatCodec : muduo::noncopyable {
public:
  using StringMessageCallback =
      std::function<void(const muduo::net::TcpConnectionPtr &,
                         const std::string &, muduo::Timestamp)>;

  explicit ChatCodec(const StringMessageCallback &cb) : messageCallback_(cb) {}

  void onMessage(const muduo::net::TcpConnectionPtr &conn,
                 muduo::net::Buffer *buf, muduo::Timestamp receiveTime) {
    while (buf->readableBytes() >= kHeaderLen) {
      const int32_t len = buf->peekInt32();
      if (len > 65536 || len < 0) {
        LOG_ERROR << "Invalid length " << len;
        conn->shutdown();
        break;
      } else if (buf->readableBytes() >= len + kHeaderLen) {
        buf->retrieve(kHeaderLen);
        std::string message(buf->peek(), len);
        buf->retrieve(len);
        messageCallback_(conn, message, receiveTime);
      } else {
        break;
      }
    }
  }

  void send(const muduo::net::TcpConnectionPtr &conn,
            const muduo::StringPiece &message) {
    muduo::net::Buffer buf;
    buf.append(message.data(), message.size());
    int32_t len = static_cast<int32_t>(message.size());
    int32_t be32 = muduo::net::sockets::hostToNetwork32(len);
    buf.prepend(&be32, sizeof(be32));
    conn->send(&buf);
  }

private:
  StringMessageCallback messageCallback_;
  const static size_t kHeaderLen = sizeof(int32_t);
};