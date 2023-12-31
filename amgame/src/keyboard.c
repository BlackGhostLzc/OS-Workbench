#include <game.h>
#include <snake.h>

#define KEYNAME(key) \
  [AM_KEY_##key] = #key,
static const char *key_names[] = {
    AM_KEYS(KEYNAME)};

void print_key()
{
  AM_INPUT_KEYBRD_T event = {.keycode = AM_KEY_NONE};
  ioe_read(AM_INPUT_KEYBRD, &event);
  // puts("Key pressed: \n");
  if (event.keycode != AM_KEY_NONE && event.keydown)
  {
    if (event.keydown && event.keycode == AM_KEY_ESCAPE)
      halt(0);
    puts("Key pressed: ");
    puts(key_names[event.keycode]);
    puts("\n");
  }
}

void get_dir(struct Snake *snake)
{
  AM_INPUT_KEYBRD_T event = {.keycode = AM_KEY_NONE};
  ioe_read(AM_INPUT_KEYBRD, &event);
  // puts("Key pressed: \n");
  int prev_dir = snake->dir;
  if (event.keycode != AM_KEY_NONE && event.keydown)
  {
    if (event.keydown && event.keycode == AM_KEY_ESCAPE)
      halt(0);
    if (event.keydown && event.keycode == AM_KEY_W && prev_dir != DOWN)
    {
      snake->dir = UP;
    }
    else if (event.keydown && event.keycode == AM_KEY_A && prev_dir != RIGHT)
    {
      snake->dir = LEFT;
    }
    else if (event.keydown && event.keycode == AM_KEY_S && prev_dir != UP)
    {
      snake->dir = DOWN;
    }
    else if (event.keydown && event.keycode == AM_KEY_D && prev_dir != LEFT)
    {
      snake->dir = RIGHT;
    }
    puts("Key pressed: ");
    puts(key_names[event.keycode]);
    puts("\n");
  }
}
