#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  static char line[4096];

  char filename[] = "/tmp/lib.c";
  int fd = mkstemp(filename);

  while (1)
  {
    printf("crepl> ");
    fflush(stdout);
    if (!fgets(line, sizeof(line), stdin))
    {
      break;
    }
    write(fd, line, strlen(line));

    printf("Got %zu chars.\n", strlen(line)); // ??
  }
}
