#include "chat.h"
#include "muduo/base/Logging.h"
#include "muduo/net/EventLoopThreadPool.h"
#include <cassert>
#include <functional>
#include <muduo/base/Mutex.h>

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

ChatServer::ChatServer(int port, int threadNum)
    : server_(&loop_, muduo::net::InetAddress(port), "ChatServer"),
      codec_(std::bind(&ChatServer::onStringMessage, this, _1, _2, _3)) {
  server_.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));
  server_.setMessageCallback(
      std::bind(&ChatCodec::onMessage, &codec_, _1, _2, _3));
  server_.setThreadNum(threadNum);
}

void ChatServer::start() {
  server_.start();
  loops_ = server_.threadPool()->getAllLoops();
  loop_.loop();
}

void ChatServer::threadInit(muduo::net::EventLoop *loop) {
  assert(LocalConnections::pointer() == NULL);
  LocalConnections::instance();
  assert(LocalConnections::pointer() != NULL);
}

void ChatServer::onConnection(const muduo::net::TcpConnectionPtr &conn) {
  LOG_INFO << conn->peerAddress().toIpPort() << " -> "
           << conn->localAddress().toIpPort() << " is "
           << (conn->connected() ? "UP" : "DOWN");
  if (conn->connected()) {
    LocalConnections::instance().insert(conn);
  } else {
    LocalConnections::instance().erase(conn);
  }
}

void ChatServer::onStringMessage(const muduo::net::TcpConnectionPtr &conn,
                                 const std::string &message, muduo::Timestamp) {
  muduo::net::EventLoop::Functor f =
      std::bind(&ChatServer::distributeMessage, this, message);

  for (auto loop : loops_) {
    loop->queueInLoop(f);
  }
}

void ChatServer::distributeMessage(const std::string &message) {
  for (auto it = LocalConnections::instance().begin();
       it != LocalConnections::instance().end(); ++it) {
    codec_.send(*it, message);
  }
}
