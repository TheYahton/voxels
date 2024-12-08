#include <GLFW/glfw3.h>

#include "camera.h"
#include <math.h>

#define M_PI_2 1.57079632679489661923 /* pi/2 */

void camera_update(Camera *camera, bool keys[1024], float dx, float dy,
				   float dt) {
	// Update camera rotation
	{
		int up = keys[GLFW_KEY_UP] - keys[GLFW_KEY_DOWN];
		int left = keys[GLFW_KEY_LEFT] - keys[GLFW_KEY_RIGHT];

		camera->direction->y += up * dt;
		camera->direction->x += left * dt;
		camera->direction->y += -dy * dt * 0.1f;
		camera->direction->x += -dx * dt * 0.1f;

		camera->direction->y = fmax(fmin(camera->direction->y, M_PI_2), -M_PI_2);
	}

	// Update camera position
	{
		int forward = keys[GLFW_KEY_W] - keys[GLFW_KEY_S];
		int left = keys[GLFW_KEY_A] - keys[GLFW_KEY_D];
		int up = keys[GLFW_KEY_SPACE] - keys[GLFW_KEY_LEFT_SHIFT];

		camera->position->z +=
			(forward * cos(camera->direction->x) - left * sin(camera->direction->x)) *
			*camera->speed * dt;
		camera->position->x +=
			(left * cos(camera->direction->x) + forward * sin(camera->direction->x)) *
			*camera->speed * dt;
		camera->position->y += up * *camera->speed * dt;
	}
}
