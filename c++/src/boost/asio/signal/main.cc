//
// Created by 范炜东 on 2019/4/23.
//

#include <iostream>
#include <boost/asio.hpp>

int main() {
  boost::asio::io_context io_context;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type> guard(boost::asio::make_work_guard(io_context));
  boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
  signals.async_wait([&io_context](const boost::system::error_code& ec, int signal_number){
    std::cout << "exit" << std::endl;
    io_context.stop();
  });

  std::cout << "main loop" << std::endl;
  io_context.run();
}