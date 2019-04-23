//
// Created by 范炜东 on 2019/4/23.
//

#include <iostream>
#include "GameServer.h"

using Poco::Exception;
using Poco::Util::Application;

int main(int argc, char **argv) {
  try {
    GameServer app;
    return app.run(argc, argv);
  }
  catch (Exception &exc) {
    std::cerr << exc.displayText() << std::endl;
    return Application::EXIT_SOFTWARE;
  }
}