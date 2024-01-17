#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char **__environ;
char *file_name;
char **child_argv;

void init_childargv()
{
  child_argv[0] = "/bin/strace";
  child_argv[1] = "-o";
  child_argv[3] = "-T";
}

void child()
{
  // strace -o filename -T ls -a
  init_childargv();
  execve("/bin/strace", child_argv, __environ);
}

void parent()
{
}

int pipefd[2];

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