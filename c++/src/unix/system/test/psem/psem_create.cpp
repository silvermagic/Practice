#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
extern "C" {
#include "tlpi_hdr.h"
}
#include <fmt/core.h>

static void usageError(const char *progName) {
  fmt::print("Usage: {} [-cx] name [octal-perms] [value]\n", progName);
  fmt::print("    -c   Create semaphore (O_CREAT)\n");
  fmt::print("    -x   Create exclusively (O_EXCL)\n");
}

int main(int argc, char *argv[]) {
  int flags = 0, opt;
  while ((opt = getopt(argc, argv, "cx")) != -1) {
    switch (opt) {
    case 'c':
      flags |= O_CREAT;
      break;
    case 'x':
      flags |= O_EXCL;
      break;
    default:
      usageError(argv[0]);
      return -1;
    }
  }

  if (optind >= argc) {
    usageError(argv[0]);
    return -1;
  }

  mode_t perms = (argc <= optind + 1)
                     ? (S_IRUSR | S_IWUSR)
                     : getInt(argv[optind + 1], GN_BASE_8, "octal-perms");
  unsigned int value =
      (argc <= optind + 2) ? 0 : getInt(argv[optind + 2], 0, "value");

  sem_t *sem = sem_open(argv[optind], flags, perms, value);
  if (sem == SEM_FAILED)
    errExit("sem_open");

  return 0;
}