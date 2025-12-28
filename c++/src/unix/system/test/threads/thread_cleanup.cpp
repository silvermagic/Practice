#include <pthread.h>
#include <unistd.h>
extern "C" {
#include "tlpi_hdr.h"
}
#include <fmt/core.h>

static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
static int glob = 0;

static void cleanupHandler(void *arg) {
  fmt::print("Cleanup: freeing block at {}\n", reinterpret_cast<void *>(arg));
  free(arg);
  fmt::print("Cleanup: unlocking mutex\n");
  int s = pthread_mutex_unlock(&m);
  if (s != 0)
    errExitEN(s, "pthread_mutex_unlock");
}

static void *threadFunc(void *arg) {
  void *buf = malloc(1024);
  fmt::print("Thread allocated memory at {}\n", reinterpret_cast<void *>(buf));

  int s = pthread_mutex_lock(&m);
  if (s != 0)
    errExitEN(s, "pthread_mutex_lock");

  pthread_cleanup_push(cleanupHandler, buf);

  while (glob == 0) {
    s = pthread_cond_wait(&cond, &m);
    if (s != 0)
      errExitEN(s, "pthread_cond_wait");
  }

  fmt::print("Thread: condition signaled\n");
  pthread_cleanup_pop(1);
  return nullptr;
}

int main(int argc, char *argv[]) {
  pthread_t t;
  int s = pthread_create(&t, nullptr, threadFunc, nullptr);
  if (s != 0)
    errExitEN(s, "pthread_create");

  sleep(3);

  if (argc == 1) {
    fmt::print("Main:   about to cancel thread\n");
    int s = pthread_cancel(t);
    if (s != 0)
      errExitEN(s, "pthread_cancel");
  } else {
    fmt::print("Main:   about to signal condition variable\n");
    glob = 1;
    s = pthread_cond_signal(&cond);
    if (s != 0)
      errExitEN(s, "pthread_cond_signal");
  }

  void *res;
  s = pthread_join(t, &res);
  if (s != 0)
    errExitEN(s, "pthread_join");

  if (res == PTHREAD_CANCELED)
    fmt::print("Main:   thread was canceled\n");
  else
    fmt::print("Main:   thread terminated normally\n");

  return 0;
}