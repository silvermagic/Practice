#include "echo.h"
#include <muduo/net/EventLoop.h>

int main(int argc, char *argv[]) {
  EchoServer server(2000, 4);
  server.start();
  return 0;
}