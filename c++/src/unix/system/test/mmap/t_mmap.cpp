#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
extern "C" {
#include "tlpi_hdr.h"
}
#include <fmt/core.h>

int main(int argc, char *argv[]) {
  if (argc < 2 || strcmp(argv[1], "--help") == 0)
    usageErr(fmt::format("{} file [new-value]\n", argv[0]).c_str());

  int fd = open(argv[1], O_RDWR);
  if (fd == -1)
    errExit("open");

  struct stat sb;
  if (fstat(fd, &sb) == -1)
    errExit("fstat");

  void *addr =
      mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (addr == MAP_FAILED)
    errExit("mmap");

  if (close(fd) == -1)
    errExit("close");

  if (argc > 2) {
    int len = strlen(argv[2]);
    if (len > sb.st_size)
      cmdLineErr(fmt::format("'new-value' too long\n").c_str());

    memset(addr, 0, len);
    strncpy(static_cast<char *>(addr), argv[2], len);
    if (msync(addr, len, MS_SYNC) == -1)
      errExit("msync");
  }

  return 0;
}
