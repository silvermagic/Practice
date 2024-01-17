#include <iostream>
#include <sstream>
#include <fstream>

int main() {
    // 文件写入
    std::ofstream ofs;
    ofs.open("example.txt");
    if (ofs.is_open()) {
        ofs << "Writing this to a file." << std::endl;
        ofs << "Writing this to a file again." << std::endl;
        ofs.close();
    } else {
        std::cout << "Fail to open the file for writing!" << std::endl;
    }

    // 文件读取
    std::ifstream ifs;
    ifs.open("example.txt");
    if (ifs.is_open()) {
        std::string line;
        while (getline(ifs, line)) {
            std::cout << line << std::endl;
        }

        // 一次读取整个文件
        std::stringstream buffer;
        ifs.clear();
        ifs.seekg(0, std::ios::beg);
        buffer << ifs.rdbuf();
        ifs.close();
        std::cout << "Read the entire file: " << std::endl;
        std::cout << buffer.str();
    } else {
        std::cout << "Fail to open the file for reading!" << std::endl;
    }

    return 0;
}