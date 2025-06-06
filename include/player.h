#include "utils.h"

#ifndef PLAYER_H
#define PLAYER_H
typedef struct {
  Vec3 position;
  Vec3 velocity;
  Vec3 acceleration;
  float speed;
  Vec3 direction;
} Player;

void player_move(Player *player, bool keys[6], float dt);
#endif /* PLAYER_H */
