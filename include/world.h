#ifndef _WORLD_H
#define _WORLD_H

#include "utils.h"

typedef struct Chunk Chunk;

Vectorize(Chunk, ChunkVector)

    struct World {
  ChunkVector chunks;
};

struct World world_init(void);
uint8_t world_getVoxel(const struct World *world, int x, int y, int z);
void world_setVoxel(struct World *world, int x, int y, int z, uint8_t value);
void world_getChunkCube(SizeVector *vec, struct World *world, Vec3 around,
                        int radius);
void world_free(struct World *world);

#endif // _WORLD_H
