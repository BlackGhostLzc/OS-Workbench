
#include <game.h>

// Operating system is a C program!
int main(const char *args)
{
  ioe_init();
  // printf("Hello World from " __ISA__ " program!\n");
  puts("mainargs = \"");
  puts(args); // make run mainargs=xxx
  puts("\n");

  AM_GPU_CONFIG_T info = {0};
  ioe_read(AM_GPU_CONFIG, &info);
  int w = info.width;
  int h = info.height;
  printf("w = %d and h = %d\n", w, h);

  splash();

  puts("Press any key to see its key code...\n");
  while (1)
  {
    print_key();
  }
  return 0;
}
