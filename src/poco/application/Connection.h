//
// Created by 范炜东 on 2019/4/23.
//

#ifndef PRACTICE_CONNECTION_H
#define PRACTICE_CONNECTION_H

#include <Poco/Net/TCPServerConnection.h>
#include <Poco/Net/TCPServerConnectionFactory.h>
#include <Poco/Net/ServerSocket.h>

using Poco::Net::StreamSocket;
using Poco::Net::TCPServerConnection;
using Poco::Net::TCPServerConnectionFactory;

class Connect : public TCPServerConnection {
public:
  Connect(const StreamSocket &s);

  // 处理线程
  void run() override;
};

class ConnectionFactory : public TCPServerConnectionFactory {
public:
  ConnectionFactory() {}

  Connect *createConnection(const StreamSocket &socket) override {
    return new Connect(socket);
  }
};

#endif //PRACTICE_CONNECTION_H
