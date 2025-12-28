#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <unistd.h>
#include <time.h>

void do_ls(char dirname[]);
void dostat(const char *pathname);
void show_file_info(const char *pathname, const struct stat *info_p);
void mode_to_letters(mode_t mode, char str[]);
char *uid_to_name(uid_t id);
char *gid_to_name(gid_t id);

int main(int argc, char *argv[])
{
  if (1 == argc)
    do_ls(".");
  else
  {
    int i = 1;
    while(i < argc)
    {
      printf("$s:\n", argv[i]);
      do_ls(argv[i]);
      i++;
    }
  }
  return 0;
}

void do_ls(char dirname[])
{
  DIR *dirp;
  if (NULL == (dirp = opendir(dirname)))
   fprintf(stderr, "ls: cannot open", dirname);
  else
  {
    struct dirent *entry;
    while(NULL != (entry = readdir(dirp)))
      dostat(entry->d_name);
    closedir(dirp);
  }
}

void dostat(const char *pathname)
{
  struct stat info;
  if (-1 == stat(pathname, &info))
    perror(pathname);
  else
    show_file_info(pathname, &info);
}

void show_file_info(const char *pathname, const struct stat *info_p)
{
  char modestr[12] = "----------.";
  mode_to_letters(info_p->st_mode, modestr);

  printf("%s", modestr);
  printf("%2d ", info_p->st_nlink);
  printf("%-7s", uid_to_name(info_p->st_uid));
  printf("%-7s", gid_to_name(info_p->st_gid));
  printf("%5ld ", info_p->st_size);
  printf("%.12s ", 4 + ctime(&info_p->st_mtime));
  printf("%s\n", pathname);
}

void mode_to_letters(mode_t mode, char str[])
{
  if (S_ISBLK(mode)) str[0]='b';
  if (S_ISCHR(mode)) str[0]='c';
  if (S_ISDIR(mode)) str[0]='d';

  if (mode & S_IRUSR) str[1]='r';
  if (mode & S_IWUSR) str[2]='w';
  if (mode & S_IXUSR) str[3]='x';

  if (mode & S_IRGRP) str[4]='r';
  if (mode & S_IWGRP) str[5]='w';
  if (mode & S_IXGRP) str[6]='x';

  if (mode & S_IROTH) str[7]='r';
  if (mode & S_IWOTH) str[8]='w';
  if (mode & S_IXOTH) str[9]='x';
}

char *uid_to_name(uid_t id)
{
  static char buf[16];
  struct passwd *info = getpwuid(id);
  if (NULL != info)
    return info->pw_name;
  else
  {
    snprintf(buf, sizeof(buf), "%d", id);
    return buf;
  }
}

char *gid_to_name(gid_t id)
{
  static char buf[16];
  struct group *info = getgrgid(id);
  if (NULL != info)
    return info->gr_name;
  else
  {
    snprintf(buf, sizeof(buf), "%d", id);
    return buf;
  }
}
