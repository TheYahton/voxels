#ifndef _WORLD_H
#define _WORLD_H

#include "utils.h"

typedef struct Chunk Chunk;

Vectorize(Chunk, ChunkVector)

	struct World {
	ChunkVector chunks;
};

struct World world_init(void);
void world_chunk_generate(struct World *world, int x, int y, int z);
unsigned char world_block_get(const struct World *world, int x, int y, int z);
void world_block_set(struct World *world, int x, int y, int z,
					 unsigned int value);
UnsignedIntVector world_chunk_circle(const struct World *world, int x, int y,
									 int z, int radius);
void world_free(struct World *world);

#endif // _WORLD_H
