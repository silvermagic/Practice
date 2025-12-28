#include <cstdio>
#include <cstring>
#include <pthread.h>
extern "C" {
#include "tlpi_hdr.h"
}
#include <fmt/core.h>

static pthread_once_t once = PTHREAD_ONCE_INIT;
static pthread_key_t strerrorKey;

#define MAX_ERROR_LEN 256

static void destructor(void *buf) { free(buf); }

static void createKey() {
  int s = pthread_key_create(&strerrorKey, destructor);
  if (s != 0) {
    errExitEN(s, "pthread_key_create");
  }
}

char *strerror(int err) {
  int s = pthread_once(&once, createKey);
  if (s != 0) {
    errExitEN(s, "pthread_once");
  }

  void *buf = pthread_getspecific(strerrorKey);
  if (buf == NULL) {
    buf = malloc(MAX_ERROR_LEN);
    if (buf == NULL) {
      errExit("malloc");
    }
    s = pthread_setspecific(strerrorKey, buf);
    if (s != 0) {
      errExitEN(s, "pthread_setspecific");
    }
  }

  const char *str = std::strerror(err);
  if (str == NULL) {
    snprintf(reinterpret_cast<char *>(buf), MAX_ERROR_LEN, "Unknown error %d",
             err);
  } else {
    strncpy(reinterpret_cast<char *>(buf), str, MAX_ERROR_LEN - 1);
    reinterpret_cast<char *>(buf)[MAX_ERROR_LEN - 1] = '\0';
  }

  return reinterpret_cast<char *>(buf);
}

static void *threadFunc(void *arg) {
  fmt::print("Other thread about to call strerror()\n");
  char *str = strerror(EINVAL);
  fmt::print("Other thread: str {} = {}\n", reinterpret_cast<void *>(str), str);
  return NULL;
}

int main() {
  char *str = strerror(EINVAL);
  fmt::print("Main thread has called strerror()\n");

  pthread_t t;
  int s = pthread_create(&t, NULL, threadFunc, NULL);
  if (s != 0) {
    errExitEN(s, "pthread_create");
  }

  s = pthread_join(t, NULL);
  if (s != 0) {
    errExitEN(s, "pthread_join");
  }

  fmt::print("Main thread: str {} = {}\n", reinterpret_cast<void *>(str), str);
  return 0;
}