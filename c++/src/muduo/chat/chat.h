#include "codec.h"
#include "muduo/base/Mutex.h"
#include "muduo/base/ThreadLocalSingleton.h"
#include "muduo/base/noncopyable.h"
#include "muduo/net/Callbacks.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/TcpServer.h"
#include <vector>

class ChatServer : muduo::noncopyable {
  using ConnectionList = std::set<muduo::net::TcpConnectionPtr>;
  using ConnectionListPtr = std::shared_ptr<ConnectionList>;
  using LocalConnections = muduo::ThreadLocalSingleton<ConnectionList>;

public:
  ChatServer(int port, int threadNum);

  void start();

private:
  void threadInit(muduo::net::EventLoop *);
  void onConnection(const muduo::net::TcpConnectionPtr &conn);
  void onStringMessage(const muduo::net::TcpConnectionPtr &conn,
                       const std::string &message, muduo::Timestamp);
  void distributeMessage(const std::string &message);

private:
  muduo::net::EventLoop loop_;
  muduo::net::TcpServer server_;
  std::vector<muduo::net::EventLoop *> loops_;
  ChatCodec codec_;
};