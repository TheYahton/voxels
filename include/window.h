#ifndef _WINDOW_H
#define _WINDOW_H

#include <stdbool.h>

enum Key {
  WKEY_UP,
  WKEY_DOWN,
  WKEY_RIGHT,
  WKEY_LEFT,
  WKEY_W,
  WKEY_A,
  WKEY_S,
  WKEY_D,
  WKEY_SPACE,
  WKEY_SHIFTL,
};

void *getProcAddress(const char *name);
bool window_create(const char *name, int width, int height);
void window_close(void);
float window_clock(void);
void window_getSize(int *width, int *height);
void window_getMouse(float *dx, float *dy);
void window_mouseAbsorb(void);
double window_getTime(void);
bool window_isPressed(enum Key);
void window_pollEvents(void);
void window_swapBuffers(void);
bool window_shouldClose(void);

#endif // _WINDOW_H
