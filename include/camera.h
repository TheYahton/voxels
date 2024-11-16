#include "utils.h"

#ifndef _camera
#define _camera
typedef struct {
	Vec3 direction;
} Camera;

void camera_update(Camera *camera, bool keys[1024], float dx, float dy,
				   float dt);
#endif // _camera
