//
// Created by 范炜东 on 2019/3/21.
//

#include <iostream>
#include <chrono>
#include <thread>
#include <boost/asio.hpp>

boost::asio::io_context io_context;
boost::asio::basic_waitable_timer<std::chrono::steady_clock> timer(io_context);

// 在定时器处理函数中更新定时器，实现周期型定时器
void handle_timeout(const boost::system::error_code& ec)
{
    if (!ec) {
        if (timer.expires_at() != std::chrono::steady_clock::time_point::min())
        {
            std::cout << std::this_thread::get_id() << " -> " << "handle_timeout" << std::endl;
            timer.expires_from_now(std::chrono::seconds(1));
            timer.async_wait(&handle_timeout);
        } else {
            std::cerr << std::this_thread::get_id() << " -> " << "shutdown" << std::endl;
        }
        /*std::cout << std::this_thread::get_id() << " -> " << "handle_timeout" << std::endl;
        timer.expires_from_now(std::chrono::seconds(1));
        timer.async_wait(&handle_timeout);*/
    } else if (ec != boost::asio::error::operation_aborted) {
        std::cerr << std::this_thread::get_id() << " -> " << ec.message() << std::endl;
    } else {
        std::cerr << std::this_thread::get_id() << " -> " << "cancel" << std::endl;
    }
}

int main()
{
    boost::asio::thread_pool pool(5);
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> guard(boost::asio::make_work_guard(io_context));

    // 启动周期型定时器
    timer.expires_after(std::chrono::seconds(1));
    // 使用线程池处理耗时请求，防止阻塞io_context
    boost::asio::post(pool, [](){
        std::cout << std::this_thread::get_id() << " -> " << "wait timer" << std::endl;
        timer.async_wait(&handle_timeout);
    });
    boost::asio::post(pool, [](){
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << std::this_thread::get_id() << " -> " << "db query a" << std::endl;
    });
    boost::asio::post(pool, [](){
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << std::this_thread::get_id() << " -> " << "db query b" << std::endl;
    });
    boost::asio::post(pool, [](){
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << std::this_thread::get_id() << " -> " << "db query c" << std::endl;
    });
    // 安全的取消周期型定时器
    boost::asio::post(pool, [](){
        std::this_thread::sleep_for(std::chrono::seconds(3));
        boost::asio::dispatch(timer.get_executor(), []() {
            std::cout << std::this_thread::get_id() << " -> " << "timer stop" << std::endl;
            timer.expires_at(std::chrono::steady_clock::time_point::min());
            // 直接cancel的方式有一定概率取消不了
            //std::cout << "Cancel " << timer.cancel() << " timers" << std::endl;
        });
    });
    // 关闭io_context
    boost::asio::post(pool, [](){
        std::this_thread::sleep_for(std::chrono::seconds(6));
        std::cout << std::this_thread::get_id() << " -> " << "stop" << std::endl;
        io_context.stop();
    });
    io_context.run();
    pool.join();
}