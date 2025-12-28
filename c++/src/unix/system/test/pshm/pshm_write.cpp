#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
extern "C" {
#include "tlpi_hdr.h"
}
#include <fmt/core.h>

int main(int argc, char *argv[]) {
  if (argc != 3 || strcmp(argv[1], "--help") == 0)
    usageErr("%s shm-name string\n", argv[0]);

  int fd = shm_open(argv[1], O_RDWR, 0);
  if (fd == -1)
    errExit("shm_open");

  size_t len = strlen(argv[2]);
  if (ftruncate(fd, len) == -1)
    errExit("ftruncate");

  void *addr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (addr == MAP_FAILED)
    errExit("mmap");

  if (close(fd) == -1)
    errExit("close");

  fmt::print("copying {} bytes", static_cast<long>(len));
  memcpy(addr, argv[2], len);
  if (msync(addr, len, MS_SYNC) == -1)
    errExit("msync");

  return 0;
}