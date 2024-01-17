//
// Created by 范炜东 on 2019/4/22.
//

#include <iostream>
#include <boost/program_options.hpp>
#include <Poco/AutoPtr.h>
#include <Poco/Logger.h>
#include <Poco/Util/LoggingConfigurator.h>
#include <Poco/Util/PropertyFileConfiguration.h>

using Poco::AutoPtr;
using Poco::Logger;
using Poco::Util::LoggingConfigurator;
using Poco::Util::PropertyFileConfiguration;


int main(int argc, char *argv[]) {
  boost::program_options::options_description cmdline_options("命令行参数");
  cmdline_options.add_options()
          ("help", "帮助信息")
          ("config-file", boost::program_options::value<std::string>(), "设置自定义配置文件");
  try {
    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, cmdline_options), vm);
    boost::program_options::notify(vm);
    if (vm.count("help")) {
      std::cout << cmdline_options << std::endl;
      return 1;
    }

    if (argc < 2) {
      std::cerr << "Missing configuration path!" << std::endl;
      return -1;
    }
    std::string properties_file = vm["config-file"].as<std::string>() + "log.properties";
    std::cout << "Loading configuration file: " << properties_file << std::endl;
    AutoPtr<PropertyFileConfiguration> pConf = new PropertyFileConfiguration(properties_file);
    LoggingConfigurator cfg;
    cfg.configure(pConf);

    Logger &basic = Logger::root();
    Logger &chat = Logger::get("chat");
    Logger &server = Logger::get("server");

    // 输出到屏幕
    basic.error("root error", __FILE__, __LINE__);
    basic.information("root information", __FILE__, __LINE__);

    // 输出到文件
    chat.information("chat information", __FILE__, __LINE__);
    chat.debug("chat debug", __FILE__, __LINE__);

    // 输出到文件 & 文件自动备份
    for (int i = 0; i < 20; i++)
      server.information("server error", __FILE__, __LINE__);

  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}