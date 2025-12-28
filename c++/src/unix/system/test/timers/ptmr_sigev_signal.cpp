#include <signal.h>
#include <time.h>
extern "C" {
#include "curr_time.h"
#include "itimerspec_from_str.h"
#include "tlpi_hdr.h"
}
#include <fmt/core.h>

#define TIMER_SIG SIGRTMAX

static void handler(int sig, siginfo_t *si, void *uc) {
  timer_t *tidptr;
  tidptr = reinterpret_cast<timer_t *>(si->si_value.sival_ptr);

  fmt::print("[{}] Got signal {}\n", currTime("%T"), sig);
  fmt::print("    *sival_ptr         = {}\n", static_cast<void *>(tidptr));
  fmt::print("    timer_getoverrun() = {}\n", timer_getoverrun(*tidptr));
}

int main(int argc, char *argv[]) {
  if (argc < 2)
    usageErr(fmt::format("{} secs[/nsecs][:int-secs[/nint-secs]]...\n", argv[0])
                 .c_str());

  timer_t *tidlist =
      reinterpret_cast<timer_t *>(calloc(argc - 1, sizeof(timer_t)));
  if (tidlist == NULL)
    errExit("calloc");

  struct sigaction sa;
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = handler;
  sigemptyset(&sa.sa_mask);
  if (sigaction(TIMER_SIG, &sa, NULL) == -1)
    errExit("sigaction");

  struct sigevent sev;
  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = TIMER_SIG;

  for (int i = 0; i < argc - 1; i++) {
    struct itimerspec its;
    itimerspecFromStr(argv[i + 1], &its);
    sev.sigev_value.sival_ptr = &tidlist[i];
    if (timer_create(CLOCK_REALTIME, &sev, &tidlist[i]) == -1)
      errExit("timer_create");

    fmt::print("Timer ID: {} ({})\n", reinterpret_cast<void *>(&tidlist[i]),
               argv[i + 1]);
    if (timer_settime(tidlist[i], 0, &its, NULL) == -1)
      errExit("timer_settime");
  }

  for (;;) {
    pause();
  }

  return 0;
}