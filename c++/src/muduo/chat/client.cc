#include "codec.h"
#include "muduo/base/Mutex.h"
#include "muduo/base/StringPiece.h"
#include "muduo/base/noncopyable.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/EventLoopThread.h"
#include "muduo/net/TcpClient.h"
#include <functional>
#include <iostream>

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

class ChatClient : muduo::noncopyable {
public:
  ChatClient(int port)
      : loop_(thread_.startLoop()),
        client_(loop_, muduo::net::InetAddress(port), "ChatClient"),
        codec_(bind(&ChatClient::onStringMessage, this, _1, _2, _3)) {
    client_.setConnectionCallback(
        std::bind(&ChatClient::onConnection, this, _1));
    client_.setMessageCallback(
        std::bind(&ChatCodec::onMessage, &codec_, _1, _2, _3));
    client_.enableRetry();
  }

  void connect() { client_.connect(); }

  void disconnect() { client_.disconnect(); }

  void write(const muduo::StringPiece &message) {
    muduo::MutexLockGuard lock(mutex_);
    if (connection_) {
      codec_.send(connection_, message);
    }
  }

private:
  void onConnection(const muduo::net::TcpConnectionPtr &conn) {
    LOG_INFO << conn->localAddress().toIpPort() << " -> "
             << conn->peerAddress().toIpPort() << " is "
             << (conn->connected() ? "UP" : "DOWN");

    muduo::MutexLockGuard lock(mutex_);
    if (conn->connected()) {
      connection_ = conn;
    } else {
      connection_.reset();
    }
  }

  void onStringMessage(const muduo::net::TcpConnectionPtr &conn,
                       const std::string &message, muduo::Timestamp) {
    printf("<<< %s\n", message.c_str());
  }

private:
  muduo::net::EventLoopThread thread_;
  muduo::net::EventLoop *loop_;
  muduo::net::TcpClient client_;
  ChatCodec codec_;

  muduo::MutexLock mutex_;
  muduo::net::TcpConnectionPtr connection_ GUARDED_BY(mutex_);
};

int main(int argc, char *argv[]) {
  ChatClient client(2000);
  client.connect();
  std::string line;
  while (std::getline(std::cin, line)) {
    client.write(line);
  }
  client.disconnect();
}