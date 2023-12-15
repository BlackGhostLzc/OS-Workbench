#include <am.h>
#include <amdev.h>
#include <klib.h>
#include <klib-macros.h>

void splash();
void print_key();

enum Diretion
{
  None = 0,
  UP = 1,
  LEFT = 2,
  DOWN = 3,
  RIGHT = 4
};

void get_dir(enum Diretion *dir);

static inline void puts(const char *s)
{
  for (; *s; s++)
    putch(*s);
}
