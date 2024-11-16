#include "chunk.h"

#include <stdlib.h>
#include <string.h>

Chunk chunk_init(int x, int y, int z) {
	Chunk chunk = {
		{x, y, z},
		malloc(CQSIZE * sizeof(char)),
		//{{NULL, 0, 0, 64}, {NULL, 0, 0, 64}},
		NULL,
		0,
	};
	memset(chunk.data, 0, CQSIZE * sizeof(char));
	return chunk;
}

void chunk_set(Chunk *chunk, unsigned int x, unsigned int y, unsigned int z,
			   char value) {
	if (x >= CSIZE || y >= CSIZE || z >= CSIZE) {
		return;
	}
	chunk->data[x + y * CSIZE + z * CSIZE * CSIZE] = value;
}

unsigned char chunk_get(const Chunk *chunk, unsigned int x, unsigned int y,
						unsigned int z) {
	if (x >= CSIZE || y >= CSIZE || z >= CSIZE) {
		return -1;
	}
	return chunk->data[x + y * CSIZE + z * CSIZE * CSIZE];
}

void chunk_worldgen(Chunk *chunk) {
	for (int x = 0; x < CSIZE; x++) {
		for (int z = 0; z < CSIZE; z++) {
			chunk_set(chunk, x, 0, z, 1);
		}
	}
}

void chunk_free(Chunk *chunk) { free(chunk->data); }
