#ifndef _WORLD_H
#define _WORLD_H

#include <pthread.h>
#include <stdbool.h>
#include "utils.h"

typedef struct Chunk Chunk;

Vectorize(Chunk*, ChunkVector)

struct Task {
  int x;
  int y;
  int z;
};

Vectorize(struct Task, Tasks);

struct World {
  ChunkVector chunks;
  Tasks tasks;
  pthread_t thread;
  pthread_mutex_t mutex;
  bool exit;
};

struct World *world_init(void);
uint8_t world_getVoxel(struct World *world, int x, int y, int z);
void world_setVoxel(struct World *world, int x, int y, int z, uint8_t value);
void world_getChunkCube(SizeVector *vec, struct World *world, Vec3 around,
                        int radius);
void world_free(struct World *world);

#endif // _WORLD_H
