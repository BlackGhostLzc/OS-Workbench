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
  char name[64];
  double time;
} Sys_Info;

Sys_Info sys_info[400];
static int sys_info_id = 0;

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

void update_sysinfo(char *sys_name, double sys_time)
{
  for (int i = 0; i < sys_info_id; i++)
  {
    if (strcmp(sys_name, sys_info[sys_info_id].name) == 0)
    {
      sys_info[sys_info_id].time += sys_time;
      return;
    }
  }

  strcpy(sys_info[sys_info_id].name, sys_name);
  sys_info[sys_info_id].time = 0;

  sys_info_id++;
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
  // printf("%s\t%lf\n", sys_name, sys_time);

  update_sysinfo(sys_name, sys_time);
}

int myCompare(const Sys_Info *a, const Sys_Info *b)
{
  return a->time < b->time;
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

  // 排序 ， 展示
  qsort(sys_info, sys_info_id, sizeof(sys_info[0]), myCompare);
  for (int i = 0; i < sys_info_id; i++)
  {
    printf("%s\t%lf\n", sys_info[i].name, sys_info[i].time);
  }
}

// ./sperf ls -a        argc = 3
// strace -o filename -T ls -a
//  0      1     2    3  4  5  6
int main(int argc, char *argv[])
{

  fflush(stdout);
  child_argv = (char **)malloc((argc + 4) * sizeof(char *));
  child_argv[argc + 3] = NULL;
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
  }
  else
  {
    parent();
  }
}