#include <algorithm>
#include <exception>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <stack>
#include <thread>
#include <vector>

std::list<int> some_list;
std::mutex some_mutex;

void add_to_list(int value) {
  std::scoped_lock<std::mutex> guard(some_mutex);
  some_list.push_back(value);
}

bool list_contains(int value_to_find) {
  std::scoped_lock<std::mutex> guard(some_mutex);
  return std::find(some_list.begin(), some_list.end(), value_to_find) !=
         some_list.end();
}

class empty_stack : std::exception {
 public:
  const char* what() const throw() { return "empty stack"; }
};

template <typename T>
class threadsafe_stack {
  std::stack<T> data_{};
  std::mutex m_{};

 public:
  threadsafe_stack() {}
  threadsafe_stack(const threadsafe_stack& other) {
    std::lock_guard<std::mutex> guard(other.m_);
    data_.swap(other.data_);
  }
  threadsafe_stack& operator=(const threadsafe_stack&) = delete;

  void push(T value) {
    std::lock_guard<std::mutex> guard(m_);
    data_.push(value);
  }
  std::shared_ptr<T> pop() {
    std::lock_guard<std::mutex> guard(m_);
    if (data_.empty()) throw empty_stack();

    std::shared_ptr<T> const res(std::make_shared<T>(data_.top()));
    data_.pop();
    return res;
  }
  void pop(T& value) {
    std::lock_guard<std::mutex> guard(m_);
    if (data_.empty()) throw empty_stack();

    value = data_.top();
    data_.top();
  }
  bool empty() const {
    std::lock_guard<std::mutex> guard(m_);
    return data_.empty();
  }
};

struct some_big_object {
  std::mutex m;
  std::vector<int> data;
};

void swap(some_big_object& lhs, some_big_object& rhs) {
  if (&lhs == &rhs) {
    return;
  }
  std::lock(lhs.m, rhs.m);
  std::lock_guard<std::mutex> lhs_guard(lhs.m, std::adopt_lock);
  std::lock_guard<std::mutex> rhs_guard(rhs.m, std::adopt_lock);
  lhs.data.swap(rhs.data);
}

std::once_flag resource_flag;

void init_resource() { std::cout << "init_resource" << std::endl; }

int main() {
  std::thread t([]() {
    add_to_list(3);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    add_to_list(5);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    add_to_list(4);
  });
  std::thread t1([]() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << list_contains(3) << std::endl;
    std::cout << list_contains(4) << std::endl;
  });
  t.join();
  t1.join();

  threadsafe_stack<int> ss;
  std::thread t2([&ss]() {
    ss.push(3);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ss.push(4);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ss.push(5);
  });
  std::thread t3([&ss]() {
    try {
      auto v = ss.pop();
      std::cout << *v << std::endl;
      // int v2;
      // ss.pop(v2);
      // std::cout << v2 << std::endl;
    } catch (empty_stack& e) {
      std::cout << "error " << e.what() << std::endl;
    }
  });
  t2.join();
  t3.join();

  std::thread t4([]() { std::call_once(resource_flag, init_resource); });
  std::thread t5([]() { std::call_once(resource_flag, init_resource); });
  t4.join();
  t5.join();
  return 0;
}