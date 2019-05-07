//
// Created by 范炜东 on 2019/5/7.
//

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>

const int BUFFER_SIZE = 2;
const int TAIL_SIZE = 2;
const int EVENT_SZIE = 5;

void handler(int signum) {
  printf("Caught signal %d, coming out...\n", signum);
  exit(1);
}

int main() {
  signal(SIGINT, handler);

  char buf[BUFFER_SIZE + TAIL_SIZE];
  struct epoll_event event, events[EVENT_SZIE];
  int epfd = epoll_create(1);
  if (epfd == -1) {
    printf("epoll_create failed! %s\n", strerror(errno));
    return -1;
  }
  event.data.fd = STDIN_FILENO;
  event.events = EPOLLIN | EPOLLET;
  if (epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &event) == -1) {
    printf("epoll_ctl failed! %s\n", strerror(errno));
    close(epfd);
    return -1;
  }

  while (1) {
    int nfds = epoll_wait(epfd, events, EVENT_SZIE, -1);
    if (nfds == -1) {
      printf("epoll_wait failed! %s\n", strerror(errno));
      break;
    }

    for (int i = 0; i < nfds; ++i) {
      if (events[i].data.fd == STDIN_FILENO) {
        // 一次最多读取2个字节
        ssize_t len = read(STDIN_FILENO, buf, BUFFER_SIZE);
        memcpy(buf + len, "\n", TAIL_SIZE);
        write(STDOUT_FILENO, "reading... ", strlen("reading... "));
        write(STDOUT_FILENO, buf, len + TAIL_SIZE);

        // 设置触发(如果缓冲区还有剩余未读数据，epoll_wait会立马返回，就像LT模式一样)
        event.data.fd = STDIN_FILENO;
        event.events = EPOLLIN | EPOLLET;
        epoll_ctl(epfd, EPOLL_CTL_MOD, STDIN_FILENO, &event);
      }
    }
  }
}