//
// Created by 范炜东 on 2019/4/23.
//

#include <iostream>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

int main(int argc, char *argv[]) {
  boost::log::add_console_log(std::cout, boost::log::keywords::format = "[%TimeStamp%]: %Message%");
  boost::log::add_file_log(
          boost::log::keywords::file_name = "sample_%N.log",
          boost::log::keywords::rotation_size = 10 * 1024 * 1024,
          boost::log::keywords::format = "[%TimeStamp%]: %Message%"
  );
  boost::log::core::get()->set_filter(
          boost::log::trivial::severity >= boost::log::trivial::info
  );
  boost::log::add_common_attributes();

  BOOST_LOG_TRIVIAL(trace) << "A trace severity message";
  BOOST_LOG_TRIVIAL(debug) << "A debug severity message";
  BOOST_LOG_TRIVIAL(info) << "An informational severity message";
  BOOST_LOG_TRIVIAL(warning) << "A warning severity message";
  BOOST_LOG_TRIVIAL(error) << "An error severity message";
  BOOST_LOG_TRIVIAL(fatal) << "A fatal severity message";

  return 0;
}