#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
extern "C" {
#include "tlpi_hdr.h"
}
#include <fmt/core.h>

int main(int argc, char *argv[]) {
  if (argc != 2 || strcmp(argv[1], "--help") == 0)
    usageErr("%s filename\n", argv[0]);

  int fd = shm_open(argv[1], O_RDONLY, 0);
  if (fd == -1)
    errExit("shm_open");

  struct stat sb;
  if (fstat(fd, &sb) == -1)
    errExit("fstat");

  void *addr = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
  if (addr == MAP_FAILED)
    errExit("mmap");

  if (close(fd) == -1)
    errExit("close");

  fmt::print("read {} bytes: {}\n", static_cast<long>(sb.st_size),
             static_cast<char *>(addr));
  return 0;
}