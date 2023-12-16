#include <am.h>
#include <amdev.h>
#include <klib.h>
#include <klib-macros.h>
// #include <game.h>

enum Direction
{
    None = 0,
    UP = 1,
    LEFT = 2,
    DOWN = 3,
    RIGHT = 4
};

#define SNAKE_MAXSIZE 24

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

void snake_init(struct Snake *snake);
void draw_snake(struct Snake *snake);
void get_dir(struct Snake *snake);
void update_snake(struct Snake *snake);