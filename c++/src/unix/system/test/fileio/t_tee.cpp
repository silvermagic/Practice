#include "fmt/core.h"
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
extern "C" {
#include "tlpi_hdr.h"
}

int main(int argc, char *argv[]) {
  if (argc != 2 || strcmp(argv[1], "--help") == 0)
    usageErr(fmt::format("Usage: {} <file>\n", basename(argv[0])).c_str());

  int filefd = open(argv[1], O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
  if (filefd == -1)
    errExit("open");

  int pipefd_stdout[2];
  if (pipe(pipefd_stdout) == -1)
    errExit("pipe");

  int pipefd_file[2];
  if (pipe(pipefd_file) == -1)
    errExit("pipe");

  int ret = splice(STDIN_FILENO, NULL, pipefd_stdout[1], NULL, 32768,
                   SPLICE_F_MORE | SPLICE_F_MOVE);
  if (ret == -1)
    errExit("splice");
  ret = tee(pipefd_stdout[0], pipefd_file[1], 32768, SPLICE_F_NONBLOCK);
  if (ret == -1)
    errExit("tee");
  ret = splice(pipefd_file[0], NULL, filefd, NULL, 32768,
               SPLICE_F_MORE | SPLICE_F_MOVE);
  if (ret == -1)
    errExit("splice");
  ret = splice(pipefd_stdout[0], NULL, STDOUT_FILENO, NULL, 32768,
               SPLICE_F_MORE | SPLICE_F_MOVE);
  if (ret == -1)
    errExit("splice");

  close(filefd);
  close(pipefd_stdout[0]);
  close(pipefd_stdout[1]);
  close(pipefd_file[0]);
  close(pipefd_file[1]);
  return 0;
}