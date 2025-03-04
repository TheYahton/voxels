#include <math.h>

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923 /* pi/2 */
#endif

#include "camera.h"
#include "window.h"

void camera_update(Camera *camera, float dt) {
  float dx, dy;
  window_getMouse(&dx, &dy);
  // Update camera rotation
  {
    int up = window_isPressed(WKEY_UP) - window_isPressed(WKEY_DOWN);
    int left = window_isPressed(WKEY_LEFT) - window_isPressed(WKEY_RIGHT);

    camera->direction->y += up * dt;
    camera->direction->x += left * dt;
    camera->direction->y += dy * dt * 0.02f;
    camera->direction->x += dx * dt * 0.02f;

    camera->direction->y = fmax(fmin(camera->direction->y, M_PI_2), -M_PI_2);
  }

  // Update camera position
  {
    int forward = window_isPressed(WKEY_W) - window_isPressed(WKEY_S);
    int left = window_isPressed(WKEY_A) - window_isPressed(WKEY_D);
    int up = window_isPressed(WKEY_SPACE) - window_isPressed(WKEY_SHIFTL);

    camera->position->z += (forward * cos(camera->direction->x) -
                            left * sin(camera->direction->x)) *
                           *camera->speed * dt;
    camera->position->x += (left * cos(camera->direction->x) +
                            forward * sin(camera->direction->x)) *
                           *camera->speed * dt;
    camera->position->y += up * *camera->speed * dt;
  }
}
