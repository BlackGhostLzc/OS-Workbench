#include <am.h>
#include <amdev.h>
#include <klib.h>
#include <klib-macros.h>

#define SNAKE_MAXSIZE 24

void splash();
void print_key();
void gpu_clear(int width, int height);

static inline void puts(const char *s)
{
  for (; *s; s++)
    putch(*s);
}
