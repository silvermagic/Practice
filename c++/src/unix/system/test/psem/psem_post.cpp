#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
extern "C" {
#include "tlpi_hdr.h"
}
#include <fmt/core.h>

int main(int argc, char *argv[]) {
  if (argc != 2 || strcmp(argv[1], "--help") == 0)
    usageErr("%s sem-name\n", argv[0]);

  sem_t *sem = sem_open(argv[1], 0);
  if (sem == SEM_FAILED)
    errExit("sem_open");

  if (sem_post(sem) == -1)
    errExit("sem_post");

  return 0;
}