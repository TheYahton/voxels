#include "utils.h"
#include <stdbool.h>

#ifndef _CAMERA_H
#define _CAMERA_H
typedef struct {
  Vec3 *position;
  Vec3 *direction;
  float *speed;
} Camera;

void camera_update(Camera *camera, float dt);
#endif // _CAMERA_H
