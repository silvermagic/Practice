#include <pthread.h>
extern "C" {
#include "tlpi_hdr.h"
}
#include <fmt/core.h>

static void *threadFunc(void *arg) {
  fmt::print("New thread started\n");

  for (int i = 0;; i++) {
    fmt::print("Loop {}\n", i);
    sleep(1);
  }

  return nullptr;
}

int main(int argc, char *argv[]) {
  pthread_t t;
  int s = pthread_create(&t, nullptr, threadFunc, nullptr);
  if (s != 0)
    errExitEN(s, "pthread_create");

  sleep(3);
  s = pthread_cancel(t);
  if (s != 0)
    errExitEN(s, "pthread_cancel");

  void *res;
  s = pthread_join(t, &res);
  if (s != 0)
    errExitEN(s, "pthread_join");

  if (res == PTHREAD_CANCELED)
    fmt::print("Thread was canceled\n");
  else
    fmt::print("Thread was not canceled (should not happen)\n");

  return 0;
}
