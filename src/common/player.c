#include <math.h>
#include <stdbool.h>

#include "player.h"

void player_move(Player *player, bool keys[6], float dt) {
	// keys is W A S D SPACE SHIFT
	//         0 1 2 3 4     5
	int forward = keys[0] - keys[2];
	int left = keys[1] - keys[3];
	int up = keys[4] - keys[5];

	player->position.z +=
		(forward * cos(player->direction.x) - left * sin(player->direction.x)) *
		player->speed * dt;
	player->position.x +=
		(left * cos(player->direction.x) + forward * sin(player->direction.x)) *
		player->speed * dt;
	player->position.y += up * player->speed * dt;
}
