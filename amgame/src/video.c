
#include <game.h>
#include <snake.h>

#define SIDE 16
static int w, h;

static void init()
{
  AM_GPU_CONFIG_T info = {0};
  ioe_read(AM_GPU_CONFIG, &info);
  w = info.width;
  h = info.height;
}

static void draw_tile(int x, int y, int w, int h, uint32_t color)
{
  uint32_t pixels[w * h]; // WARNING: large stack-allocated memory
  AM_GPU_FBDRAW_T event = {
      .x = x,
      .y = y,
      .w = w,
      .h = h,
      .sync = 1,
      .pixels = pixels,
  };
  for (int i = 0; i < w * h; i++)
  {
    pixels[i] = color;
  }
  ioe_write(AM_GPU_FBDRAW, &event);
}

void splash()
{
  init();
  for (int x = 0; x * SIDE <= w; x++) // SIDE = 16  表示一个方格（填充最小块）的像素
  {
    for (int y = 0; y * SIDE <= h; y++)
    {
      if ((x & 1) ^ (y & 1))
      {
        draw_tile(x * SIDE, y * SIDE, SIDE, SIDE, 0xffffff); // white
      }
    }
  }
}

void draw_snake(struct Snake *snake)
{
  int sz = snake->size;
  for (int i = 0; i < sz; i++)
  {
    draw_tile((snake->pos[i]).x * SIDE, (snake->pos[i]).y * SIDE, SIDE, SIDE, 0xffffff);
  }
}

void gpu_clear(int width, int height)
{
  for (int i = 0; i < width / SIDE; i++)
  {
    for (int j = 0; j < height / SIDE; j++)
    {
      draw_tile(i * SIDE, j * SIDE, SIDE, SIDE, 0x000000);
    }
  }
}

void draw_food(struct Position *food)
{
  draw_tile(food->x * SIDE, food->y * SIDE, SIDE, SIDE, 0x624124);
}