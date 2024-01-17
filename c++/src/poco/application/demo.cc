//
// Created by 范炜东 on 2019/4/23.
//

#include <iostream>
#include <exception>
#include <boost/program_options.hpp>
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/StreamSocket.h"

using Poco::Net::SocketAddress;
using Poco::Net::StreamSocket;

int main(int argc, char *argv[]) {
  boost::program_options::options_description cmdline_options("命令行参数");
  cmdline_options.add_options()
          ("port", boost::program_options::value<int>()->default_value(2000), "服务器端口");

  try {
    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, cmdline_options), vm);
    boost::program_options::notify(vm);

    SocketAddress sa("127.0.0.1", vm["port"].as<int>());
    StreamSocket socket(sa);
    char buffer[64] = {};
    int n = socket.receiveBytes(buffer, sizeof(buffer));
    if (n > 0)
      std::cout << buffer << std::endl;
  } catch (std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }

  return 0;
}