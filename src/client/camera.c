#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923 /* pi/2 */
#endif

#include "camera.h"
#include "window.h"

void camera_update(Camera *camera, float dt) {
  float dx, dy;
  window_getMouse(&dx, &dy);
  bool *keys = window_getKeys();

  // Update camera rotation
  {
    int up = keys[WKEY_UP] - keys[WKEY_DOWN];
    int left = keys[WKEY_LEFT] - keys[WKEY_RIGHT];

    camera->direction->y += up * dt;
    camera->direction->x += left * dt;
    camera->direction->y += dy * dt * 0.02f;
    camera->direction->x += dx * dt * 0.02f;

    camera->direction->y = fmax(fmin(camera->direction->y, M_PI_2), -M_PI_2);
  }

  // Update camera position
  {
    int forward = keys[WKEY_W] - keys[WKEY_S];
    int left = keys[WKEY_A] - keys[WKEY_D];
    int up = keys[WKEY_SPACE] - keys[WKEY_SHIFTL];

    camera->position->z += (forward * cos(camera->direction->x) -
                            left * sin(camera->direction->x)) *
                           *camera->speed * dt;
    camera->position->x += (left * cos(camera->direction->x) +
                            forward * sin(camera->direction->x)) *
                           *camera->speed * dt;
    camera->position->y += up * *camera->speed * dt;
  }
}
