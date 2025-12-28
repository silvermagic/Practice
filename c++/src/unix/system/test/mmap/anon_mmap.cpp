#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
extern "C" {
#include "tlpi_hdr.h"
}
#include <fmt/core.h>

int main(int argc, char *argv[]) {
  void *addr;

#ifdef USE_MAP_ANON
  addr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
              MAP_ANONYMOUS | MAP_SHARED, -1, 0);
  if (addr == MAP_FAILED)
    errExit("mmap");
#else
  int fd = open("/dev/zero", O_RDWR);
  if (fd == -1)
    errExit("open");
  addr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (addr == MAP_FAILED)
    errExit("mmap");
  if (close(fd) == -1)
    errExit("close");
#endif

  *static_cast<int *>(addr) = 1;
  switch (fork()) {
  case -1:
    errExit("fork");
  case 0:
    fmt::print("Child started, value = {}\n", *static_cast<int *>(addr));
    (*static_cast<int *>(addr))++;

    if (munmap(addr, sizeof(int)) == -1)
      errExit("munmap");
    return 0;
  default:
    if (wait(NULL) == -1)
      errExit("wait");
    fmt::print("In parent, value = {}\n", *static_cast<int *>(addr));
    if (munmap(addr, sizeof(int)) == -1)
      errExit("munmap");
    return 0;
  }
}