//
// Created by 范炜东 on 2019/3/26.
//

#include <iostream>
#include <cstdlib>
#include <boost/asio.hpp>

boost::asio::io_context io_context; // 串行处理
boost::asio::io_context background_context; // 并行处理

// 串行处理查询结果
void modify(std::string name, int value) {
    static int object = 10;
    std::cout << name << " : " << object << "->" << object + value << std::endl;
    object += value;
}

int main() {
    std::srand(std::time(nullptr));

    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work(boost::asio::make_work_guard(io_context));
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> background_work(boost::asio::make_work_guard(background_context));

    // 使用线程池并行处理
    boost::asio::thread_pool pool(5);
    boost::asio::post(pool, [](){
        background_context.run();
    });
    boost::asio::post(pool, [](){
        background_context.run();
    });
    boost::asio::post(pool, [](){
        background_context.run();
    });
    boost::asio::post(pool, [](){
        background_context.run();
    });
    boost::asio::post(pool, [](){
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::cout << "stop" << std::endl;
        io_context.stop();
    });

    // 查询可以并行，但是查询结果的处理需要串行
    boost::asio::post(io_context, [](){
        std::cout << "exec db query a" << std::endl;
        boost::asio::post(background_context, [](){
            int sleep = std::rand() % 3;
            std::this_thread::sleep_for(std::chrono::seconds(sleep));
            boost::asio::post(io_context, std::bind(&modify, "a", sleep));
        });
    });

    boost::asio::post(io_context, [](){
        std::cout << "exec db query b" << std::endl;
        boost::asio::post(background_context, [](){
            int sleep = std::rand() % 3;
            std::this_thread::sleep_for(std::chrono::seconds(sleep));
            boost::asio::post(io_context, std::bind(&modify, "b", sleep));
        });
    });

    boost::asio::post(io_context, [](){
        std::cout << "exec db query c" << std::endl;
        boost::asio::post(background_context, [](){
            std::this_thread::sleep_for(std::chrono::seconds(2));
            boost::asio::post(io_context, std::bind(&modify, "c", 2));
        });
    });

    boost::asio::post(io_context, [](){
        std::cout << "exec db query d" << std::endl;
        boost::asio::post(background_context, [](){
            std::this_thread::sleep_for(std::chrono::seconds(2));
            boost::asio::post(io_context, std::bind(&modify, "d", 2));
        });
    });

    io_context.run();
    background_context.stop();
    pool.join();
}