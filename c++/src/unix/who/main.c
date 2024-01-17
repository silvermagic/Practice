#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <utmp.h>
#include <fcntl.h>
#include <time.h>

void show_time(time_t t)
{
  struct tm *local_time = localtime(&t);
  char str_f_t[64] = {0};
  strftime(str_f_t, sizeof(str_f_t), "%G-%m-%d %H:%M", local_time);
  printf("%-16s", str_f_t);
}

void show_info(struct utmp *info)
{
  if (USER_PROCESS != info->ut_type)
    return;

  printf("%-8.8s", info->ut_name);
  printf(" ");
  printf("%-12s", info->ut_line);
  printf(" ");
  show_time(info->ut_time);
  printf(" (%s)", info->ut_host);
  printf("\n");
}

int main()
{
  int utmpfd;
  
  if (-1 == (utmpfd = open(UTMP_FILE, O_RDONLY)))
  {
    perror(UTMP_FILE);
    return -1;
  }
 
  struct utmp info;
  while (read(utmpfd, &info, sizeof(struct utmp)) > 0)
    show_info(&info);

  close(utmpfd);
  return 0;
}
