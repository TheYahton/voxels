#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>

#include "camera.h"
#include "window.h"

void camera_update(Camera *camera, float dt) {
  float yaw, pitch;
  window_getYawPitch(&yaw, &pitch);
  bool *keys = window_getKeys();

  // Update camera rotation
  camera->direction->x = -yaw;
  camera->direction->y = -pitch;

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

  // Wireframe
  if (window_wasPressed(WKEY_P)) camera->polygon_mode = !camera->polygon_mode;
}
