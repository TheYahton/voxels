#define RGFW_EXPORT
#include <RGFW.h>
#include <math.h>

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923 /* pi/2 */
#endif

#include "camera.h"

void camera_update(Camera *camera, bool keys[1024], float dx, float dy,
                   float dt) {
  // Update camera rotation
  {
    int up = keys[RGFW_up] - keys[RGFW_down];
    int left = keys[RGFW_left] - keys[RGFW_right];

    camera->direction->y += up * dt;
    camera->direction->x += left * dt;
    camera->direction->y += dy * dt * 0.02f;
    camera->direction->x += dx * dt * 0.02f;

    camera->direction->y = fmax(fmin(camera->direction->y, M_PI_2), -M_PI_2);
  }

  // Update camera position
  {
    int forward = keys[RGFW_w] - keys[RGFW_s];
    int left = keys[RGFW_a] - keys[RGFW_d];
    int up = keys[RGFW_space] - keys[RGFW_shiftL];

    camera->position->z += (forward * cos(camera->direction->x) -
                            left * sin(camera->direction->x)) *
                           *camera->speed * dt;
    camera->position->x += (left * cos(camera->direction->x) +
                            forward * sin(camera->direction->x)) *
                           *camera->speed * dt;
    camera->position->y += up * *camera->speed * dt;
  }
}
