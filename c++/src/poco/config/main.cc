//
// Created by 范炜东 on 2019/4/22.
//

#include <iostream>
#include <exception>
#include <boost/program_options.hpp>
#include <Poco/Util/SystemConfiguration.h>
#include <Poco/Util/LayeredConfiguration.h>
#include <Poco/Util/IniFileConfiguration.h>
#include <Poco/Util/PropertyFileConfiguration.h>

using Poco::AutoPtr;
using Poco::Util::SystemConfiguration;
using Poco::Util::LayeredConfiguration;
using Poco::Util::IniFileConfiguration;
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

    // 环境信息
    AutoPtr <SystemConfiguration> sysInfo = new SystemConfiguration();
    std::cout << "==========System environment==========" << std::endl;
    std::cout << sysInfo->getString("system.osName") << std::endl;

    if (argc < 2) {
      std::cerr << "Missing configuration path!" << std::endl;
      return -1;
    }

    std::string ini_file = vm["config-file"].as<std::string>() + "config.ini";
    std::string properties_file = vm["config-file"].as<std::string>() + "config.properties";
    std::cout << "Loading configuration file: " << ini_file << std::endl;
    std::cout << "Loading configuration file: " << properties_file << std::endl;
    AutoPtr<IniFileConfiguration> pConfIni(new IniFileConfiguration(ini_file));
    AutoPtr<PropertyFileConfiguration> pConfPrp(new PropertyFileConfiguration(properties_file));

    // 键值大小写、不存在键值
    std::cout << "==========Single config==========" << std::endl;
    std::cout << pConfIni->getString("Section.key_str") << std::endl;
    std::cout << pConfIni->getInt("Section.key_int") << std::endl;
    std::cout << pConfIni->getInt("Section.key_unknow", -1) << std::endl;
    std::cout << pConfPrp->getInt("key_ext") << std::endl;

    // 多配置文件
    std::cout << "==========Multiple config==========" << std::endl;
    AutoPtr<LayeredConfiguration> pConf = new LayeredConfiguration();
    pConf->addWriteable(pConfIni, 0);
    pConf->addWriteable(pConfPrp, 0);
    std::cout << pConf->getString("Section.key_str") << std::endl;
    std::cout << pConf->getInt("Section.key_int") << std::endl;
    std::cout << pConf->getInt("Section.key_unknow", -1) << std::endl;
    std::cout << pConf->getInt("key_ext") << std::endl;
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}