#ifndef _world
#define _world

#include "utils.h"

typedef struct Chunk Chunk;

Vectorize(Chunk, ChunkVector)

	struct World {
	ChunkVector chunks;
};

struct World world_init(void);
void world_chunk_generate(struct World *world, int x, int y, int z);
void world_mesh_generate(struct World *world, unsigned int index);
unsigned char world_block_get(struct World *world, int x, int y, int z);
void world_block_set(struct World *world, int x, int y, int z,
					 unsigned int value);
void world_free(struct World *world);

#endif // _world
