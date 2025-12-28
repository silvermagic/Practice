#include <ctime>
#include <sys/timerfd.h>
extern "C" {
#include "itimerspec_from_str.h"
#include "tlpi_hdr.h"
}
#include <fmt/core.h>

int main(int argc, char *argv[]) {
  if (argc < 2 || strcmp(argv[1], "--help") == 0)
    usageErr("%s secs[/nsecs][:int-secs[/nint-secs]] [max-exp]\n", argv[0]);

  struct itimerspec its;
  itimerspecFromStr(argv[1], &its);
  uint64_t maxExp = (argc > 2) ? getInt(argv[2], GN_GT_0, "max-exp") : 1;

  int fd = timerfd_create(CLOCK_REALTIME, 0);
  if (fd == -1)
    errExit("timerfd_create");

  if (timerfd_settime(fd, 0, &its, NULL) == -1)
    errExit("timerfd_settime");

  struct timespec start;
  if (clock_gettime(CLOCK_REALTIME, &start) == -1)
    errExit("clock_gettime");

  for (uint64_t totalExp = 0; totalExp < maxExp;) {
    uint64_t numExp;
    ssize_t s = read(fd, &numExp, sizeof(uint64_t));
    if (s != sizeof(uint64_t))
      errExit("read");
    totalExp += numExp;

    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) == -1)
      errExit("clock_gettime");

    int secs = now.tv_sec - start.tv_sec;
    int nanosecs = now.tv_nsec - start.tv_nsec;
    if (nanosecs < 0) {
      secs--;
      nanosecs += 1000000000;
    }
    fmt::print("{}.{:3}: expirations read: {}; total={}\n", secs,
               (nanosecs + 500000) / 1000000, numExp, totalExp);
  }
  return 0;
}