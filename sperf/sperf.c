#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdio.h>

// 声明外部变量 environ
extern char **environ;

int main()
{
  // 遍历环境变量列表
  // for (char **env = environ; *env != NULL; env++)
  // {
  //   // 打印每个环境变量
  //   printf("%s\n", *env);
  // }

  char *path = getenv("PATH");
  printf("%s\n", path);
  return 0;
}