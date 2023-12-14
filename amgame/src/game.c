
#include <game.h>

// Operating system is a C program!
int main(const char *args)
{
  ioe_init();
  printf("Hello World from " __ISA__ " program!\n");
  puts("mainargs = \"");
  puts(args); // make run mainargs=xxx
  puts("\n");

  splash();

  puts("Press any key to see its key code...\n");
  while (1)
  {
    print_key();
  }
  return 0;
}
