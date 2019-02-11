#include <iomanip>
#include <iostream>
#include <endian.h>
#include "Poco/ByteOrder.h"

using Poco::ByteOrder;
using Poco::UInt16;
using Poco::Int16;
using Poco::UInt32;
using Poco::Int32;
using Poco::UInt64;
using Poco::Int64;

template<class T>
void dump(const char *type, T t) {
    int len = sizeof(T);
    std::cout << std::setfill(' ') << std::setw(14) << type << " : ";
    unsigned char *buffer = reinterpret_cast<unsigned char *>(&t);
    for (int i = 0; i < len; i++)
        std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2) << (static_cast<int>(buffer[i]) & 0xFF) << " ";
    std::cout << std::dec << std::endl;
}

int main() {
#ifdef POCO_ARCH_LITTLE_ENDIAN
    std::cout << "little endian" << std::endl;
#else
    std::cout << "big endian" << std::endl;
#endif

    // 无符号16位int
    {
        std::cout << std::setfill(' ') << std::setw(14) << 65535 << " : " << std::endl;
        dump("Initial", int(65535));
        // Poco实现
        {
            UInt16 nPort = ByteOrder::toNetwork(UInt16(65535));
            dump("Net:UInt16", nPort);
            UInt16 hPort = ByteOrder::fromNetwork(nPort);
            dump("Host:UInt16", hPort);
        }
        // C实现
        {

            uint16_t nPort = htobe16(uint16_t(65535));
            dump("Net:uint16_t", nPort);
            uint16_t hPort = be16toh(nPort);
            dump("Host:uint16_t", hPort);
        }
    }

    // 有符号16位int
    {
        std::cout << std::setfill(' ') << std::setw(14) << -32768 << " : " << std::endl;
        dump("Initial", int(-32768));
        // Poco实现
        {
            Int16 nPort = ByteOrder::toNetwork(Int16(-32768));
            dump("Net:Int16", nPort);
            Int16 hPort = ByteOrder::fromNetwork(nPort);
            dump("Host:Int16", hPort);
        }
        // C实现
        {
            uint16_t nPort = htobe16(uint16_t(-32768));
            dump("Net:int16_t", nPort);
            uint16_t hPort = be16toh(nPort);
            dump("Host:int16_t", (int16_t)hPort);
        }
    }

    // 无符号32位int
    {
        std::cout << std::setfill(' ') << std::setw(14) << 16777216 << " : " << std::endl;
        dump("Initial", int(16777216));
        // Poco实现
        {
            UInt32 nPort = ByteOrder::toNetwork(UInt32(16777216));
            dump("Net:UInt32", nPort);
            UInt32 hPort = ByteOrder::fromNetwork(nPort);
            dump("Host:UInt32", hPort);
        }
        // C实现
        {

            uint32_t nPort = htobe32(uint32_t(16777216));
            dump("Net:uint32_t", nPort);
            uint32_t hPort = be32toh(nPort);
            dump("Host:uint32_t", hPort);
        }
    }

    // 有符号32位int
    {
        std::cout << std::setfill(' ') << std::setw(14) << -16777216 << " : " << std::endl;
        dump("Initial", int(-16777216));
        // Poco实现
        {
            Int32 nPort = ByteOrder::toNetwork(Int32(-16777216));
            dump("Net:Int32", nPort);
            Int32 hPort = ByteOrder::fromNetwork(nPort);
            dump("Host:Int32", hPort);
        }
        // C实现
        {
            uint32_t nPort = htobe32(uint32_t(-16777216));
            dump("Net:int32_t", nPort);
            uint32_t hPort = be32toh(nPort);
            dump("Host:int32_t", (int32_t)hPort);
        }
    }

    // 无符号64位int
    {
        std::cout << std::setfill(' ') << std::setw(14) << (unsigned long long)(281474976710656) << " : " << std::endl;
        dump("Initial", (unsigned long long)(281474976710656));
        // Poco实现
        {
            UInt64 nPort = ByteOrder::toNetwork(UInt64(281474976710656));
            dump("Net:UInt64", nPort);
            UInt64 hPort = ByteOrder::fromNetwork(nPort);
            dump("Host:UInt64", hPort);
        }
        // C实现
        {

            uint64_t nPort = htobe64(uint64_t(281474976710656));
            dump("Net:uint64_t", nPort);
            uint64_t hPort = be64toh(nPort);
            dump("Host:uint64_t", hPort);
        }
    }

    // 有符号64位int
    {
        std::cout << std::setfill(' ') << std::setw(14) << (long long)(-281474976710656) << " : " << std::endl;
        dump("Initial", (long long)(-281474976710656));
        // Poco实现
        {
            Int64 nPort = ByteOrder::toNetwork(Int64(-281474976710656));
            dump("Net:Int64", nPort);
            Int64 hPort = ByteOrder::fromNetwork(nPort);
            dump("Host:Int64", hPort);
        }
        // C实现
        {
            uint64_t nPort = htobe64(uint64_t(-281474976710656));
            dump("Net:int64_t", nPort);
            uint64_t hPort = be64toh(nPort);
            dump("Host:int64_t", (int64_t)hPort);
        }
    }
    return 0;
}