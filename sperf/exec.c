#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char **__environ;

int main()
{
    char *child_argv[] = {"/bin/strace", "-o", "trace.txt", "-T", "pstree", NULL};
    execve("/bin/strace", child_argv, __environ);
}