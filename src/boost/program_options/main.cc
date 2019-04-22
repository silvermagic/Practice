//
// Created by 范炜东 on 2019/4/22.
//

#include <iostream>
#include <exception>
#include <vector>
#include <fstream>
#include <boost/program_options.hpp>

int main(int argc, char *argv[]) {
  std::string HOST = "localhost";
  unsigned int PORT = 3306;
  int PRINCE_MAX_HP = 1000;

  // 配置文件参数定义
  boost::program_options::options_description server_desc("服务器设定");
  server_desc.add_options()
          ("Host", boost::program_options::value<std::string>(&HOST), "服务器地址")
          ("Port", boost::program_options::value<unsigned int>(&PORT), "服务器端口");
  boost::program_options::options_description char_desc("游戏角色设定");
  char_desc.add_options()
          ("PrinceMaxHP", boost::program_options::value<int>(&PRINCE_MAX_HP), "王族最大血量");
  boost::program_options::options_description cfgfile_options("配置文件参数");
  cfgfile_options.add(server_desc).add(char_desc);

  // 命令行参数定义
  boost::program_options::options_description cmdline_options("命令行参数");
  cmdline_options.add_options()
          ("config-file", boost::program_options::value < std::vector < std::string >> (), "设置自定义配置文件");

  try {
    boost::program_options::variables_map vm;
    // 解析命令行参数
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, cmdline_options), vm);
    if (vm.count("config-file")) {
      auto files = vm["config-file"].as<std::vector<std::string>>();
      for (auto file : files) {
        std::cout << "Loading configuration file: " << file << std::endl;
        std::ifstream ifs(file.c_str());
        // 解析配置文件参数
        boost::program_options::store(parse_config_file(ifs, cfgfile_options), vm);
      }
    }
    boost::program_options::notify(vm);

    // 打印参数
    std::cout << "Host: " << HOST << std::endl;
    std::cout << "Port: " << PORT << std::endl;
    std::cout << "Prince Max HP: " << PRINCE_MAX_HP << std::endl;
  } catch (std::exception &e) {
    std::cerr << "Loading configuration file failed!(" << e.what() << ")" << std::endl;
  }

  return 0;
}