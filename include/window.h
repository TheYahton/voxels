#ifndef WINDOW_H
#define WINDOW_H

typedef char WKey; enum {
  WKEY_NULL,
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
void *window_getKeys(void);
void window_getSize(int *width, int *height);
void window_getMouse(float *dx, float *dy);
void window_mouseAbsorb(void);
double window_getTime(void);
bool window_isPressed(WKey);
void window_pollEvents(void);
void window_swapBuffers(void);
bool window_shouldClose(void);

#endif /* WINDOW_H */
