#include <signal.h>
#include <sys/signalfd.h>
extern "C" {
#include "tlpi_hdr.h"
}
#include <fmt/core.h>

int main(int argc, char *argv[]) {
  sigset_t mask;
  struct signalfd_siginfo fdsi;
  ssize_t s;

  if (argc < 2 || strcmp(argv[1], "--help") == 0)
    usageErr(fmt::format("{} sig-num...\n", argv[0]).c_str());

  fmt::print("{}: PID is {}\n", argv[0], static_cast<long>(getpid()));

  sigemptyset(&mask);
  for (int sig = 1; sig < argc; sig++) {
    sigaddset(&mask, atoi(argv[sig]));
  }

  if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1)
    errExit("sigprocmask");

  int sfd = signalfd(-1, &mask, 0);
  if (sfd == -1)
    errExit("signalfd");

  for (;;) {
    s = read(sfd, &fdsi, sizeof(struct signalfd_siginfo));
    if (s != sizeof(struct signalfd_siginfo))
      errExit("read");

    fmt::print("%s: got signal: {}", argv[0], fdsi.ssi_signo);
    if (fdsi.ssi_code == SI_QUEUE) {
      fmt::print("  ; ssi_pid = {}; ssi_int = {}\n", fdsi.ssi_pid,
                 fdsi.ssi_int);
    }
    fmt::print("\n");
  }

  return 0;
}