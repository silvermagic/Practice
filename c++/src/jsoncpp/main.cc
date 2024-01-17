#include <iostream>
#include <fstream>
#include <string>
#include "json/json.h"

const char *style = "{ \
    \"encoding\" : \"UTF-8\", \
    \"plug-ins\" : [ \
        \"python\", \
        \"c++\", \
        \"ruby\" \
        ], \
    \"indent\" : {\"length\" : 3, \"use_space\": true } \
}";

int main() {
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(style, root)) {
        std::cout << "parse fails." << std::endl;
        return -1;
    }
    std::cout << "Json before modify:" << std::endl;
    std::cout << style << std::endl;

    std::cout << "Json data display:" << std::endl;
    // 普通KV类型数据的读取
    std::string encoding = root.get("encoding", "GBK").asString();
    std::cout << "encoding : " << encoding << std::endl;
    // 数组类型数据的读取
    const Json::Value plugins = root["plug-ins"];
    std::cout << "plug-ins : [ ";
    for (int index = 0; index < (int)plugins.size(); ++index) {
        std::cout << plugins[index].asString();
        if (index != (int)plugins.size())
            std::cout << ", ";
    }
    std::cout << " ]" << std::endl;
    // 字典类型数据的读取和修改
    root["indent"]["length"] = 4;
    root["indent"]["use_space"] = false;

    // 打印修改后的JSON串
    std::cout << "Json after modify:" << std::endl;
    Json::StyledStreamWriter output;
    output.write(std::cout, root);

    // 输出JSON串到文件
    std::ofstream ofs;
    ofs.open("style.json");
    if (ofs.is_open()) {
        output.write(ofs, root);
        ofs.close();

        std::ifstream ifs;
        ifs.open("style.json");
        if (ifs.is_open()) {
            std::cout << "Json from file:" << std::endl;
            std::string line;
            while (getline(ifs, line)) {
                std::cout << line << std::endl;
            }
            ifs.clear();
            ifs.seekg(0, std::ios::beg);
            if (!reader.parse(ifs, root)) {
                std::cout << "parse fails." << std::endl;
                return -1;
            }
            std::cout << "Json parsed from file:" << std::endl;
            output.write(std::cout, root);
        } else {
            std::cout << "Fail to open the file for reading!" << std::endl;
        }
    } else {
        std::cout << "Fail to open the file for writing!" << std::endl;
    }

    return 0;
}
