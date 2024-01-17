#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char **__environ;
char **child_argv;
char file_path[64];

int pipefd[2];

// /bin/strace -o file_path -T ls -a
// ./sperf pstree
// strace -o file_path -T pstree
void init_childargv(int argc, char *argv[])
{
  child_argv[0] = "/bin/strace";
  child_argv[1] = "-o";
  child_argv[3] = "-T";

  // child_argv[4] = argv[1]    child_argv[5] = argv[2]
  for (int i = 1; i < argc; i++)
  {
    child_argv[i + 3] = argv[i];
  }

  // child_argv[argc + 4] = "NULL";
}

void child()
{
  // 子进程首先关闭读口
  close(pipefd[0]);

  // strace -o filename -T ls -a
  // /proc/pid/fd/ 目录下是进程所打开的全部文件描述符，这是个符号链接
  sprintf(file_path, "/proc/%d/fd/%d", getpid(), pipefd[1]);

  child_argv[2] = file_path;

  // 为 child_argv[4] 加上路径前缀
  char *tmp[] = {"/bin/trace", "-o", "trace.txt", "-T", "pwd", NULL};
  execve("/bin/strace", tmp, __environ);
  printf("hh\n");
  printf("Should never get here\n");
}

void parent()
{
  // 父进程关闭写口
  close(pipefd[1]);
}

// ./sperf ls -a
int main(int argc, char *argv[])
{
  printf("argc: %d\n", argc);
  // fflush(stdout);
  child_argv = (char **)malloc((argc + 4) * sizeof(char *));
  init_childargv(argc, argv);

  if (pipe(pipefd) < 0)
  {
    printf("Error when initializing pipe\n");
    exit(0);
  }

  pid_t pid = fork();
  if (pid == 0)
  {
    child();
    while (1)
      ;
  }
  else
  {
    parent();
    while (1)
      ;
  }
}