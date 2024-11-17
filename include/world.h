#ifndef _world
#define _world

#include "chunk.h"
#include "utils.h"

Vectorize(Chunk, ChunkVector)

	typedef struct {
	ChunkVector chunks;
} World;

World world_init(void);
void world_free(World *world);

#endif // _world
