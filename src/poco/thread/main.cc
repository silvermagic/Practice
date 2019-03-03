#include <iostream>
#include "Poco/Thread.h"
#include "Poco/Runnable.h"
#include "Poco/RunnableAdapter.h"
#include "Poco/ThreadLocal.h"
#include "Poco/ThreadPool.h"
#include "Poco/ErrorHandler.h"

class HelloRunnable: public Poco::Runnable
{
    virtual void run()
    {
        std::cout << "Hello, world!" << std::endl;
    }
};

class Greeter
{
public:
    void greet()
    {
        std::cout << "Hello, world!" << std::endl;
    }
};

class Counter : public Poco::Runnable
{

public:
    Counter(std::string name): name_(name)
    {
    }

protected:
    virtual void run()
    {
        {
            static Poco::ThreadLocal<int> tls;
            for (*tls = 0; *tls < 5; ++(*tls))
            {
                std::cout << name_ << " : " << *tls << std::endl;
            }
        }

        {
            Poco::ThreadLocal<int> tls;
            for (*tls = 0; *tls < 5; ++(*tls))
            {
                std::cout << name_ << " : " << *tls << std::endl;
            }
        }
    }

protected:
    std::string name_;
};

class Bug : public Poco::Runnable
{
    void run()
    {
        throw Poco::ApplicationException("got you");
    }
};

class Hunter : public Poco::ErrorHandler
{
public:
    void exception(const Poco::Exception& e)
    {
        std::cerr << e.displayText() << std::endl;
    }

    void exception(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    void exception()
    {
        std::cerr << "unknown exception" << std::endl;
    }
};

int main(int argc, char** argv)
{
    // 方式一: 继承Poco::Runnable并重载run方法
    {
        Poco::Thread thread;
        HelloRunnable runnable;
        thread.start(runnable);
        thread.join();
    }

    // 方式二: 使用Poco::RunnableAdapter封装类的成员方法(方法的参数和返回值都必须为void)
    {
        Poco::Thread thread;
        Greeter greeter;
        Poco::RunnableAdapter<Greeter> runnable(greeter, &Greeter::greet);
        thread.start(runnable);
        thread.join();
    }

    // 线程池使用
    {
        HelloRunnable runnable;

        // 默认线程池
        Poco::ThreadPool::defaultPool().start(runnable);
        Poco::ThreadPool::defaultPool().start(runnable);
        Poco::ThreadPool::defaultPool().joinAll();

        try {
            // 自定义线程池
            Poco::ThreadPool pool(1, 1, 30, 0);
            pool.start(runnable);
            pool.start(runnable);
            pool.joinAll();
        } catch (Poco::NoThreadAvailableException &) {
            std::cout << "exceed thread pool capability" << std::endl;
        }

    }

    // 线程本地变量使用
    {
        Counter c1("A"), c2("B");
        Poco::Thread t[2];
        t[0].start(c1);
        t[1].start(c2);
        t[0].join();
        t[1].join();
    }

    // 线程异常捕获
    {
        Hunter h;
        Poco::ErrorHandler *ph = Poco::ErrorHandler::set(&h);
        Bug runnable;
        Poco::Thread thread;
        thread.start(runnable);
        thread.join();
        Poco::ErrorHandler::set(ph);
    }

    return 0;
}