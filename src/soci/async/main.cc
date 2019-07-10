//
// Created by 范炜东 on 2019/7/10.
//

#include <thread>
#include <iostream>
#include <string>
#include <exception>
#include <ctime>
#include <queue>
#include <vector>
#include <future>
#include <functional>
#include <boost/asio.hpp>
#include <boost/thread/sync_queue.hpp>
#include <soci.h>
#include <postgresql/soci-postgresql.h>

using namespace soci;

class Worker {
public:
  Worker(): querys_(), worker_(&Worker::Run, this) {
  }

  ~Worker() {
    Stop();
    worker_.join();
  }

  void Add(std::function<void(void)> query) {
    querys_.push(query);
  }

  void Stop() {
    if (!querys_.closed())
      querys_.close();
  }

protected:
  void Run() {
    std::cout << "worker start" << std::endl;
    try {
      while (!querys_.closed()) {
        auto query = querys_.pull();
        if (query) {
          query();
        } else {
          std::cout << "no callable" << std::endl;
          break;
        }
      }
    }
    catch(boost::sync_queue_is_closed&) {
    }
    std::cout << "worker exit" << std::endl;
  }

protected:
  boost::sync_queue<std::function<void(void)>> querys_;
  std::thread worker_;
};

class AsyncQuery {
public:
  AsyncQuery(std::future<rowset<row>> res): res_(std::move(res)) {
  }

  AsyncQuery(std::future<rowset<row>> res, std::function<void(rowset<row>&&)> func): res_(std::move(res)), func_(func) {
  }

  bool InvokeIfReady() {
    if (res_.valid() && res_.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
      if (func_) {
        func_(res_.get());
      }
      return true;
    }

    return false;
  }

protected:
  std::future<rowset<row>> res_;
  std::function<void(rowset<row>&&)> func_;
};

class Session {
public:
  Session(): io_context_(), signals_(io_context_, SIGINT, SIGTERM), timer_(io_context_), async_querys_() {
    signals_.async_wait([this](const boost::system::error_code& ec, int signal_number){
      io_context_.stop();
    });
  }

  ~Session() {
    io_context_.stop();
    async_querys_.clear();
  }

  void Add(std::shared_ptr<AsyncQuery> iter) {
    async_querys_.push_back(iter);
  }

  void Run() {
    timer_.expires_after(std::chrono::seconds(2));
    timer_.async_wait([this](const boost::system::error_code &ec) {
      update(ec);
    });
    io_context_.run();
  }

protected:
  void update(const boost::system::error_code &ec) {
    if (!ec) {
      std::cout << "Session Update" << std::endl;

      // 再次循环
      timer_.expires_after(std::chrono::seconds(2));
      timer_.async_wait([this](const boost::system::error_code &ec) {
        update(ec);
      });

      // 检测异步查询任务是否已经完成，如果完成将其移除
      async_querys_.erase(std::remove_if(async_querys_.begin(), async_querys_.end(), [](std::shared_ptr<AsyncQuery> iter){
        return iter->InvokeIfReady();
      }), async_querys_.end());
    } else {
      io_context_.stop();
    }
  }

protected:
  boost::asio::io_context io_context_;
  boost::asio::signal_set signals_;
  boost::asio::steady_timer timer_;
  std::vector<std::shared_ptr<AsyncQuery>> async_querys_;
};

int main() {
  try {
    session sql(postgresql, "dbname=mydb user=postgres password=123456");

    // 创建表
    sql << "CREATE TABLE IF NOT EXISTS PERSON (\n"
           "    NAME        VARCHAR(10) PRIMARY KEY NOT NULL,\n"
           "    TITLE       VARCHAR(20),\n"
           "    AGE         INT NOT NULL,\n"
           "    BIRTHDAY    DATE,\n"
           "    MARRIED     BOOL\n"
           ");";
    // 插入数据
    sql << "INSERT INTO PERSON (NAME, AGE, BIRTHDAY, MARRIED) VALUES ('fwdssg', 28, date '1991-06-22', true)";
    sql << "INSERT INTO PERSON (NAME, AGE, BIRTHDAY, MARRIED) VALUES ('xb', 26, date '1993-09-22', true)";

    // 异步SQL执行线程
    Worker worker;
    // 会话
    Session sess;

    // 在其他线程中执行异步查询
    std::thread async([&]() {
      std::this_thread::sleep_for(std::chrono::seconds(3));
      // 此处可以封装下，但是worker.Add的部分必须足够灵活，因为有些SQL语句可能还需要动态绑定参数，参考statements教程
      {
        auto res = std::make_shared<std::promise<rowset<row>>>();
        auto iter = std::make_shared<AsyncQuery>(std::move(res->get_future()), [](rowset<row>&& rs) {
          std::cout << "First Async Query" << std::endl;
          for (auto iter = rs.begin(); iter != rs.end(); iter++) {
            std::cout << "Name: " << iter->get<std::string>(0) << " Age: " << iter->get<int>(1) << std::endl;
          }
        });
        worker.Add([&, res]() {
          rowset<row> rs = (sql.prepare << "SELECT NAME, AGE FROM PERSON");
          res->set_value(rs);
        });
        sess.Add(iter);
      }

      std::this_thread::sleep_for(std::chrono::seconds(2));
      {
        auto res = std::make_shared<std::promise<rowset<row>>>();
        auto iter = std::make_shared<AsyncQuery>(std::move(res->get_future()), [](rowset<row>&& rs) {
          std::cout << "Second Async Query" << std::endl;
          for (auto iter = rs.begin(); iter != rs.end(); iter++) {
            std::cout << "Name: " << iter->get<std::string>(0) << " Age: " << iter->get<int>(1) << std::endl;
          }
        });
        worker.Add([&, res]() {
          rowset<row> rs = (sql.prepare << "SELECT NAME, AGE FROM PERSON");
          res->set_value(rs);
        });
        sess.Add(iter);
      }
    });

    // 执行异步循环
    sess.Run();

    // 清理
    async.join();
    worker.Stop();

    // 删除表
    sql << "DROP TABLE PERSON";
  } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }

  return 0;
}