
#include <game.h>

// 每秒的帧数
#define FPS 1

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

  // splash();
  uint32_t next_frame = 0;
  uint32_t current_frame = 0;
  AM_TIMER_UPTIME_T time;

  while (1)
  {
    while (current_frame < next_frame)
    {
      ioe_read(AM_TIMER_UPTIME, &time);
      current_frame = time.us / (1000000 / FPS);
      continue;
    }
    printf("x\n");
    next_frame++;
  }

  puts("Press any key to see its key code...\n");
  while (1)
  {
    print_key();
  }
  return 0;
}
