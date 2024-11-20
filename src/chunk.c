#include "chunk.h"

#include <stdlib.h>
#include <string.h>

struct Chunk chunk_init(int x, int y, int z) {
	struct Chunk chunk = {
		{x, y, z}, malloc(CHUNK_CSIZE * sizeof(char)),
		// {{NULL, 0, 0, 64}, {NULL, 0, 0, 64}},
		// 0,
	};
	memset(chunk.data, 0, CHUNK_CSIZE * sizeof(char));
	return chunk;
}

void chunk_set(struct Chunk *chunk, unsigned int x, unsigned int y,
			   unsigned int z, char value) {
	if (x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE) {
		return;
	}
	chunk->data[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] = value;
}

unsigned char chunk_get(const struct Chunk *chunk, unsigned int x,
						unsigned int y, unsigned int z) {
	if (chunk == NULL || x >= CHUNK_SIZE || y >= CHUNK_SIZE ||
		z >= CHUNK_SIZE) {
		return -1;
	}
	return chunk->data[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE];
}

void chunk_free(struct Chunk *chunk) { free(chunk->data); }
