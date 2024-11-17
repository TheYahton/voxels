#include <GLFW/glfw3.h>

#include <math.h>
#include <stdbool.h>

#include "player.h"

void player_move(Player *player, bool keys[1024], float dt) {
	int forward = keys[GLFW_KEY_W] - keys[GLFW_KEY_S];
	int left = keys[GLFW_KEY_A] - keys[GLFW_KEY_D];
	int up = keys[GLFW_KEY_SPACE] - keys[GLFW_KEY_LEFT_SHIFT];

	player->position.z +=
		(forward * cos(player->direction.x) - left * sin(player->direction.x)) *
		player->speed * dt;
	player->position.x +=
		(left * cos(player->direction.x) + forward * sin(player->direction.x)) *
		player->speed * dt;
	player->position.y += up * player->speed * dt;
}
