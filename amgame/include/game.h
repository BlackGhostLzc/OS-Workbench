#include <am.h>
#include <amdev.h>
#include <klib.h>
#include <klib-macros.h>

#define SNAKE_MAXSIZE 24

void splash();
void print_key();

struct Snake;

void draw_snake(struct Snake *snake);

enum Direction
{
  None = 0,
  UP = 1,
  LEFT = 2,
  DOWN = 3,
  RIGHT = 4
};

void get_dir(int *dir);

static inline void puts(const char *s)
{
  for (; *s; s++)
    putch(*s);
}

struct Position
{
  int x;
  int y;
};

struct Snake
{
  int size;
  int dir;
  struct Position pos[SNAKE_MAXSIZE];
};

/*

void snake_init(struct Snake *snake)
{
  snake->size = 1;
  snake->dir = RIGHT;
  snake->pos[0].x = 20;
  snake->pos[0].y = 20;
}

*/