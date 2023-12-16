#include <snake.h>
// #include <game.h>

void snake_init(struct Snake *snake)
{
    snake->size = 1;
    snake->dir = RIGHT;
    snake->pos[0].x = 20;
    snake->pos[0].y = 20;
}

void update_snake(struct Snake *snake)
{
    for (int i = snake->size - 1; i > 0; i--)
    {
        snake->pos[i].x = snake->pos[i - 1].x;
        snake->pos[i].y = snake->pos[i - 1].y;
    }

    switch (snake->dir)
    {
    case UP:
        snake->pos[0].y = snake->pos[0].y - 1;
    case LEFT:
        snake->pos[0].x = snake->pos[0].x - 1;
    case DOWN:
        snake->pos[0].y = snake->pos[0].y + 1;
    case RIGHT:
        snake->pos[0].x = snake->pos[0].x + 1;
    }
}