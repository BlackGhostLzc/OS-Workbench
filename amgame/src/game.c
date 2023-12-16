
#include <game.h>
#include <snake.h>

// 每秒的帧数
#define FPS 30

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

  // 每一个帧，都要读取一个键盘按键
  struct Snake snake;

  snake_init(&snake);

  while (1)
  {

    while (current_frame < next_frame)
    {
      get_dir(&snake);
      ioe_read(AM_TIMER_UPTIME, &time);
      current_frame = time.us / (1000000 / FPS);
    }

    printf("%d\n", next_frame);
    // 一秒 会移动 2格
    if (next_frame % 15 == 0)
    {
      printf("get here\n");
      update_snake(&snake);
      // 清空画布
      gpu_clear(w, h);
      draw_snake(&snake);
    }
    // draw_snake(&snake);
    next_frame++;
  }

  /*
  puts("Press any key to see its key code...\n");
  while (1)
  {
    print_key();
  }
  return 0;
  */
}
