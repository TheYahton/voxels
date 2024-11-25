#include "chunk.h"

#include <stdlib.h>
#include <string.h>

struct Chunk chunk_init(int x, int y, int z) {
	struct Chunk chunk = {
		.position = {x, y, z},
		.data = (uint8_t *)malloc(CHUNK_CSIZE * sizeof(char)),
		.mesh_index = -1,
	};
	memset(chunk.data, 0, CHUNK_CSIZE * sizeof(char));
	return chunk;
}

void chunk_set(struct Chunk *chunk, uint8_t x, uint8_t y, uint8_t z,
			   char value) {
	if (x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE) {
		return;
	}
	chunk->data[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] = value;
}

uint8_t chunk_get(const struct Chunk *chunk, uint8_t x, uint8_t y, uint8_t z) {
	if (chunk == NULL || x >= CHUNK_SIZE || y >= CHUNK_SIZE ||
		z >= CHUNK_SIZE) {
		return -1;
	}
	return chunk->data[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE];
}

void chunk_free(struct Chunk *chunk) { free(chunk->data); }
