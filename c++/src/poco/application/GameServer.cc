//
// Created by 范炜东 on 2019/4/23.
//

#include <iostream>
#include <Poco/ThreadPool.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Net/TCPServer.h>
#include <Poco/Net/TCPServerParams.h>
#include <Poco/Net/ServerSocket.h>
#include "Connection.h"
#include "GameServer.h"

using Poco::ThreadPool;
using Poco::Net::TCPServer;
using Poco::Net::TCPServerParams;
using Poco::Net::ServerSocket;
using Poco::Util::Option;
using Poco::Util::HelpFormatter;

GameServer::GameServer() :
        help_(false) {
}

GameServer::~GameServer() {
}

void GameServer::initialize(Application &self) {
  loadConfiguration(); // load default configuration files, if present
  ServerApplication::initialize(self);

  logger().information("starting up");
}

void GameServer::uninitialize() {
  logger().information("shutting down");
  ServerApplication::uninitialize();
}

void GameServer::defineOptions(OptionSet &options) {
  ServerApplication::defineOptions(options);

  options.addOption(
          Option("help", "h", "display help information on command line arguments")
                  .required(false)
                  .repeatable(false));

  options.addOption(
          Option("config-file", "f", "load configuration data from a file")
                  .required(false)
                  .repeatable(true)
                  .argument("file"));
}

void GameServer::handleOption(const std::string &name, const std::string &value) {
  ServerApplication::handleOption(name, value);

  if (name == "help") {
    help_ = true;
    stopOptionsProcessing();
  } else if (name == "config-file")
    loadConfiguration(value);
}

void GameServer::displayHelp() {
  HelpFormatter helpFormatter(options());
  helpFormatter.setCommand(commandName());
  helpFormatter.setUsage("OPTIONS");
  helpFormatter.setHeader("A sample server application that demonstrates some of the features of the Util::ServerApplication class.");
  helpFormatter.format(std::cout);
}

int GameServer::main(const std::vector<std::string> &args) {
  if (help_) {
    displayHelp();
  } else {
    // 全局变量初始化

    // get parameters from configuration file
    unsigned short port = (unsigned short) config().getInt("Server.port", 2000);
    // set-up a server socket
    ServerSocket svs(port);
    // set-up a TCPServerParams
    TCPServerParams *pParams = new TCPServerParams();
    pParams->setMaxThreads(1000);
    // set-up a TCPServer instance
    TCPServer srv(new ConnectionFactory(), svs, pParams);
    // start the TCPServer
    srv.start();
    // wait for CTRL-C or kill
    waitForTerminationRequest();
    // Stop the TCPServer
    srv.stop();
    // 等待所有处理线程退出
    ThreadPool::defaultPool().joinAll();
  }
  return Application::EXIT_OK;
}