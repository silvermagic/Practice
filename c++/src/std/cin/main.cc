#include <iostream>
#include <sstream>
#include <locale>
#include <iomanip>

int main() {
    std::istringstream in("3.14159 true +10 2019-1-26 09:30:00");
    in.imbue(std::locale("en_US.UTF-8"));
    // 浮点数
    float f;
    in >> f;
    std::cout << f << std::endl;
    // 布尔
    bool b;
    in >> std::boolalpha >> b;
    std::cout << std::boolalpha << b << std::endl;
    // 整数
    int i;
    in >> std::showpos>> i;
    std::cout << i << std::endl;
    // 时间（需要GCC 5版本支持）
    //std::tm t;
    //in >> std::get_time(&t, "%Y-%m-%s %H:%M:%S");
    //std::cout << std::asctime(&t) << std::endl;
    return 0;
}