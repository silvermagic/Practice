#include <signal.h>
#include <time.h>
extern "C" {
#include "signal_functions.h"
#include "tlpi_hdr.h"
}
#include <fmt/core.h>

static volatile sig_atomic_t gotSigquit = 0;

static void handler(int sig) {
  fmt::print("Caught signal {} ({})\n", sig, strsignal(sig));
  if (sig == SIGQUIT)
    gotSigquit = 1;
}

int main(int argc, char *argv[]) {
  sigset_t origMask, blockedMask;
  struct sigaction sa;

  printSigMask(stdout, "Initial signal mask is:\n");

  sigemptyset(&blockedMask);
  sigaddset(&blockedMask, SIGINT);
  sigaddset(&blockedMask, SIGQUIT);
  if (sigprocmask(SIG_BLOCK, &blockedMask, &origMask) == -1)
    errExit("sigprocmask - SIG_BLOCK");

  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = handler;

  if (sigaction(SIGINT, &sa, NULL) == -1)
    errExit("sigaction");
  if (sigaction(SIGQUIT, &sa, NULL) == -1)
    errExit("sigaction");

  for (int loopNum = 0; !gotSigquit; loopNum++) {
    fmt::print("=== LOOP {}\n", loopNum);

    printSigMask(stdout, "Starting critical section, signal mask is:\n");
    for (time_t startTime = time(NULL); time(NULL) < startTime + 4;)
      continue;

    printPendingSigs(stdout, "Before sigsuspend() - pending signals:\n");
    if (sigsuspend(&origMask) == -1 && errno != EINTR)
      errExit("sigsuspend");
  }

  if (sigprocmask(SIG_SETMASK, &origMask, NULL) == -1)
    errExit("sigprocmask");

  printSigMask(stdout, "=== Exited loop\nRestored signal mask to:\n");

  return 0;
}