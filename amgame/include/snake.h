#include <am.h>
#include <amdev.h>
#include <klib.h>
#include <klib-macros.h>
#include <game.h>

#define SNAKE_MAXSIZE 24

struct Position
{
};

struct Snake
{
    int size;
    enum Direcion dir;
    Position pos[SNAKE_MAXSIZE];
};