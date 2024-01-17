#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum bool
{
  false=0,
  true
};

void print_path(ino_t inode, enum bool flag);
void inode_to_name(ino_t inode, char *buf, int len);
ino_t get_inode(const char *name);

int main()
{
  print_path(get_inode("."), false);
  printf("\n");
  return 0;
}

void print_path(ino_t inode, enum bool flag)
{
  char its_name[BUFSIZ] = "/";
  ino_t parent_inode = get_inode("..");
  if (inode != parent_inode) 
  {
    if (-1 == chdir(".."))
    {
      perror("chdir");
      exit(1);
    }
  
    inode_to_name(inode, its_name, BUFSIZ);
    print_path(parent_inode, true);
    if (flag)
      printf("%s/", its_name);
    else
      printf("%s", its_name);
  }
  else
    printf("/");
}

void inode_to_name(ino_t inode, char *buf, int len)
{
  DIR *dirp;
  if (NULL == (dirp=opendir(".")))
  {
    perror(".");
    exit(1);
  }

  struct dirent *info;
  while (NULL != (info = readdir(dirp)))
  {
    if (info->d_ino == inode)
    {
      snprintf(buf, len, "%s", info->d_name);
      closedir(dirp);
      return;
    }
  }

  fprintf(stderr, "Cannot find inode %d\n", inode);
  exit(1);
}

ino_t get_inode(const char *name)
{
  struct stat buf;
  if (-1 == stat(name, &buf))
  {
    fprintf(stderr, "Cannot stat");
    perror(name);
    exit(1);
  }

  return buf.st_ino;
}
