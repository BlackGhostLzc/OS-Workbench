#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/wait.h>

char libc_name[] = "./tmp/lib.c";
char libso_name[] = "./tmp/lib.so";
char *gcc_option[] = {
    "gcc",
    "-shared",
    "-fPIC",
    libc_name,
    "-o",
    libso_name,
    NULL,
};

void update_lib()
{
  if (fork() == 0)
  {
    execvp("gcc", gcc_option);
    perror("gcc error");
    exit(-1);
  }
  wait(NULL);
}

int wrapper_num = 0;

int main(int argc, char *argv[])
{
  static char line[4096];

  int (*wrapper)();

  while (1)
  {
    printf("crepl> ");
    fflush(stdout);
    if (!fgets(line, sizeof(line), stdin))
    {
      break;
    }

    if (strlen(line) > 3 && strncmp(line, "int", 3) == 0)
    {
      // 是个函数申明
      // int func_name(int a, ...) {....; return x;}
      FILE *f = fopen(libc_name, "a");
      fprintf(f, "\n%s\n", line);
      fclose(f);
      printf("Got %zu chars.\n", strlen(line)); // ??
    }
    else
    {
      // 这是一个表达式.需要构造一个wrapper， wrapper_0  wrapper_1
      char wrapper_buf[1024];
      printf("here\n");
      FILE *f = fopen(libc_name, "a");
      fprintf(f, "%s%s%d%s%s%s",
              "int ", "wrapper_", wrapper_num, "(){ return ", line, "; }\n");
      printf("here\n");
      update_lib();

      void *libHandle = dlopen(libso_name, RTLD_LAZY);
      if (!libHandle)
      {
        printf("handle error\n");
        continue;
      }

      sprintf(wrapper_buf, "%s%d", "wrapper_", wrapper_num);
      wrapper = (int (*)())dlsym(libHandle, wrapper_buf);
      if (!wrapper)
      {
        printf("wrapper error\n");
      }

      int res = wrapper();

      printf("%d\n", res);
      wrapper_num++;
      fclose(f);
    }
  }
}
