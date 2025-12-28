#include <semaphore.h>
#include <sys/stat.h>
#include <thread>
extern "C" {
#include "tlpi_hdr.h"
}
#include <fmt/core.h>

static sem_t sem;
static int glob = 0;

int main(int argc, char *argv[]) {
  if (argc > 2 || strcmp(argv[1], "--help") == 0)
    usageErr("%s [num-loops]\n", argv[0]);

  int numLoops = (argc > 1) ? getInt(argv[1], GN_GT_0, "num-loops") : 1000000;

  if (sem_init(&sem, 0, 1) == -1)
    errExit("sem_init");

  auto func = [](int loops) {
    for (int i = 0; i < loops; i++) {
      if (sem_wait(&sem) == -1)
        errExit("sem_wait");

      int loc = glob;
      loc++;
      glob = loc;
      if (sem_post(&sem) == -1)
        errExit("sem_post");
    }

    return;
  };
  std::thread t1(func, numLoops);
  std::thread t2(func, numLoops);
  t1.join();
  t2.join();

  fmt::print("global = {}\n", glob);
  return 0;
}