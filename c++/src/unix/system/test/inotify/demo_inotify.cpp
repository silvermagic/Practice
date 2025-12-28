#include <fcntl.h>
#include <sys/inotify.h>
#include <unistd.h>
extern "C" {
#include "tlpi_hdr.h"
}
#include <fmt/core.h>

static void displayInotifyEvent(struct inotify_event *i) {
  fmt::print("    wd = {:2}; ", i->wd);
  if (i->cookie > 0) {
    fmt::print("cookie = {:3}; ", i->cookie);
  }

  fmt::print("mask = ");
  if (i->mask & IN_ACCESS)
    fmt::print("IN_ACCESS ");
  if (i->mask & IN_ATTRIB)
    fmt::print("IN_ATTRIB ");
  if (i->mask & IN_CLOSE_WRITE)
    fmt::print("IN_CLOSE_WRITE ");
  if (i->mask & IN_CLOSE_NOWRITE)
    fmt::print("IN_CLOSE_NOWRITE ");
  if (i->mask & IN_CREATE)
    fmt::print("IN_CREATE ");
  if (i->mask & IN_DELETE)
    fmt::print("IN_DELETE ");
  if (i->mask & IN_DELETE_SELF)
    fmt::print("IN_DELETE_SELF ");
  if (i->mask & IN_IGNORED)
    fmt::print("IN_IGNORED ");
  if (i->mask & IN_ISDIR)
    fmt::print("IN_ISDIR ");
  if (i->mask & IN_MODIFY)
    fmt::print("IN_MODIFY ");
  if (i->mask & IN_MOVE_SELF)
    fmt::print("IN_MOVE_SELF ");
  if (i->mask & IN_MOVED_FROM)
    fmt::print("IN_MOVED_FROM ");
  if (i->mask & IN_MOVED_TO)
    fmt::print("IN_MOVED_TO ");
  if (i->mask & IN_OPEN)
    fmt::print("IN_OPEN ");
  if (i->mask & IN_Q_OVERFLOW)
    fmt::print("IN_Q_OVERFLOW ");
  if (i->mask & IN_UNMOUNT)
    fmt::print("IN_UNMOUNT ");
  fmt::print("\n");

  if (i->len > 0) {
    fmt::print("        name = {}\n", i->name);
  }
}

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

int main(int argc, char *argv[]) {
  if (argc < 2 || strcmp(argv[1], "--help") == 0)
    usageErr(fmt::format("{} pathname...\n", argv[0]).c_str());

  int inotifyFd = inotify_init();
  if (inotifyFd == -1)
    errExit("inotify_init");

  for (int i = 1; i < argc; i++) {
    int wd = inotify_add_watch(inotifyFd, argv[i], IN_ALL_EVENTS);
    if (wd == -1)
      errExit("inotify_add_watch");
    fmt::print("Watching {} using wd {}\n", argv[i], wd);
  }

  char buf[BUF_LEN];
  for (;;) {
    ssize_t nread = read(inotifyFd, buf, BUF_LEN);
    if (nread == 0)
      fatal("read from inotify fd returned 0!");

    if (nread == -1)
      errExit("read");

    for (char *p = buf; p < buf + nread;) {
      struct inotify_event *event = (struct inotify_event *)p;
      displayInotifyEvent(event);
      p += sizeof(struct inotify_event) + event->len;
    }
  }

  return 0;
}