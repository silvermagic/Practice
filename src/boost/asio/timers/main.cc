//
// Created by 范炜东 on 2019/3/21.
//

#include <iostream>
#include <boost/asio.hpp>

int main()
{
    boost::asio::io_context io_context;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> guard(boost::asio::make_work_guard(io_context));

    boost::asio::steady_timer timer(io_context);
    // 同步定时器
    std::cout << "Start timer" << std::endl;
    timer.expires_after(std::chrono::seconds(2));
    timer.wait();
    std::cout << "Stop timer" << std::endl;

    // 取消定时器
    std::cout << "Cancel " << timer.cancel() << " timers" << std::endl;
    timer.expires_after(std::chrono::seconds(2));
    timer.async_wait([](const boost::system::error_code &ec){
        if (!ec) {
            std::cout << "timeout" << std::endl;
        } else if (ec == boost::asio::error::operation_aborted) {
            std::cerr << "aborted" << std::endl;
        } else {
            std::cerr << "unknow" << std::endl;
        }
    });
    std::cout << "Cancel " << timer.cancel() << " timers" << std::endl;

    // 异步定时器
    std::cout << "start async timer" << std::endl;
    timer.expires_after(std::chrono::seconds(2));
    timer.async_wait([&io_context](const boost::system::error_code &ec){
        io_context.stop();
        std::cout << "stop async timer" << std::endl;
    });

    io_context.run();
}