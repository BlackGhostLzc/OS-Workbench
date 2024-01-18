#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <regex.h>

extern char **__environ;
char **child_argv;
char file_path[64];

int pipefd[2];

// /bin/strace -o file_path -T ls -a
// ./sperf pstree
// strace -o file_path -T pstree NULL
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
}

void child()
{
  // 子进程首先关闭读口
  close(pipefd[0]);

  // strace -o filename -T ls -a
  // /proc/pid/fd/ 目录下是进程所打开的全部文件描述符，这是个符号链接

  // 管道的大小不是无限大的，而是一个缓冲区，缓冲区满了就不能再写了。
  sprintf(file_path, "/proc/%d/fd/%d", getpid(), pipefd[1]);

  child_argv[2] = file_path;

  execve("/bin/strace", child_argv, __environ);
  printf("Should never get here\n");
}

typedef struct Sys_Info
{
  char *name;
  double time;
} Sys_Info;

Sys_Info sys_info[400];

int ifexist(const char *line, char target)
{
  int idx = 0;
  int len = strlen(line);
  while (idx < len)
  {
    if (line[idx] == target)
    {
      return 1;
    }
    idx++;
  }
  return 0;
}

void handle_line(char *line)
{
  // 获取系统调用的名字 和 时间
  if (!ifexist(line, '<') && !ifexist(line, '>'))
  {
    return;
  }
  char sys_name[64];
  char systimechar[64];
  double sys_time = 0;
  int idx = 0;
  while (line[idx] != '(')
  {
    sys_name[idx] = line[idx];
    idx++;
  }

  sscanf(line, "%*[^<]<%64[^>]>", systimechar);
  sscanf(systimechar, "%lf", &sys_time);
  printf("%s\t%lf\n", sys_name, sys_time);
}

void parent()
{
  // 父进程关闭写口
  close(pipefd[1]);

  // 不断读取 strace 的输出
#define MAX_LEN 4096

  char buffer[MAX_LEN];
  char line[1024];

  size_t byte_read = 0;
  int buffer_idx = 0;
  int line_idx = 0;

  int flag = 0;

  while ((byte_read = read(pipefd[0], buffer, MAX_LEN)) > 0)
  {
  line_read:
    while (buffer_idx < byte_read)
    {
      line[line_idx++] = buffer[buffer_idx++];
      if (line[line_idx - 1] == '\n')
      {
        flag = 1;
        break;
      }
    }

    if (flag)
    {
      // printf("%s", line);

      // 处理这行输出 .....
      handle_line(line);

      line_idx = 0;
      flag = 0;
      memset(line, 0, sizeof(line));

      goto line_read;
    }
    else
    {
      // 接着读取
      buffer_idx = 0;
      continue;
    }
  }
}

// ./sperf ls -a
int main(int argc, char *argv[])
{

  fflush(stdout);
  child_argv = (char **)malloc((argc + 4) * sizeof(char *));
  child_argv[argc + 4] = NULL;

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