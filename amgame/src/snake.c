#include <snake.h>
// #include <game.h>

#define MAX_X 40
#define MAX_Y 40

void snake_init(struct Snake *snake)
{
    snake->size = 3;
    snake->dir = UP;
    snake->pos[0].x = 20;
    snake->pos[0].y = 20;
    snake->pos[1].x = 19;
    snake->pos[1].y = 20;
    snake->pos[2].x = 18;
    snake->pos[2].y = 20;
}

bool collide(struct Snake *snake)
{
    if (snake->pos[0].x < 0 || snake->pos[0].x >= MAX_X || snake->pos[0].y < 0 || snake->pos[0].y >= MAX_Y)
    {
        return true;
    }
    return false;
}

void update_snake(struct Snake *snake)
{
    if (collide(snake))
    {
        snake_init(snake);
        return;
    }

    for (int i = snake->size - 1; i > 0; i--)
    {
        snake->pos[i].x = snake->pos[i - 1].x;
        snake->pos[i].y = snake->pos[i - 1].y;
    }

    switch (snake->dir)
    {
    case UP:
        snake->pos[0].y = snake->pos[0].y - 1;
        break;
    case LEFT:
        snake->pos[0].x = snake->pos[0].x - 1;
        break;
    case DOWN:
        snake->pos[0].y = snake->pos[0].y + 1;
        break;
    case RIGHT:
        snake->pos[0].x = snake->pos[0].x + 1;
        break;
    }
}