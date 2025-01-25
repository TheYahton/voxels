#define RGFW_EXPORT
#define RGFW_IMPLEMENTATION

#include "window.h"
#include "logs.h"
#include "shader.h"

void *getProcAddress(const char *name) { return RGFW_getProcAddress(name); }

void windowClose(MyWindow *window __attribute__((unused))) {
  RGFW_window_close(window->window);
}

void windowPollEvents(MyWindow *window __attribute__((unused))) {
  RGFW_window_checkEvents(window->window, 0);
}

double getTime(void) { return RGFW_getTimeNS() / 1000000000.0f; }

void swapBuffer(MyWindow *window) { RGFW_window_swapBuffers(window->window); }

bool windowShouldClose(MyWindow *window) {
  return RGFW_window_shouldClose(window->window);
}

static void key_callback(RGFW_window *win, u32 keycode,
                         char keyName[16] __attribute__((__unused__)),
                         u8 lockState __attribute__((__unused__)), b8 pressed) {
  MyWindow *my_window = win->userPtr;

  if (keycode < 1024) {
    if (pressed) {
      my_window->keys[keycode] = true;
    } else if (!pressed) {
      my_window->keys[keycode] = false;
    }
  }

  if (keycode == RGFW_q && pressed) {
    RGFW_window_setShouldClose(win);
  } else if (keycode == RGFW_p && pressed) {
    my_window->polygon_mode = !my_window->polygon_mode;
  }
}

static void resize_callback(RGFW_window *win, RGFW_rect r) {
  MyWindow *my_window = win->userPtr;
  my_window->width = r.w;
  my_window->height = r.h;
  glViewport(0, 0, r.w, r.h);
}

MyWindow createWindow(int width, int height) {
  return (MyWindow){NULL, width, height, {false}, false};
}

int initWindow(MyWindow *window) {
  RGFW_setGLVersion(RGFW_GL_CORE, 3, 3);
  window->window = RGFW_createWindow("Voxels", (RGFW_rect){0, 0, 800, 600},
                                     RGFW_CENTER | RGFW_HIDE_MOUSE);
  info("The window has been initialized.");

  window->window->userPtr = window;

  RGFW_area size = RGFW_getScreenSize();
  window->width = size.w;
  window->height = size.h;

  // Мы уже скрыли мышь на этапе создания окна с помощью флага RGFW_HIDE_MOUSE
  info("The cursor has been disabled.");

  // TODO: return raw mouse motion back?
  // if (glfwRawMouseMotionSupported()) {
  // 	glfwSetInputMode(window->window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
  // 	info("Raw mouse motion mode enabled.");
  // } else {
  // 	warning("The system does not support raw mouse motion mode.");
  // }

  RGFW_window_mouseHold(window->window, RGFW_AREA(window->window->r.w / 2,
                                                  window->window->r.h / 2));

  RGFW_window_makeCurrent(window->window);
  info("The OpenGL context has been attached to the window.");

  RGFW_setKeyCallback(key_callback);
  info("The key_callback function handles user input.");

  RGFW_setWindowResizeCallback(resize_callback);
  info("The resize_callback function handles window size changes.");

  return 0;
}
