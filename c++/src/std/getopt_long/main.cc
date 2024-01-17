#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <getopt.h>

int main(int argc, char *argv[]) {
    const char *optstring = "u:p:qh";
    struct option longopts[] = {
            {"user", required_argument, nullptr, 'u'}, //
            {"password", required_argument, nullptr, 'p'},
            {"quiet", no_argument, nullptr, 'q'},
            {"help", no_argument, nullptr, 'h'},
            {nullptr, 0, nullptr, 0}
    };

    int opt, longindex;
    while ((opt = getopt_long(argc, argv, optstring, longopts, &longindex)) != -1)
    {
        switch (opt)
        {
            case 'u':
                std::cout << "user: " << argv[optind - 1] << std::endl;
                break;
            case 'p':
                std::cout << "password: " << argv[optind - 1] << std::endl;
                break;
            case 'q':
                std::cout << "quiet" << std::endl;
                break;
            case 'h':
                std::cout << argv[0] << " [options]" << std::endl;
                std::cout << std::setw(2) << " " << "options:" << std::endl;
                std::cout << std::setw(4) << " " << "-u | --user=NAME" << "login name" << std::endl;
                std::cout << std::setw(4) << " " << "-p | --password=PWD" << "login password" << std::endl;
                std::cout << std::setw(4) << " " << "-q | --quiet" << "quiet mode" << std::endl;
                std::cout << std::setw(4) << " " << "-h | --help" << "this help" << std::endl;
                break;
            default:
                std::cout << "See --help." << std::endl;
                return -1;
        }
    }
    return 0;
}