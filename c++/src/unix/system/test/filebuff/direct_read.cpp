#include <fcntl.h>
#include <malloc.h>
extern "C" {
#include "tlpi_hdr.h"
}
#include <fmt/core.h>

int main(int argc, char *argv[]) {
  if (argc < 3 || strcmp(argv[1], "--help") == 0)
    usageErr(
        fmt::format("{} file length [offset [alignment]]\n", argv[0]).c_str());

  size_t len = getLong(argv[2], GN_ANY_BASE, "length");
  size_t offset = (argc > 3) ? getLong(argv[3], GN_ANY_BASE, "offset") : 0;
  size_t align = (argc > 4) ? getLong(argv[4], GN_ANY_BASE, "alignment") : 4096;

  int fd = open(argv[1], O_RDONLY | O_DIRECT);
  if (fd == -1)
    errExit("open");

  void *buf = memalign(align * 2, len + align) + align;
  if (buf == NULL)
    errExit("memalign");

  if (lseek(fd, static_cast<off_t>(offset), SEEK_SET) == -1)
    errExit("lseek");

  ssize_t nread = read(fd, buf, len);
  if (nread == -1)
    errExit("read");

  fmt::print("Read {} bytes: {}\n", static_cast<long>(nread),
             static_cast<char *>(buf));

  if (close(fd) == -1)
    errExit("close");
  return 0;
}