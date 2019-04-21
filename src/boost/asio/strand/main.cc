//
// Created by 范炜东 on 2019/3/21.
//

#include <iostream>
#include <chrono>
#include <thread>
#include <memory>
#include <boost/asio.hpp>

boost::asio::io_context io_context;

int main()
{
    boost::asio::thread_pool pool(6);
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> guard(boost::asio::make_work_guard(io_context));
    boost::asio::io_context::strand a_obj(io_context);
    boost::asio::io_context::strand b_obj(io_context);

    // 保证对象间处理是串行的
    boost::asio::post(pool, [](){
        io_context.run();
    });
    boost::asio::post(pool, [](){
        io_context.run();
    });

    //（一）对象a的加速、缓速状态生效，以及对象b的加速、缓速状态生效打印同时出现，说明处理都是并行的，即线程池是生效的
    //（二）对象a的加速状态移除，以及对象b的加速、缓速状态移除打印同时出现，说明对象间处理是并行的
    //（三）对象a的缓速状态移除在3秒后才出现，说明对象内处理是串行的，即strand生效了
    boost::asio::post(a_obj, [&a_obj](){
        std::cout << "Apply acceleration to the object a" << std::endl;
        auto timer = std::make_shared<boost::asio::basic_waitable_timer<std::chrono::steady_clock>>(io_context);
        timer->expires_after(std::chrono::seconds(2));
        timer->async_wait(boost::asio::bind_executor(a_obj, [timer](const boost::system::error_code &ec){
            if (!ec) {
                std::cout << "Remove the acceleration state of the object a" << std::endl;
                // 休眠一秒，（一）验证对象a阻塞是否会影响对象b的处理，（二）验证对象a的处理是为否串行
                std::this_thread::sleep_for(std::chrono::seconds(3));
            } else if (ec == boost::asio::error::operation_aborted) {
                std::cerr << "aborted" << std::endl;
            } else {
                std::cerr << "unknow" << std::endl;
            }
        }));
    });
    boost::asio::post(b_obj, [&b_obj](){
        std::cout << "Apply acceleration to the object b" << std::endl;
        auto timer = std::make_shared<boost::asio::basic_waitable_timer<std::chrono::steady_clock>>(io_context);
        timer->expires_after(std::chrono::seconds(2));
        timer->async_wait(boost::asio::bind_executor(b_obj, [timer](const boost::system::error_code &ec){
            if (!ec) {
                std::cout << "Remove the acceleration state of the object b" << std::endl;
            } else if (ec == boost::asio::error::operation_aborted) {
                std::cerr << "aborted" << std::endl;
            } else {
                std::cerr << "unknow" << std::endl;
            }
        }));
    });

    boost::asio::post(a_obj, [&a_obj](){
        std::cout << "Apply slow to the object a" << std::endl;
        auto timer = std::make_shared<boost::asio::basic_waitable_timer<std::chrono::steady_clock>>(io_context);
        timer->expires_after(std::chrono::seconds(2));
        timer->async_wait(boost::asio::bind_executor(a_obj, [timer](const boost::system::error_code &ec){
            if (!ec) {
                // 对象b的缓速状态移除紧随加速状态移除，说明对象a处理阻塞不影响对象b的处理，说明对象a和对象b的处理是并行的
                std::cout << "Remove the slow state of the object a" << std::endl;
            } else if (ec == boost::asio::error::operation_aborted) {
                std::cerr << "aborted" << std::endl;
            } else {
                std::cerr << "unknow" << std::endl;
            }
        }));
    });
    boost::asio::post(b_obj, [&b_obj](){
        std::cout << "Apply slow to the object b" << std::endl;
        auto timer = std::make_shared<boost::asio::basic_waitable_timer<std::chrono::steady_clock>>(io_context);
        timer->expires_after(std::chrono::seconds(2));
        timer->async_wait(boost::asio::bind_executor(b_obj, [timer](const boost::system::error_code &ec){
            if (!ec) {
                // 等待2秒后才出现，说明对象a的处理是串行的，说明strand生效了
                std::cout << "Remove the slow state of the object b" << std::endl;
            } else if (ec == boost::asio::error::operation_aborted) {
                std::cerr << "aborted" << std::endl;
            } else {
                std::cerr << "unknow" << std::endl;
            }
        }));
    });

    boost::asio::post(pool, [](){
        std::this_thread::sleep_for(std::chrono::seconds(6));
        std::cout << "stop" << std::endl;
        io_context.stop();
    });
    pool.join();
}