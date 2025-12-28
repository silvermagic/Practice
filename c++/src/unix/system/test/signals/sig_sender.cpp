#include <signal.h>
extern "C" {
#include "tlpi_hdr.h"
}
#include <fmt/core.h>

int main(int argc, char *argv[]) {
  int numSigs, sig;
  pid_t pid;

  if (argc < 4 || strcmp(argv[1], "--help") == 0)
    usageErr("%s pid num sig [sig-2]\n", argv[0]);

  pid = getLong(argv[1], 0, "PID");
  numSigs = getInt(argv[2], GN_GT_0, "num");
  sig = getInt(argv[3], 0, "sig");

  fmt::print("{} sending signal {} to process {} {} times\n", argv[0], sig,
             static_cast<long>(pid), numSigs);

  for (int i = 0; i < numSigs; i++) {
    if (kill(pid, sig) == -1)
      errExit("kill");
  }

  if (argc > 4) {
    if (kill(pid, getInt(argv[4], 0, "sig-2")) == -1)
      errExit("kill");
  }

  fmt::print("{}: exiting\n", argv[0]);
  return 0;
}