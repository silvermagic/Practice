#include <iostream>
#include <iomanip>

int main() {
    // 十六进制显示 + 大小写 + 进制前缀显示 + 位置控制 + 宽度控制
    std::cout << "value: " << std::hex << std::uppercase << 10 << " : " << std::nouppercase << std::showbase << std::setw(16) << std::setfill('0') << std::internal << 10 << std::endl;
    // 精度控制
    std::cout << "value: " << 3.141592653589793239 << " : " << std::setprecision(10) << 3.141592653589793239 << std::endl;
    // 布尔值显示
    std::cout << "value: " << std::boolalpha << true << std::endl;
    // 正符号显示
    std::cout << "value: " << std::dec << std::showpos << 0 << " : " << std::noshowpos << 0 << std::endl;
    return 0;
}