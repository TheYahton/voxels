#ifndef WORLD_H
#define WORLD_H

#include "utils.h"

typedef struct Chunk Chunk;

struct ChunkArray {
  size_t size;
  size_t capacity;
  Chunk *data;
};

struct Task {
  int x;
  int y;
  int z;
};

struct TaskArray {
  size_t size;
  size_t capacity;
  struct Task *data;
};

struct World {
  struct ChunkArray chunks;
  struct TaskArray tasks;
  pthread_t thread;
  pthread_mutex_t mutex;
  bool exit;
};

struct World *world_init(void);
uint8_t world_getVoxel(struct World *world, int x, int y, int z);
void world_setVoxel(struct World *world, int x, int y, int z, uint8_t value);
void world_getChunkCube(struct SizeArray *vec, struct World *world, Vec3 around,
                        int radius);
void world_free(struct World *world);

#endif /* WORLD_H */
