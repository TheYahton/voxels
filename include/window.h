#include <RGFW.h>

#include <stdbool.h>

#ifndef _WINDOW_H
#define _WINDOW_H
typedef struct {
  RGFW_window *window;
  int width, height;
  bool keys[1024];
  bool polygon_mode;
  float dx, dy;
} MyWindow;

void *getProcAddress(const char *name);
MyWindow createWindow(int width, int height);
int initWindow(MyWindow *window);
void getWindowSize(MyWindow *window, int *width, int *height);
void swapBuffer(MyWindow *window);
bool windowShouldClose(MyWindow *window);
void windowClose(MyWindow *window);
void windowPollEvents(MyWindow *window);
double getTime(void);
#endif // _WINDOW_H
