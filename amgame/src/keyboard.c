#include <game.h>

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

void get_dir(int *dir)
{
  AM_INPUT_KEYBRD_T event = {.keycode = AM_KEY_NONE};
  ioe_read(AM_INPUT_KEYBRD, &event);
  // puts("Key pressed: \n");
  if (event.keycode != AM_KEY_NONE && event.keydown)
  {
    if (event.keydown && event.keycode == AM_KEY_ESCAPE)
      halt(0);
    if (event.keydown && event.keycode == AM_KEY_W)
    {
      *dir = UP;
    }
    else if (event.keydown && event.keycode == AM_KEY_A)
    {
      *dir = LEFT;
    }
    else if (event.keydown && event.keycode == AM_KEY_S)
    {
      *dir = DOWN;
    }
    else if (event.keydown && event.keycode == AM_KEY_D)
    {
      *dir = RIGHT;
    }
    puts("Key pressed: ");
    puts(key_names[event.keycode]);
    puts("\n");
  }
}
