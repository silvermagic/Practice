#include <atomic>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <fmt/core.h>
#include <iostream>
#include <signal.h>
#include <sys/eventfd.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>
#include <vector>

// ================= 配置与结构 =================

constexpr size_t kNumSlots = 20;
constexpr uint32_t kMagic = 0xDEADC0DE;
const char *kShmName = "/shm_img";
const char *kSocketPath = "/tmp/shm_img.sock";

struct ShmSharedState {
  uint32_t magic;
  std::atomic<int> ready_slot_idx; // 已处理的最大索引
  std::atomic<bool> stop_flag;     // 全局停止标志
  int slots[kNumSlots];            // 图像数据
};

std::atomic<bool> keep_running{true};
void sig_handler(int) { keep_running = false; }

// ================= SCM_RIGHTS 辅助函数 =================

void send_fd(int socket, int fd_to_send) {
  struct msghdr msg = {0};
  char buf[CMSG_SPACE(sizeof(int))];
  struct iovec io = {.iov_base = (void *)"!", .iov_len = 1};
  msg.msg_iov = &io;
  msg.msg_iovlen = 1;
  msg.msg_control = buf;
  msg.msg_controllen = sizeof(buf);

  struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
  cmsg->cmsg_level = SOL_SOCKET;
  cmsg->cmsg_type = SCM_RIGHTS;
  cmsg->cmsg_len = CMSG_LEN(sizeof(int));
  *((int *)CMSG_DATA(cmsg)) = fd_to_send;
  sendmsg(socket, &msg, 0);
}

int recv_fd(int socket) {
  struct msghdr msg = {0};
  char control_buf[CMSG_SPACE(sizeof(int))];
  char data_buf[1];
  struct iovec io = {.iov_base = data_buf, .iov_len = sizeof(data_buf)};
  msg.msg_iov = &io;
  msg.msg_iovlen = 1;
  msg.msg_control = control_buf;
  msg.msg_controllen = sizeof(control_buf);

  if (recvmsg(socket, &msg, 0) <= 0)
    return -1;
  struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
  if (!cmsg || cmsg->cmsg_type != SCM_RIGHTS)
    return -1;
  return *((int *)CMSG_DATA(cmsg));
}

// ================= Producer 逻辑 =================

struct ConsumerEntry {
  int efd;  // 门铃 FD
  int sock; // 控制连接（用于探测存活）
};

void run_producer(ShmSharedState *s) {
  unlink(kSocketPath);
  int server_sock = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
  struct sockaddr_un addr = {.sun_family = AF_UNIX};
  strncpy(addr.sun_path, kSocketPath, sizeof(addr.sun_path) - 1);
  bind(server_sock, (struct sockaddr *)&addr, sizeof(addr));
  listen(server_sock, 10);

  std::vector<ConsumerEntry> consumers;
  s->stop_flag.store(false);
  int next_idx = s->ready_slot_idx.load() + 1;

  fmt::print("Producer: Started. Resuming from idx {}.\n", next_idx);

  while (keep_running && next_idx < (int)kNumSlots) {
    // 1. 接收新消费者
    int client_sock = accept(server_sock, NULL, NULL);
    if (client_sock != -1) {
      int efd = recv_fd(client_sock);
      if (efd != -1) {
        consumers.push_back({efd, client_sock});
        fmt::print("Producer: Consumer joined. Active count: {}\n",
                   consumers.size());
      } else {
        close(client_sock);
      }
    }

    // 2. 模拟生产数据
    usleep(800000);
    s->slots[next_idx] = std::rand() % 100 + 1;
    s->ready_slot_idx.store(next_idx, std::memory_order_release);

    // 3. 广播通知
    uint64_t u = 1;
    for (auto it = consumers.begin(); it != consumers.end();) {
      if (write(it->efd, &u, sizeof(u)) == -1) {
        fmt::print("Producer: Removing disconnected consumer.\n");
        close(it->efd);
        close(it->sock);
        it = consumers.erase(it);
      } else {
        ++it;
      }
    }
    fmt::print("Producer: Processed slot {}\n", next_idx);
    next_idx++;
  }

  // 4. 结束逻辑：发送停止信号
  fmt::print("Producer: Tasks finished or interrupted. Notifying consumers to "
             "exit...\n");
  if (keep_running) {
    s->stop_flag.store(true);
    shm_unlink(kShmName);
  }
  uint64_t u = 1;
  for (auto &c : consumers) {
    write(c.efd, &u, sizeof(u));
    close(c.efd);
    close(c.sock);
  }
  close(server_sock);
  unlink(kSocketPath);
}

