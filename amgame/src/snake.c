#include <snake.h>
// #include <game.h>

#define MAX_X 40
#define MAX_Y 30

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
    if (snake->pos[0].x <= 1 || snake->pos[0].x >= MAX_X - 1 || snake->pos[0].y <= 0 || snake->pos[0].y >= MAX_Y - 1)
    {
        return true;
    }
    return false;
}

bool istouch_food(struct Snake *snake, struct Position *food)
{
    // 首先 food 的位置必须在 snake->pos[0] 的旁边
    if (abs(snake->pos[0].x - food->x) + abs(snake->pos[0].y - food->y) != 1)
    {
        return false;
    }
    // 左右
    if (abs(snake->pos[0].x - food->x) == 1)
    {
        if (snake->pos[0].x - food->x == 1 && snake->dir == LEFT)
        {
            return true;
        }
        else if (snake->dir == RIGHT)
        {
            return true;
        }
    }
    else if (abs(snake->pos[0].y - food->y) == 1)
    {
        if (snake->pos[0].y - food->y == 1 && snake->dir == UP)
        {
            return true;
        }
        else if (snake->dir == DOWN)
        {
            return true;
        }
    }
    return false;
}

void incr_sz(struct Snake *snake, struct Position *food)
{
    for (int i = snake->size - 1; i >= 0; i++)
    {
        snake->pos[i + 1].x = snake->pos[i].x;
        snake->pos[i + 1].y = snake->pos[i].y;
    }
    snake->size++;
    snake->pos[0].x = food->x;
    snake->pos[0].y = food->y;
}

void generate_newfood(struct Snake *snake, struct Position *food)
{
    while (1)
    {
        int x = rand() % MAX_X;
        int y = rand() % MAX_Y;
        int flag = 1;
        for (int i = 0; i < snake->size; i++)
        {
            if (x == snake->pos[i].x && y == snake->pos[i].y)
            {
                flag = 0;
                break;
            }
        }
        if (flag)
        {
            food->x = x;
            food->y = y;
            return;
        }
    }
}

void update_snake(struct Snake *snake, struct Position *food)
{
    if (collide(snake))
    {
        snake_init(snake);
        return;
    }

    if (istouch_food(snake, food))
    {
        // 重新生成一个 food 还要判断是否与snake冲突
        incr_sz(snake, food);
        generate_newfood(snake, food);
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