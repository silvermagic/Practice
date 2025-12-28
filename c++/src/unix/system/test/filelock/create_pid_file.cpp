#include <fcntl.h>
#include <filesystem>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
extern "C" {
#include "tlpi_hdr.h"
}
#include <fmt/core.h>

int main(int argc, char *argv[]) {
  if (argc != 1)
    usageErr("Usage: {}", argv[0]);

  std::string programName = std::filesystem::path(argv[0]).filename().string();
  std::string pidFile = fmt::format("/tmp/{}.pid", programName);
  int fd = open(pidFile.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  if (fd == -1)
    errExit("Could not open PID file {}", pidFile);

  struct flock fl;
  fl.l_type = F_WRLCK;
  fl.l_whence = SEEK_SET;
  fl.l_start = 0;
  fl.l_len = 0;
  if (fcntl(fd, F_SETLK, &fl) == -1) {
    if (errno == EAGAIN || errno == EACCES) {
      fmt::print("PID file {} is already locked\n", pidFile);
      return 0;
    } else {
      errExit("Unable to lock PID file {}", pidFile);
    }
  }

  if (ftruncate(fd, 0) == -1)
    errExit("Could not truncate PID file {}", pidFile);

  std::string pid = fmt::format("{}", getpid());
  if (write(fd, pid.c_str(), pid.size()) != pid.size())
    errExit("Writing to PID file {} failed", pidFile);

  sleep(10);

  fmt::print("{}: exiting\n", programName);
  return 0;
}