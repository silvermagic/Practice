//
// Created by 范炜东 on 2019/4/23.
//

#include <Poco/Util/Application.h>
#include "Connection.h"

using Poco::Util::Application;

Connect::Connect(const StreamSocket &s) :
        TCPServerConnection(s) {
}

void Connect::run() {
  Application &app = Application::instance();
  app.logger().information("请求来自: " + socket().peerAddress().toString());
  try {
    std::string data = "hello world";
    int n = socket().sendBytes(data.c_str(), data.length() + 1);
    poco_assert(n == static_cast<int>(data.length() + 1));
  }
  catch (Poco::Exception &e) {
    app.logger().log(e);
  }
  app.logger().information("请求处理结束");
}