#include "echo.h"
#include "muduo/base/Logging.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/TcpServer.h"
#include <functional>

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

EchoServer::EchoServer(int port, int threadNum)
    : server_(&loop_, muduo::net::InetAddress(port), "EchoServer") {
  server_.setConnectionCallback(std::bind(&EchoServer::onConnection, this, _1));
  server_.setMessageCallback(
      std::bind(&EchoServer::onMessage, this, _1, _2, _3));
  server_.setThreadNum(threadNum);
}

void EchoServer::start() {
  server_.start();
  loop_.loop();
}

void EchoServer::onConnection(const muduo::net::TcpConnectionPtr &conn) {
  LOG_INFO << "EchoServer - " << conn->localAddress().toIpPort() << " -> "
           << conn->peerAddress().toIpPort() << " is "
           << (conn->connected() ? "UP" : "DOWN");
}

void EchoServer::onMessage(const muduo::net::TcpConnectionPtr &conn,
                           muduo::net::Buffer *buf,
                           muduo::Timestamp receiveTime) {
  muduo::string msg(buf->retrieveAllAsString());
  LOG_INFO << conn->name() << " echo " << msg.size() << " bytes, "
           << "data received at " << receiveTime.toString();
  conn->send(msg);
}