#include "chunk.h"

#include <stdlib.h>
#include <string.h>

struct Chunk chunk_init(World *world, int x, int y, int z) {
	struct Chunk chunk = {
		world,
		{x, y, z},
		malloc(CQSIZE * sizeof(char)),
		//{{NULL, 0, 0, 64}, {NULL, 0, 0, 64}},
		NULL,
		0,
	};
	memset(chunk.data, 0, CQSIZE * sizeof(char));
	return chunk;
}

void chunk_set(struct Chunk *chunk, unsigned int x, unsigned int y,
			   unsigned int z, char value) {
	if (x >= CSIZE || y >= CSIZE || z >= CSIZE) {
		return;
	}
	chunk->data[x + y * CSIZE + z * CSIZE * CSIZE] = value;
}

unsigned char chunk_get(const struct Chunk *chunk, unsigned int x,
						unsigned int y, unsigned int z) {
	if (chunk == NULL || x >= CSIZE || y >= CSIZE || z >= CSIZE) {
		return -1;
	}
	return chunk->data[x + y * CSIZE + z * CSIZE * CSIZE];
}

void chunk_worldgen(struct Chunk *chunk) {
	for (int x = 0; x < CSIZE; x++) {
		for (int z = 0; z < CSIZE; z++) {
			chunk_set(chunk, x, 0, z, 1);
		}
	}
}

void chunk_free(struct Chunk *chunk) { free(chunk->data); }
