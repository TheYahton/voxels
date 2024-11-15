#include "utils.h"

typedef struct {
  Vec3 position;
  Vec3 velocity;
  Vec3 acceleration;
  float speed;
} Player;

void player_move(Player *player, bool keys[1024], Vec3 cameraDirection);