// ================= Consumer 逻辑 =================

void run_consumer(ShmSharedState *s) {
  int local_idx = 0;
  int my_efd = eventfd(0, EFD_NONBLOCK);
  int ctrl_sock = -1;

  while (keep_running) {
    // 1. 自动重连逻辑
    if (ctrl_sock == -1) {
      ctrl_sock = socket(AF_UNIX, SOCK_STREAM, 0);
      struct sockaddr_un addr = {.sun_family = AF_UNIX};
      strncpy(addr.sun_path, kSocketPath, sizeof(addr.sun_path) - 1);

      if (connect(ctrl_sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        close(ctrl_sock);
        ctrl_sock = -1;
        if (s->stop_flag.load())
          break; // 生产者已结束且不再回来
        usleep(500000);
        continue;
      }
      send_fd(ctrl_sock, my_efd);
      fmt::print("Consumer: Connected to producer.\n");
    }

    // 2. 等待通知
    struct pollfd pfds[2];
    pfds[0].fd = my_efd;
    pfds[0].events = POLLIN;
    pfds[1].fd = ctrl_sock;
    pfds[1].events = POLLIN | POLLRDHUP;

    int ret = poll(pfds, 2, 500);

    // 3. 检查生产者是否退出
    if (ret > 0 && (pfds[1].revents & (POLLHUP | POLLRDHUP | POLLERR))) {
      fmt::print("Consumer: Producer connection lost.\n");
      close(ctrl_sock);
      ctrl_sock = -1;
      continue;
    }

    if (ret > 0 && (pfds[0].revents & POLLIN)) {
      uint64_t u;
      read(my_efd, &u, sizeof(u));
    }

    // 4. 消费数据
    int current_ready = s->ready_slot_idx.load(std::memory_order_acquire);
    while (local_idx <= current_ready) {
      fmt::print("Consumer: Read slot {} = {}\n", local_idx,
                 s->slots[local_idx]);
      local_idx++;
    }

    // 5. 检查全局停止标志
    if (s->stop_flag.load()) {
      fmt::print("Consumer: Received stop signal from producer. Exiting.\n");
      break;
    }
  }
  if (ctrl_sock != -1)
    close(ctrl_sock);
  close(my_efd);
}

// ================= Main =================

int main(int argc, char *argv[]) {
  if (argc < 2)
    return 1;
  bool is_prod = (strcmp(argv[1], "producer") == 0);

  struct sigaction sa {};
  sa.sa_handler = sig_handler;
  sigaction(SIGINT, &sa, nullptr);

  int shm_fd = shm_open(kShmName, O_RDWR | O_CREAT, 0666);
  ftruncate(shm_fd, sizeof(ShmSharedState));
  auto *s =
      (ShmSharedState *)mmap(NULL, sizeof(ShmSharedState),
                             PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

  if (is_prod) {
    if (flock(shm_fd, LOCK_EX | LOCK_NB) == -1) {
      fmt::print("Error: Producer already running.\n");
      return 1;
    }
    close(shm_fd);
    if (s->magic != kMagic) {
      s->ready_slot_idx.store(-1);
      s->stop_flag.store(false);
      s->magic = kMagic;
    }
    run_producer(s);
  } else {
    close(shm_fd);
    while (s->magic != kMagic)
      usleep(100000);
    run_consumer(s);
  }

  munmap(s, sizeof(ShmSharedState));
  fmt::print("{} exiting cleanly.\n", argv[1]);
  return 0;
}