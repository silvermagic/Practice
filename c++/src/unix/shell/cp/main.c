#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFERSIZE 4096

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    fprintf(stderr, "usage: %s source destination\n", argv[0]);
    return -1;
  }

  int in_fd, out_fd, n_chars;
  if (-1 == (in_fd = open(argv[1], O_RDONLY)))
  {
    perror(argv[1]);
    return -2;
  }
  if (-1 == (out_fd = open(argv[2], O_CREAT|O_WRONLY|O_EXCL)))
  {
    perror(argv[2]);
    close(in_fd);
    return -3;
  }

  char buf[BUFFERSIZE];
  while ((n_chars = read(in_fd, buf, BUFFERSIZE)) > 0)
  {
    if (write(out_fd, buf, n_chars) != n_chars)
    {
      perror(argv[2]);
      close(in_fd);
      close(out_fd);
      return -4;
    }
  }

  return 0;
}
