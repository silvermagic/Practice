#include <iostream>
#include "Poco/Any.h"
#include "Poco/DynamicAny.h"
#include "Poco/Exception.h"

using Poco::Any;
using Poco::AnyCast;
using Poco::RefAnyCast;
using Poco::DynamicAny;

int main() {
    {
        Any any(42);
        int i = AnyCast<int>(any);
        int& ri = RefAnyCast<int>(any);
        try {
            short s = AnyCast<short>(any); // throws BadCastException
        } catch (Poco::BadCastException &) {
            std::cout << "AnyCast failed" << std::endl;
        }
    }

    // 动态任意类型，
    {
        DynamicAny any(42);
        std::string s = any;
        std::cout << "explicit convert string "<< s << std::endl;
        s = any.convert<std::string>();
        std::cout << "implicit convert string "<< s << std::endl;
        const int& ri = any.extract<int>();
        std::cout << "extract short " << ri << std::endl;
        short si = any;
        std::cout << "implicit convert short " << si << std::endl;
        try {
            short s = any.extract<short>(); // throws BadCastException
        } catch (Poco::BadCastException &) {
            std::cout << "extract failed" << std::endl;
        }
    }
    return 0;
}