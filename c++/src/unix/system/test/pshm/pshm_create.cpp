#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
extern "C" {
#include "tlpi_hdr.h"
}
#include <fmt/core.h>

static void usageError(const char *progName) {
  fmt::print("Usage: {} [-cx] name size [octal-perms]\n", progName);
  fmt::print("    -c   Create semaphore (O_CREAT)\n");
  fmt::print("    -x   Create exclusively (O_EXCL)\n");
}

int main(int argc, char *argv[]) {
  int flags = O_RDWR, opt;
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

  if (optind + 1 >= argc) {
    usageError(argv[0]);
    return -1;
  }

  size_t size = getLong(argv[optind + 1], GN_ANY_BASE, "size");
  mode_t perms = (argc <= optind + 2)
                     ? (S_IRUSR | S_IWUSR)
                     : getLong(argv[optind + 2], GN_BASE_8, "octal-perms");

  int fd = shm_open(argv[optind], flags, perms);
  if (fd == -1)
    errExit("shm_open");

  if (ftruncate(fd, size) == -1)
    errExit("ftruncate");

  void *addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (addr == MAP_FAILED)
    errExit("mmap");

  return 0;
}