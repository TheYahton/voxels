#include <GLFW/glfw3.h>

#include "camera.h"
#include <math.h>
#include <stdbool.h>

#define M_PI_2 1.57079632679489661923 /* pi/2 */

void camera_update(Camera *camera, bool keys[1024], float dx, float dy,
				   float dt) {
	int up = keys[GLFW_KEY_UP] - keys[GLFW_KEY_DOWN];
	int left = keys[GLFW_KEY_LEFT] - keys[GLFW_KEY_RIGHT];

	camera->direction.y += up * dt;
	camera->direction.x += left * dt;
	camera->direction.y += -dy * dt * 0.1f;
	camera->direction.x += -dx * dt * 0.1f;

	camera->direction.y = fmax(fmin(camera->direction.y, M_PI_2), -M_PI_2);
}
