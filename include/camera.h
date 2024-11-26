#include "utils.h"
#include <stdbool.h>

#ifndef _CAMERA_H
#define _CAMERA_H
typedef struct {
	Vec3 *position;
	Vec3 *direction;
} Camera;

void camera_update(Camera *camera, bool keys[1024], float dx, float dy,
				   float dt);
#endif // _CAMERA_H
