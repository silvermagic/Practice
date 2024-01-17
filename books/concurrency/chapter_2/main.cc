#include <iostream>
#include <thread>

class background_task {
 public:
  void operator()() const {
    std::cout << "Hello Concurrent World!" << std::endl;
  }
};

class widget_data {
 public:
  explicit widget_data() { std::cout << "widget_data" << std::endl; }
};

void update_data_for_widget(int id, widget_data& data) {
  std::cout << "update_data_for_widget " << id << std::endl;
}

class X {
 public:
  explicit X(int data) : data_(data) { std::cout << "X" << std::endl; }
  X(const X& x) : data_(x.data_) { std::cout << "X(X)" << std::endl; }
  void do_lengthy_work(int count) {
    std::cout << "do_lengthy_work " << count << " times with data " << data_
              << std::endl;
  }

 private:
  int data_{0};
};

void some_function() { std::cout << "some_function" << std::endl; }

class scoped_thread {
  std::thread t_;

 public:
  scoped_thread() noexcept = default;
  template <typename Callable, typename... Args>
  scoped_thread(Callable&& func, Args&&... args)
      : t_(std::forward<Callable>(func), std::forward<Args>(args)...) {}
  explicit scoped_thread(std::thread t) : t_(std::move(t)) {}
  scoped_thread(scoped_thread&& other) noexcept : t_(std::move(other.t_)) {}
  ~scoped_thread() noexcept {
    if (joinable()) {
      join();
    }
  }

  scoped_thread& operator=(scoped_thread&& other) noexcept {
    if (joinable()) {
      join();
    }
    t_ = std::move(other.t_);
    return *this;
  }
  scoped_thread& operator=(std::thread other) noexcept {
    if (joinable()) {
      join();
    }
    t_ = std::move(other);
    return *this;
  }

  std::thread::id get_id() const noexcept { return t_.get_id(); }
  void swap(scoped_thread& other) noexcept { t_.swap(other.t_); }

  bool joinable() const noexcept { return t_.joinable(); }
  void join() { t_.join(); }
  void detach() { t_.detach(); }

  std::thread& as_thread() noexcept { return t_; }
  const std::thread& as_thread() const noexcept { return t_; }
};

void do_work(int id) { std::cout << "do_work for " << id << std::endl; }

int main() {
  background_task bgt;
  std::thread t(bgt);
  t.join();
  std::thread t1{background_task()};
  t1.join();
  // std::thread t2(background_task());
  // t2.join();
  widget_data data;
  std::thread t3(update_data_for_widget, 0, std::ref(data));
  t3.join();
  X my_x{1};
  std::thread t4(&X::do_lengthy_work, &my_x, 3);
  t4.join();

  // std::thread t5(some_function);
  // t5 = std::thread(some_function);
  // t5.join();

  scoped_thread t6(do_work, 2);
  return 0;
}