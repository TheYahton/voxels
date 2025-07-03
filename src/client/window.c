#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>

#include "logging.h"
#include "utils.h"
#include "window.h"

#define RGFW_EXPORT
#include "../../RGFW/RGFW.h"

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923 /* pi/2 */
#endif

#define MOUSE_SENSITIVITY 0.005
 
RGFW_window *win;
float yaw = 0.0, pitch = 0.0;
bool keys[256] = {0};
void *userKeyCallback;

void *getProcAddress(const char *name) { return RGFW_getProcAddress(name); }

// static void key_callback(RGFW_window*, u8, char, RGFW_keymod, RGFW_bool);
static void key_callback(RGFW_window* win, u8 key, u8 keyChar, RGFW_keymod keyMod, RGFW_bool pressed);
static void mouse_callback(RGFW_window*, RGFW_point, RGFW_point);
static void resize_callback(RGFW_window*, RGFW_rect);

u8 wkey2rgfw(char wkey) {
  unsigned char rgfw_key = 0;
  switch (wkey) {
  case WKEY_UP:     rgfw_key = RGFW_up; break;
  case WKEY_DOWN:   rgfw_key = RGFW_down; break;
  case WKEY_LEFT:   rgfw_key = RGFW_left; break;
  case WKEY_RIGHT:  rgfw_key = RGFW_right; break;
  case WKEY_W:      rgfw_key = RGFW_w; break;
  case WKEY_A:      rgfw_key = RGFW_a; break;
  case WKEY_S:      rgfw_key = RGFW_s; break;
  case WKEY_D:      rgfw_key = RGFW_d; break;
  case WKEY_P:      rgfw_key = RGFW_p; break;
  case WKEY_SPACE:  rgfw_key = RGFW_space; break;
  case WKEY_SHIFTL: rgfw_key = RGFW_shiftL; break;
  }
  return rgfw_key;
}

u8 rgfw2wkey(u8 rgfw_key) {
  unsigned char wkey = 0;
  switch (rgfw_key) {
  case RGFW_up:     wkey = WKEY_UP; break;
  case RGFW_down:   wkey = WKEY_DOWN; break;
  case RGFW_left:   wkey = WKEY_LEFT; break;
  case RGFW_right:  wkey = WKEY_RIGHT; break;
  case RGFW_w:      wkey = WKEY_W; break;
  case RGFW_a:      wkey = WKEY_A; break;
  case RGFW_s:      wkey = WKEY_S; break;
  case RGFW_d:      wkey = WKEY_D; break;
  case RGFW_p:      wkey = WKEY_P; break;
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

  RGFW_window_mouseHold(win, RGFW_AREA(0, 0));

  RGFW_window_makeCurrent(win);

  RGFW_setKeyCallback(key_callback);
  RGFW_setMousePosCallback(mouse_callback);
  RGFW_setWindowResizedCallback(resize_callback);

  logging_log(LL_DEBUG, "The window has been fully initialized");

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

bool window_wasPressed(WKey wkey) {
  return RGFW_wasPressed(win, wkey2rgfw(wkey));
}

double window_getTime(void) { return RGFW_getTimeNS() / 1000000000.0f; }

void window_getYawPitch(float *yaw_p, float *pitch_p) {
  pitch = fmax(fmin(pitch, M_PI_2), -M_PI_2);
  *yaw_p = yaw;
  *pitch_p = pitch;
}

void window_getSize(int *width, int *height) {
  RGFW_area a = RGFW_getScreenSize();
  *width = a.w;
  *height = a.h;
}

void* window_getKeys(void) {
  return keys;
}

// Call this function only once per frame.
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

static void key_callback(RGFW_window* win, u8 key, u8 keyChar, RGFW_keymod keyMod, RGFW_bool pressed) {
  (void) keyChar;
  (void) keyMod;
  keys[rgfw2wkey(key)] = pressed;
  if (key == RGFW_q && pressed) {
    RGFW_window_setShouldClose(win, true);
  }
}

static void mouse_callback(RGFW_window *win, RGFW_point point, RGFW_point vector) {
  (void) win;
  (void) point;
  yaw   += vector.x * MOUSE_SENSITIVITY;
  pitch += vector.y * MOUSE_SENSITIVITY;
}

static void resize_callback(RGFW_window *win, RGFW_rect r) {
  (void) win;
  glViewport(0, 0, r.w, r.h);
}
