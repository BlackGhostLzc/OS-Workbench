#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    pid_t pid = fork();
    if (pid == 0)
    {
    }
    else
    {
        while (1)
            ;
    }
}