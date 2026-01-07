#include <cstring>
#include <deque>
#include <fcntl.h>
#include <iostream>
#include <liburing.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_set>

static constexpr int PORT = 8888;
static constexpr int QUEUE_DEPTH = 512;
static constexpr size_t BUF_SIZE = 4096;
static const char *FILE_NAME = "test.txt";

/* ---------------- operation types ---------------- */

enum class OpType { ACCEPT, SOCKET_READ, SOCKET_WRITE, FILE_OPEN, FILE_READ };

/* ---------------- connection state ---------------- */

struct WriteItem {
  std::string data;
};

struct Conn {
  int sock_fd;
  int file_fd{-1};

  /* input side */
  std::string in_buf;      // accumulated input
  char read_buf[BUF_SIZE]; // single recv buffer

  /* output side */
  std::deque<WriteItem> write_queue;
  bool write_inflight{false};

  /* file read */
  char file_buf[BUF_SIZE];
};

/* ---------------- request wrapper ---------------- */

struct Request {
  OpType type;
  Conn *conn;
};

/* ---------------- globals ---------------- */

static io_uring ring;
static int listen_fd;
static std::unordered_set<Conn *> conns;

/* ---------------- utils ---------------- */

static void set_nonblock(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

/* ---------------- submit helpers ---------------- */

static void submit_accept() {
  auto *req = new Request{OpType::ACCEPT, nullptr};
  auto *addr = new sockaddr_in;
  auto *len = new socklen_t(sizeof(sockaddr_in));

  io_uring_sqe *sqe = io_uring_get_sqe(&ring);
  io_uring_prep_accept(sqe, listen_fd, reinterpret_cast<sockaddr *>(addr), len,
                       0);

  io_uring_sqe_set_data(sqe, req);
}

static void submit_socket_read(Conn *c) {
  auto *req = new Request{OpType::SOCKET_READ, c};

  io_uring_sqe *sqe = io_uring_get_sqe(&ring);
  io_uring_prep_recv(sqe, c->sock_fd, c->read_buf, BUF_SIZE, 0);

  io_uring_sqe_set_data(sqe, req);
}

static void submit_socket_write(Conn *c) {
  if (c->write_queue.empty() || c->write_inflight)
    return;

  c->write_inflight = true;
  auto &item = c->write_queue.front();

  auto *req = new Request{OpType::SOCKET_WRITE, c};
  io_uring_sqe *sqe = io_uring_get_sqe(&ring);
  io_uring_prep_send(sqe, c->sock_fd, item.data.data(), item.data.size(), 0);

  io_uring_sqe_set_data(sqe, req);
}

static void submit_file_open(Conn *c) {
  auto *req = new Request{OpType::FILE_OPEN, c};

  io_uring_sqe *sqe = io_uring_get_sqe(&ring);
  io_uring_prep_openat(sqe, AT_FDCWD, FILE_NAME, O_RDONLY, 0);

  io_uring_sqe_set_data(sqe, req);
}

static void submit_file_read(Conn *c) {
  auto *req = new Request{OpType::FILE_READ, c};

  io_uring_sqe *sqe = io_uring_get_sqe(&ring);
  io_uring_prep_read(sqe, c->file_fd, c->file_buf, BUF_SIZE,
                     -1 /* use file offset */);

  io_uring_sqe_set_data(sqe, req);
}

/* ---------------- write queue API ---------------- */

static void enqueue_write(Conn *c, const char *data, size_t len) {
  c->write_queue.push_back({std::string(data, len)});
  submit_socket_write(c);
}

/* ---------------- main ---------------- */

int main() {
  listen_fd = socket(AF_INET, SOCK_STREAM, 0);

  int opt = 1;
  setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  set_nonblock(listen_fd);

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(PORT);

  bind(listen_fd, (sockaddr *)&addr, sizeof(addr));
  listen(listen_fd, SOMAXCONN);

  io_uring_queue_init(QUEUE_DEPTH, &ring, 0);

  std::cout << "io_uring proactor echo/file server on port " << PORT
            << std::endl;

  submit_accept();
  io_uring_submit(&ring);

  while (true) {
    io_uring_cqe *cqe;
    io_uring_wait_cqe(&ring, &cqe);

    auto *req = static_cast<Request *>(io_uring_cqe_get_data(cqe));
    int res = cqe->res;

    switch (req->type) {

    /* ---------- accept ---------- */
    case OpType::ACCEPT: {
      if (res >= 0) {
        auto *c = new Conn;
        c->sock_fd = res;
        set_nonblock(c->sock_fd);
        conns.insert(c);
        submit_socket_read(c);
      }
      submit_accept();
      break;
    }

    /* ---------- socket read ---------- */
    case OpType::SOCKET_READ: {
      Conn *c = req->conn;

      if (res <= 0) {
        close(c->sock_fd);
        conns.erase(c);
        delete c;
        break;
      }

      /* accumulate input */
      c->in_buf.append(c->read_buf, res);

      /* parse complete lines */
      while (true) {
        auto pos = c->in_buf.find('\n');
        if (pos == std::string::npos)
          break;

        std::string line = c->in_buf.substr(0, pos);
        c->in_buf.erase(0, pos + 1);

        if (line == "GETFILE") {
          submit_file_open(c);
        } else {
          enqueue_write(c, line.data(), line.size());
          enqueue_write(c, "\n", 1);
        }
      }

      submit_socket_read(c);
      break;
    }

    /* ---------- file open ---------- */
    case OpType::FILE_OPEN: {
      Conn *c = req->conn;
      if (res < 0) {
        const char *msg = "open file failed\n";
        enqueue_write(c, msg, strlen(msg));
      } else {
        c->file_fd = res;
        submit_file_read(c);
      }
      break;
    }

    /* ---------- file read ---------- */
    case OpType::FILE_READ: {
      Conn *c = req->conn;

      if (res <= 0) {
        close(c->file_fd);
        c->file_fd = -1;
      } else {
        enqueue_write(c, c->file_buf, res);
        submit_file_read(c);
      }
      break;
    }

    /* ---------- socket write ---------- */
    case OpType::SOCKET_WRITE: {
      Conn *c = req->conn;
      c->write_queue.pop_front();
      c->write_inflight = false;
      submit_socket_write(c);
      break;
    }
    }

    delete req;
    io_uring_cqe_seen(&ring, cqe);
    io_uring_submit(&ring);
  }
}
