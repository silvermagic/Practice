#include <signal.h>
extern "C" {
#include "tlpi_hdr.h"
}
#include <fmt/core.h>

int main(int argc, char *argv[]) {
  int sig, numSigs, sigData;
  union sigval sv;

  if (argc < 4 || strcmp(argv[1], "--help") == 0)
    usageErr(fmt::format("{} pid sig-num data [num-sigs]\n", argv[0]).c_str());

  fmt::print("{}: PID is {}, UID is {}\n", argv[0], static_cast<long>(getpid()),
             static_cast<long>(getuid()));

  sig = getInt(argv[2], 0, "sig-num");
  sigData = getInt(argv[3], 0, "data");
  numSigs = (argc > 4) ? getInt(argv[4], GN_GT_0, "num-sigs") : 1;

  for (int i = 0; i < numSigs; i++) {
    sv.sival_int = sigData + i;
    if (sigqueue(getLong(argv[1], 0, "pid"), sig, sv) == -1)
      errExit(fmt::format("sigqueue: {}", i).c_str());
  }

  return 0;
}