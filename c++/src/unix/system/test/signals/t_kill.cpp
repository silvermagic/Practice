#include <signal.h>
extern "C" {
#include "tlpi_hdr.h"
}
#include <fmt/core.h>

int main(int argc, char *argv[]) {
  int s, sig;

  if (argc != 3 || strcmp(argv[1], "--help") == 0)
    usageErr("%s sig-num pid\n", argv[0]);

  sig = getInt(argv[2], 0, "sig-num");
  s = kill(getLong(argv[1], 0, "pid"), sig);

  if (sig != 0) {
    if (s == -1)
      errExit("kill");
  } else {
    if (s == 0) {
      fmt::println("Process exists and we can send it a signal");
    } else {
      if (errno == EPERM) {
        fmt::println(
            "Process exists, but we don't have permission to send it a signal");
      } else if (errno == ESRCH) {
        fmt::println("Process does not exist");
      } else {
        errExit("kill");
      }
    }
  }

  return 0;
}