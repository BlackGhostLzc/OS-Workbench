#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdio.h>

// 声明外部变量 environ
extern char **environ;

int main()
{
  char *path = getenv("PATH");
  printf("%s\n", path);
  printf("%c", PATH_SPLIT);
}