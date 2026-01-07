#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <unistd.h>

#include <condition_variable>
#include <cstring>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>

constexpr int MAX_EVENTS = 64;
constexpr int BUF_SIZE = 4096;
static const char *FILE_NAME = "test.txt";

// ================= 工具 =================
int set_nonblock(int fd) {
  int f = fcntl(fd, F_GETFL, 0);
  return fcntl(fd, F_SETFL, f | O_NONBLOCK);
}

// ================= 连接对象（只属于 epoll 线程） =================
struct Connection {
  int fd;
  std::string in_buf;
  std::string out_buf;
};

// ================= 文件线程 → epoll 线程的结果 =================
struct FileResult {
  int fd;
  std::string data;
};

// ================= 全局状态 =================
int epfd;
int notify_fd;

std::unordered_map<int, Connection> conns;

// 文件线程队列
std::queue<int> file_tasks;
std::mutex file_mtx;
std::condition_variable file_cv;

// 完成队列（file → epoll）
std::queue<FileResult> completed;
std::mutex complete_mtx;

// ================= 文件 I/O 线程 =================
void file_worker() {
  while (true) {
    int fd;
    {
      std::unique_lock lk(file_mtx);
      file_cv.wait(lk, [] { return !file_tasks.empty(); });
      fd = file_tasks.front();
      file_tasks.pop();
    }

    int f = open(FILE_NAME, O_RDONLY);
    if (f < 0)
      continue;

    char buf[BUF_SIZE];
    ssize_t n = read(f, buf, sizeof(buf));
    close(f);

    if (n > 0) {
      {
        std::lock_guard lk(complete_mtx);
        completed.push({fd, std::string(buf, n)});
      }
      uint64_t one = 1;
      write(notify_fd, &one, sizeof(one));
    }
  }
}

// ================= epoll: 处理文件完成 =================
void handle_file_complete() {
  uint64_t v;
  read(notify_fd, &v, sizeof(v)); // 清空 eventfd

  std::queue<FileResult> local;
  {
    std::lock_guard lk(complete_mtx);
    std::swap(local, completed);
  }

  while (!local.empty()) {
    auto &r = local.front();
    auto it = conns.find(r.fd);
    if (it != conns.end()) {
      it->second.out_buf.append(r.data);

      epoll_event ev{};
      ev.events = EPOLLIN | EPOLLOUT;
      ev.data.fd = r.fd;
      epoll_ctl(epfd, EPOLL_CTL_MOD, r.fd, &ev);
    }
    local.pop();
  }
}

// ================= 读处理 =================
void handle_read(int fd) {
  char buf[BUF_SIZE];

  while (true) {
    ssize_t n = recv(fd, buf, sizeof(buf), 0);
    if (n > 0) {
      conns[fd].in_buf.append(buf, n);
    } else if (n == 0) {
      close(fd);
      conns.erase(fd);
      return;
    } else {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        break;
      close(fd);
      conns.erase(fd);
      return;
    }
  }

  auto &c = conns[fd];

  while (true) {
    auto pos = c.in_buf.find('\n');
    if (pos == std::string::npos)
      break;

    std::string line = c.in_buf.substr(0, pos);
    c.in_buf.erase(0, pos + 1);

    if (line == "GETFILE") {
      std::lock_guard lk(file_mtx);
      file_tasks.push(fd);
      file_cv.notify_one();
    } else {
      c.out_buf.append(line);
      c.out_buf.push_back('\n');

      epoll_event ev{};
      ev.events = EPOLLIN | EPOLLOUT;
      ev.data.fd = fd;
      epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
    }
  }
}

// ================= 写处理 =================
void handle_write(int fd) {
  auto &c = conns[fd];

  while (!c.out_buf.empty()) {
    ssize_t n = send(fd, c.out_buf.data(), c.out_buf.size(), 0);
    if (n > 0) {
      c.out_buf.erase(0, n);
    } else {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        break;
      close(fd);
      conns.erase(fd);
      return;
    }
  }

  if (c.out_buf.empty()) {
    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
  }
}

// ================= main =================
int main() {
  int listenfd = socket(AF_INET, SOCK_STREAM, 0);
  set_nonblock(listenfd);

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8888);
  addr.sin_addr.s_addr = INADDR_ANY;

  bind(listenfd, (sockaddr *)&addr, sizeof(addr));
  listen(listenfd, 128);

  epfd = epoll_create1(0);

  epoll_event ev{};
  ev.events = EPOLLIN;
  ev.data.fd = listenfd;
  epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

  notify_fd = eventfd(0, EFD_NONBLOCK);
  ev.events = EPOLLIN;
  ev.data.fd = notify_fd;
  epoll_ctl(epfd, EPOLL_CTL_ADD, notify_fd, &ev);

  std::thread(file_worker).detach();

  std::cout << "nginx-style epoll + posted event server on 8888\n";

  epoll_event events[MAX_EVENTS];

  while (true) {
    int n = epoll_wait(epfd, events, MAX_EVENTS, -1);
    for (int i = 0; i < n; ++i) {
      int fd = events[i].data.fd;

      if (fd == listenfd) {
        while (true) {
          int cfd = accept(listenfd, nullptr, nullptr);
          if (cfd < 0)
            break;
          set_nonblock(cfd);
          conns[cfd] = {cfd};

          ev.events = EPOLLIN;
          ev.data.fd = cfd;
          epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
        }
      } else if (fd == notify_fd) {
        handle_file_complete();
      } else {
        if (events[i].events & EPOLLIN)
          handle_read(fd);
        if (events[i].events & EPOLLOUT)
          handle_write(fd);
      }
    }
  }
}