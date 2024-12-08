#ifndef _WORLD_H
#define _WORLD_H

#include "utils.h"

typedef struct Chunk Chunk;

Vectorize(Chunk, ChunkVector)

struct World {
	ChunkVector chunks;
};

struct World world_init(void);
uint8_t world_block_get(const struct World *world, int x, int y, int z);
void world_block_set(struct World *world, int x, int y, int z, uint8_t value);
void world_chunk_cube(SizeVector *vec, struct World *world, Vec3 around, int radius);
void world_free(struct World *world);

#endif // _WORLD_H
