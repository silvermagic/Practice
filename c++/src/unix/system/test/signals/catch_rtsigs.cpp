#include <csignal>
#include <signal.h>
extern "C" {
#include "tlpi_hdr.h"
}
#include <fmt/core.h>

static volatile int handlerSleepTime;
static volatile int sigCnt = 0;
static volatile int allDone = 0;

static void siginfoHandler(int sig, siginfo_t *si, void *ucontext) {
  if (sig == SIGINT || sig == SIGTERM) {
    allDone = 1;
    return;
  }

  sigCnt++;
  fmt::print("caught signal {}\n", sig);
  fmt::print("  si_signo={}, si_code={} ({}), ", si->si_signo, si->si_code,
             (si->si_code == SI_USER
                  ? "SI_USER"
                  : (si->si_code == SI_QUEUE ? "SI_QUEUE" : "other")));
  fmt::print("si_value={}\n", si->si_value.sival_int);
  fmt::print("  si_pid={}, si_uid={}\n", static_cast<long>(si->si_pid),
             static_cast<long>(si->si_uid));

  sleep(handlerSleepTime);
}

int main(int argc, char *argv[]) {
  struct sigaction sa;
  sigset_t prevMask, blockMask;

  if (argc > 1 && strcmp(argv[1], "--help") == 0)
    usageErr(
        fmt::format("{} [block-time [handler-sleep-time]]\n", argv[0]).c_str());

  fmt::print("{}: PID is {}\n", argv[0], static_cast<long>(getpid()));
  handlerSleepTime =
      (argc > 2) ? getInt(argv[2], GN_NONNEG, "handler-sleep-time") : 1;

  sa.sa_sigaction = siginfoHandler;
  sa.sa_flags = SA_SIGINFO;
  sigfillset(&sa.sa_mask);

  for (int sig = 1; sig < NSIG; sig++) {
    if (sig != SIGTSTP && sig != SIGQUIT) {
      sigaction(sig, &sa, NULL);
    }
  }

  if (argc > 1) {
    sigfillset(&blockMask);
    sigdelset(&blockMask, SIGINT);
    sigdelset(&blockMask, SIGTERM);

    if (sigprocmask(SIG_SETMASK, &blockMask, &prevMask) == -1)
      errExit("sigprocmask");

    fmt::print("{}: signals blocked - sleeping {} seconds\n", argv[0], argv[1]);
    sleep(getInt(argv[1], GN_NONNEG, "block-time"));
    fmt::print("{}: sleep complete\n", argv[0]);

    if (sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1)
      errExit("sigprocmask");
  }

  while (!allDone)
    pause();

  return 0;
}