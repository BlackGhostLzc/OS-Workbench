#include <common.h>

static void os_init()
{
  pmm->init();
}

static void os_run()
{
  for (const char *s = "Hello World from CPU #*\n"; *s; s++)
  {
    putch(*s == '*' ? '0' + cpu_current() : *s);
  }

  while (1)
  {
    int size = rand() + 23;
    void *addr = pmm->alloc(size);
    printf("%d allocate memory size\t", cpu_current());
    printf("%d\t", size);
    printf("at address %p \n", addr);
    int x = 100000000;
    while (x)
    {
      x--;
    }

    pmm->free(addr);
  }

  while (1)
    ;
}

MODULE_DEF(os) = {
    .init = os_init,
    .run = os_run,
};
