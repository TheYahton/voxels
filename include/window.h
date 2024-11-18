#include <GLFW/glfw3.h>

#include <stdbool.h>

#ifndef _WINDOW_H
#define _WINDOW_H
typedef struct {
	GLFWwindow *window;
	int width, height;
	bool keys[1024];
	bool polygon_mode;
} Window;

Window createWindow(int width, int height);
int initWindow(Window *window);
void getWindowSize(Window *window, int *width, int *height);
void swapBuffer(Window *window);
bool windowShouldClose(Window *window);
void getCursorPos(Window *window, double *xpos, double *ypos);
void windowClose(Window *window);
void windowPollEvents(Window *window);
double getTime(void);
#endif // _WINDOW_H
