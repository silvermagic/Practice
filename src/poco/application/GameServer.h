//
// Created by 范炜东 on 2019/4/23.
//

#ifndef PRACTICE_GAMESERVER_H
#define PRACTICE_GAMESERVER_H

#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>

using Poco::Util::Application;
using Poco::Util::ServerApplication;
using Poco::Util::OptionSet;

class GameServer : public ServerApplication {
public:
  GameServer();

  ~GameServer();

protected:
  void initialize(Application &self) override;

  void uninitialize() override;

  void defineOptions(OptionSet &options) override;

  void handleOption(const std::string &name, const std::string &value) override;

  int main(const std::vector<std::string> &args) override;

  void displayHelp();

private:
  bool help_;
};

#endif //PRACTICE_GAMESERVER_H
