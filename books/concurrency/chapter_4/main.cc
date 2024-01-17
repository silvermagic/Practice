#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <queue>
#include <random>
#include <thread>

void chapter_4_1_1() {
  std::mutex m;
  std::condition_variable cv;
  std::queue<int> data;
  bool stop = false;

  std::thread t([&m, &cv, &data, &stop]() {
    int i = 0;
    while (!stop) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      std::unique_lock<std::mutex> guard(m);
      data.push(i++);
      cv.notify_one();
    }
  });

  std::thread t1([&m, &cv, &data, &stop]() {
    while (!stop) {
      std::unique_lock<std::mutex> guard(m);
      cv.wait(guard, [&data, &stop]() { return stop || !data.empty(); });
      if (stop) {
        break;
      }

      std::cout << "recv data " << data.front() << std::endl;
      data.pop();
      guard.unlock();
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  });

  std::this_thread::sleep_for(std::chrono::seconds(5));
  stop = true;
  t.join();
  t1.join();
}

template <typename T>
class threadsafe_queue {
  std::mutex m_{};
  std::condition_variable cv_{};
  std::queue<T> data_{};
  std::atomic<bool> stop_{false};

 public:
  threadsafe_queue() = default;
  threadsafe_queue(const threadsafe_queue& other) {
    std::lock_guard<std::mutex> guard(other.m_);
    data_ = other.data_;
  }
  ~threadsafe_queue() {
    std::cout << "~threadsafe_queue()" << std::endl;
    close();
  }
  threadsafe_queue& operator=(const threadsafe_queue&) = delete;
  void close() {
    stop_ = true;
    cv_.notify_all();
  }

  void push(T value) {
    std::lock_guard<std::mutex> guard(m_);
    data_.push(value);
    cv_.notify_one();
  }

  std::shared_ptr<T> wait_and_pop() {
    std::unique_lock<std::mutex> guard(m_);
    if (data_.empty() || !stop_.load()) {
      std::cout << "wait_and_pop" << std::endl;
      cv_.wait(guard, [this]() { return !data_.empty() || stop_; });
      std::cout << "wakeup" << std::endl;
    }
    if (stop_.load()) {
      return nullptr;
    }
    std::shared_ptr<T> value = std::make_shared<T>(data_.front());
    data_.pop();
    return value;
  }

  bool empty() const {
    std::lock_guard<std::mutex> guard(m_);
    return data_.empty();
  }
};

void chapter_4_1_2() {
  bool stop = false;
  threadsafe_queue<int> q;

  std::thread t([&stop, &q]() {
    int i = 0;
    while (!stop) {
      q.push(i);
      i += 1;
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  });
  std::thread t1([&stop, &q]() {
    int i = 100;
    while (!stop) {
      q.push(i);
      i += 2;
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  });
  std::thread t2([&stop, &q]() {
    while (!stop) {
      auto vptr = q.wait_and_pop();
      if (nullptr != vptr) std::cout << "recv " << *vptr << std::endl;
    }
  });

  std::this_thread::sleep_for(std::chrono::seconds(6));
  stop = true;
  q.close();
  t.join();
  t1.join();
  t2.join();
}

int find_the_answer(int seed) {
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::cout << "find_the_answer" << std::endl;
  return seed + 10;
}

void chapter_4_2_1() {
  std::future<int> the_answer = std::async(find_the_answer, 1);
  std::this_thread::sleep_for(std::chrono::seconds(3));
  std::cout << "The answer is " << the_answer.get() << std::endl;
  std::future<int> the_answer2 =
      std::async(std::launch::deferred, find_the_answer, 2);
  std::this_thread::sleep_for(std::chrono::seconds(3));
  std::cout << "wakeup" << std::endl;
  the_answer2.wait();
  std::cout << "The answer2 is " << the_answer2.get() << std::endl;
}

void do_work(int id) { std::cout << "do work for " << id << std::endl; }

void chapter_4_2_2() {
  std::mutex m;
  std::condition_variable cv;
  std::deque<std::packaged_task<void()>> tasks;
  bool stop = false;
  std::thread main_loop([&m, &cv, &tasks, &stop]() {
    while (!stop) {
      std::packaged_task<void()> task;
      {
        std::unique_lock<std::mutex> lk(m);
        if (tasks.empty()) {
          cv.wait_for(lk, std::chrono::seconds(1),
                      [&tasks]() { return !tasks.empty(); });
          continue;
        }
        task = std::move(tasks.front());
        tasks.pop_front();
      }
      task();
    }
  });

  std::thread t1([&m, &cv, &tasks]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::lock_guard<std::mutex> lk(m);
    tasks.push_back(std::packaged_task<void()>{std::bind(do_work, 1)});
    cv.notify_one();
  });
  std::thread t2([&m, &cv, &tasks]() {
    std::lock_guard<std::mutex> lk(m);
    tasks.push_back(std::packaged_task<void()>{std::bind(do_work, 2)});
    cv.notify_one();
  });

  std::this_thread::sleep_for(std::chrono::seconds(3));
  stop = true;
  main_loop.join();
  t1.join();
  t2.join();
}

std::random_device rd;
std::mt19937 eng(rd());
std::uniform_int_distribution dis(0, 10);

int do_shared_work() {
  std::cout << "do_shared_work" << std::endl;
  return dis(eng);
}

void chapter_4_2_5() {
  std::shared_future<int> sf{
      std::move(std::async(std::launch::deferred, do_shared_work))};
  std::thread t1([sf]() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    int v = sf.get();
    std::cout << "thread one: " << v << std::endl;
  });
  std::thread t2([sf]() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    int v = sf.get();
    std::cout << "thread two: " << v << std::endl;
  });

  t1.join();
  t2.join();
}

int main() {
  // chapter_4_1_1();
  // chapter_4_1_2();
  // chapter_4_2_1();
  // chapter_4_2_2();
  chapter_4_2_5();
  return 0;
}