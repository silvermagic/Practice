#include "muduo/base/noncopyable.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/TcpServer.h"

class EchoServer : muduo::noncopyable {
public:
  EchoServer(int port, int threadNum);

  void start();

private:
  void onConnection(const muduo::net::TcpConnectionPtr &conn);
  void onMessage(const muduo::net::TcpConnectionPtr &conn,
                 muduo::net::Buffer *buf, muduo::Timestamp time);

  muduo::net::EventLoop loop_;
  muduo::net::TcpServer server_;
};