#include "utils.h"

#ifndef CAMERA_H
#define CAMERA_H
typedef struct {
  Vec3 *position;
  Vec3 *direction;
  float *speed;
  bool polygon_mode;
} Camera;

void camera_update(Camera *camera, float dt);
#endif /* CAMERA_H */
