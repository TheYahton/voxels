#include "window.h"
#include "logs.h"

#define RGFW_EXPORT
#include "../../RGFW/RGFW.h"


RGFW_window *win;
float mouse_dx, mouse_dy;
bool keys[256] = {0};

void *getProcAddress(const char *name) { return RGFW_getProcAddress(name); }

static void key_callback(RGFW_window*, u8, char, RGFW_keymod, RGFW_bool);
static void mouse_callback(RGFW_window*, RGFW_point, RGFW_point);
static void resize_callback(RGFW_window*, RGFW_rect);

u8 wkey2rgfw(char wkey) {
  unsigned char rgfw_key;
  switch (wkey) {
  case WKEY_UP:     rgfw_key = RGFW_up; break;
  case WKEY_DOWN:   rgfw_key = RGFW_down; break;
  case WKEY_LEFT:   rgfw_key = RGFW_left; break;
  case WKEY_RIGHT:  rgfw_key = RGFW_right; break;
  case WKEY_W:      rgfw_key = RGFW_w; break;
  case WKEY_A:      rgfw_key = RGFW_a; break;
  case WKEY_S:      rgfw_key = RGFW_s; break;
  case WKEY_D:      rgfw_key = RGFW_d; break;
  case WKEY_SPACE:  rgfw_key = RGFW_space; break;
  case WKEY_SHIFTL: rgfw_key = RGFW_shiftL; break;
  }
  return rgfw_key;
}

u8 rgfw2wkey(u8 rgfw_key) {
  unsigned char wkey;
  switch (rgfw_key) {
  case RGFW_up:     wkey = WKEY_UP; break;
  case RGFW_down:   wkey = WKEY_DOWN; break;
  case RGFW_left:   wkey = WKEY_LEFT; break;
  case RGFW_right:  wkey = WKEY_RIGHT; break;
  case RGFW_w:      wkey = WKEY_W; break;
  case RGFW_a:      wkey = WKEY_A; break;
  case RGFW_s:      wkey = WKEY_S; break;
  case RGFW_d:      wkey = WKEY_D; break;
  case RGFW_space:  wkey = WKEY_SPACE; break;
  case RGFW_shiftL: wkey = WKEY_SHIFTL; break;
  }
  return wkey;
}

bool window_create(const char *name, int width, int height) {
  RGFW_setGLHint(RGFW_glMajor, 3);
  RGFW_setGLHint(RGFW_glMinor, 3);
  RGFW_setGLHint(RGFW_glProfile, RGFW_glCore);
  win = RGFW_createWindow(name, (RGFW_rect){0, 0, width, height}, RGFW_windowCenter | RGFW_windowHideMouse);
  info("The window has been initialized. The cursor is disabled.");

  RGFW_window_mouseHold(win, RGFW_AREA(0, 0));

  RGFW_window_makeCurrent(win);
  info("The OpenGL context has been attached to the window.");

  RGFW_setKeyCallback(key_callback);
  info("The key_callback function handles keyboard input.");
  RGFW_setMousePosCallback(mouse_callback);
  info("The mouse_callback function handles mouse input.");
  RGFW_setWindowResizeCallback(resize_callback);
  info("The resize_callback function handles window size changes.");

  return 0;
}

bool window_shouldClose(void) {
  return RGFW_window_shouldClose(win);
}

void window_pollEvents(void) {
  RGFW_window_checkEvents(win, 0);
}

void window_swapBuffers(void) { RGFW_window_swapBuffers(win); }

bool window_isPressed(WKey wkey) {
  return RGFW_isPressed(win, wkey2rgfw(wkey));
}

double window_getTime(void) { return RGFW_getTimeNS() / 1000000000.0f; }

void window_getMouse(float *dx, float *dy) {
  *dx = mouse_dx, *dy = mouse_dy;
}

void window_mouseAbsorb(void) {
  mouse_dx = 0, mouse_dy = 0;
}

void window_getSize(int *width, int *height) {
  RGFW_area a = RGFW_getScreenSize();
  *width = a.w;
  *height = a.h;
}

void* window_getKeys(void) {
  return keys;
}

// Call this function only once per tick.
float window_clock(void) {
    static float last = 0;
    float now = window_getTime();
    float dt = now - last;
    last = now;
    return dt;
}

void window_close(void) {
  RGFW_window_close(win);
}

static void key_callback(RGFW_window *win, u8 key, char keyChar __attribute__((__unused__)), RGFW_keymod keyMod __attribute__((__unused__)), RGFW_bool pressed) {
  keys[rgfw2wkey(key)] = pressed;
  if (key == RGFW_q && pressed) {
    RGFW_window_setShouldClose(win);
  }
}

static void mouse_callback(RGFW_window *win __attribute__((__unused__)), RGFW_point point __attribute__((__unused__)), RGFW_point vector) {
  mouse_dx = vector.x;
  mouse_dy = vector.y;
}

static void resize_callback(RGFW_window *win __attribute__((__unused__)), RGFW_rect r) {
  glViewport(0, 0, r.w, r.h);
}
