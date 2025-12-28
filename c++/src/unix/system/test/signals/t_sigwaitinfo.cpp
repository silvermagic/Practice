#include <signal.h>
extern "C" {
#include "tlpi_hdr.h"
}
#include <fmt/core.h>

int main(int argc, char *argv[]) {
  siginfo_t si;
  sigset_t allSigs;

  if (argc > 1 && strcmp(argv[1], "--help") == 0)
    usageErr(fmt::format("{} [delay-secs]\n", argv[0]).c_str());

  fmt::print("{}: PID is {}\n", argv[0], static_cast<long>(getpid()));

  sigfillset(&allSigs);
  if (sigprocmask(SIG_SETMASK, &allSigs, NULL) == -1)
    errExit("sigprocmask");
  fmt::print("{}: signals blocked\n", argv[0]);

  if (argc > 1) {
    fmt::print("{}: about to delay {} seconds\n", argv[0], argv[1]);
    sleep(getInt(argv[1], GN_GT_0, "delay-secs"));
    fmt::print("{}: finished delay\n", argv[0]);
  }

  for (;;) {
    int sig = sigwaitinfo(&allSigs, &si);
    if (sig == -1)
      errExit("sigwaitinfo");

    if (sig == SIGINT || sig == SIGTERM)
      return 0;

    fmt::print("got signal: {} ({})\n", sig, strsignal(sig));
    fmt::print("  si_signo={}, si_code={} ({}), si_value={}\n", si.si_signo,
               si.si_code,
               (si.si_code == SI_USER
                    ? "SI_USER"
                    : (si.si_code == SI_QUEUE ? "SI_QUEUE" : "other")),
               si.si_value.sival_int);
    fmt::print("  si_pid={}, si_uid={}\n", static_cast<long>(si.si_pid),
               static_cast<long>(si.si_uid));
  }
}