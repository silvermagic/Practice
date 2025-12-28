#include <signal.h>
extern "C" {
#include "signal_functions.h"
#include "tlpi_hdr.h"
}
#include <fmt/core.h>

static int sigCnt[NSIG];
static volatile sig_atomic_t gotSigint = 0;

static void handler(int sig) {
  if (sig == SIGINT)
    gotSigint = 1;
  else
    sigCnt[sig]++;
}

int main(int argc, char *argv[]) {
  int n, numSecs;
  sigset_t pendingMask, blockingMask, emptyMask;

  fmt::print("{}: PID is {}\n", argv[0], static_cast<long>(getpid()));

  for (int sig = 0; sig < NSIG; sig++) {
    (void)signal(sig, handler);
  }

  if (argc > 1) {
    numSecs = getInt(argv[1], GN_GT_0, NULL);

    sigfillset(&blockingMask);
    if (sigprocmask(SIG_SETMASK, &blockingMask, NULL) == -1)
      errExit("sigprocmask");

    fmt::print("{}: sleeping for {} seconds\n", argv[0], numSecs);
    sleep(numSecs);

    if (sigpending(&pendingMask) == -1)
      errExit("sigpending");

    fmt::print("{}: pending signals are:\n", argv[0]);
    printSigset(stdout, "\t\t", &pendingMask);

    sigemptyset(&emptyMask);
    if (sigprocmask(SIG_SETMASK, &emptyMask, NULL) == -1)
      errExit("sigprocmask");
  }

  while (!gotSigint)
    continue;

  for (int sig = 1; sig < NSIG; sig++) {
    if (sigCnt[sig] != 0)
      fmt::print("{}: signal {} caught {} time{}\n", argv[0], sig, sigCnt[sig],
                 (sigCnt[sig] == 1 ? "" : "s"));
  }

  return 0;
}