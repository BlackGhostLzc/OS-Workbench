#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <regex.h>

extern char **__environ;
char **child_argv;
char file_path[64];

int pipefd[2];

void display();

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
double total_time = 0;

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
    if (strcmp(sys_name, sys_info[i].name) == 0)
    {
      sys_info[i].time += sys_time;
      return;
    }
  }

  strcpy(sys_info[sys_info_id].name, sys_name);
  sys_info[sys_info_id].time = sys_time;

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

  total_time += sys_time;

  update_sysinfo(sys_name, sys_time);
}

int myCompare(const void *a, const void *b)
{
  return ((Sys_Info *)(a))->time < ((Sys_Info *)(b))->time;
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
  // for (int i = 0; i < sys_info_id; i++)
  // {
  //   printf("%s\t%lf\n", sys_info[i].name, sys_info[i].time);
  // }

  display();
}

// ----------------- 绘图 -----------------------------
// 根据实验指南说明，其为图像中展示的不同的系统调用的个数,其余全部为 others
#define SYSCALL_INFO_SHOW_SIZE (5)

/*
 * 由于每次切割后,记录可用的矩阵的左上坐标即可
 * (row, col)
 * (0,0)			(0, width - 1)
 * _______________________________________
 * |					 |
 * |					 |
 * |_____________________________________|
 * (height - 1, 0)			(height - 1, witdh - 1)
 */

// 设置终端展示时候的窗口高
#define SYSCALL_INFO_WINDOW_HEIGHT (20)
// 设置终端展示时候的窗口宽
#define SYSCALL_INFO_WINDOW_WIDTH (80)

#define syscall_info_show_format(color) ("\e[" #color ";37m%s\e[0m")
const char *syscall_info_show_formats[SYSCALL_INFO_SHOW_SIZE] = {syscall_info_show_format(42), syscall_info_show_format(45), syscall_info_show_format(43), syscall_info_show_format(44), syscall_info_show_format(46)};
#define syscall_info_show(idx, str) (fprintf(stderr, syscall_info_show_formats[(idx)], (str)))

#define syscall_info_show_move(opcode) (fprintf(stderr, "\e[1" #opcode))
// 将当前光标上移n行，列数不变
void syscall_info_show_move_up(int idx)
{
  for (int i = 0; i < idx; ++i)
  {
    syscall_info_show_move(A);
  }
}
// 将当前光标下移n行，列数不变
void syscall_info_show_move_down(int idx)
{
  for (int i = 0; i < idx; ++i)
  {
    syscall_info_show_move(B);
  }
}
// 将当前光标左移n列，行数不变
void syscall_info_show_move_left(int idx)
{
  for (int i = 0; i < idx; ++i)
  {
    syscall_info_show_move(D);
  }
}
// 将当前光标右移n列，行数不变
void syscall_info_show_move_right(int idx)
{
  for (int i = 0; i < idx; ++i)
  {
    syscall_info_show_move(C);
  }
}
// 将光标默认移动到第0行，第0列
#define syscall_info_show_position_init() (fprintf(stderr, "\e[0;0H"))

// ./sperf ls -a        argc = 3
// strace -o filename -T ls -a
//  0      1     2    3  4  5  6
int min(int a, int b)
{
  if (a < b)
    return a;
  return b;
}

void display()
{
  // 展示五个系统调用,其他为others
  int rest_width = SYSCALL_INFO_WINDOW_WIDTH;
  int rest_height = SYSCALL_INFO_WINDOW_HEIGHT;

  for (int i = 0; i < min(SYSCALL_INFO_SHOW_SIZE, sys_info_id) - 1; i++)
  {
    syscall_info_show_position_init();
    syscall_info_show_move_down(SYSCALL_INFO_WINDOW_HEIGHT - rest_height);
    syscall_info_show_move_right(SYSCALL_INFO_WINDOW_WIDTH - rest_width);

    double percent = sys_info[i].time / total_time;

    char buf[100];
    snprintf(buf, sizeof(buf), "%.*s(%d)", 64, sys_info[i].name, (int)(percent * 100));

    if (i % 2 == 0)
    {
      int width = percent * rest_width;
      for (int h = 0; h < rest_height; h++)
      {

        if (h == rest_height / 2)
        {
          if (width >= strlen(buf))
          {
            for (int k = 0; k < (width - strlen(buf)) / 2; k++)
            {
              syscall_info_show(i, " ");
            }
            syscall_info_show(i, buf);
            for (int k = 0; k < width - (width - strlen(buf)) / 2 - strlen(buf); k++)
            {
              syscall_info_show(i, " ");
            }
          }

          syscall_info_show_move_down(1);
          syscall_info_show_move_left(width);
          continue;
        }

        for (int w = 0; w < width; w++)
        {
          syscall_info_show(i, " ");
        }
        syscall_info_show_move_down(1);
        syscall_info_show_move_left(width);
      }
      rest_width = rest_width - width;
    }
    else
    {
      int height = percent * rest_height;
      for (int h = 0; h < height; h++)
      {
        /*
        if (h == rest_height / 2)
        {
          syscall_info_show(i, sys_info[i].name);
          if (strlen(sys_info[i].name) < rest_width)
          {
            for (int t = 0; t < rest_width - strlen(sys_info[i].name); t++)
            {
              syscall_info_show(i, " ");
            }
          }

          syscall_info_show_move_down(1);
          syscall_info_show_move_left(rest_width);
          continue;
        }
        */
        for (int w = 0; w < rest_width; w++)
        {
          syscall_info_show(i, " ");
        }
        syscall_info_show_move_down(1);
        syscall_info_show_move_left(rest_width);
      }

      rest_height = rest_height - height;
    }
  }

  syscall_info_show_position_init();
  syscall_info_show_move_down(SYSCALL_INFO_WINDOW_HEIGHT - rest_height);
  syscall_info_show_move_right(SYSCALL_INFO_WINDOW_WIDTH - rest_width);

  for (int h = 0; h < rest_height; h++)
  {
    for (int w = 0; w < rest_width; w++)
    {
      syscall_info_show(SYSCALL_INFO_SHOW_SIZE - 1, " ");
    }
    syscall_info_show_move_down(1);
    syscall_info_show_move_left(rest_width);
  }
}

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