#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char **__environ;
char *file_name;
char **child_argv;

int pipefd[2];

void init_childargv()
{
  child_argv[0] = "/bin/strace";
  child_argv[1] = "-o";
  child_argv[3] = "-T";
}

void child()
{
  // 子进程首先关闭读口
  close(pipefd[0]);

  // strace -o filename -T ls -a
  init_childargv();
  char file_path[64];
  // /proc/pid/fd/ 目录下是进程所打开的全部文件描述符，这是个符号链接
  sprintf(file_path, "/proc/%d/fd/%d", getpid(), pipefd[1]);
  child_argv[2] = file_path;
  printf("here\n");
  execve("/bin/strace", child_argv, __environ);
  printf("Should never get here\n");
}

void parent()
{
}

// ./sperf ls -a
int main(int argc, char *argv[])
{
  child_argv = (char **)malloc((argc + 3) * sizeof(char *));

  if (pipe(pipefd) < 0)
  {
    printf("Error when initializing pipe\n");
    exit(0);
  }
  pid_t pid = fork();
  if (pid == 0)
  {
    child();
  }
  else
  {
    parent();
  }
}