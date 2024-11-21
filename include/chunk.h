#ifndef _CHUNK_H
#define _CHUNK_H

#include "utils.h"

#define CHUNK_SIZE 8
#define CHUNK_CSIZE (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE)

struct Chunk {
	Vec3i position;		 // Vec3i is 3 * 4 bytes = 12 bytes
	unsigned char *data; // ptr is 8 bytes
}; // 20 bytes

struct Chunk chunk_init(int x, int y, int z);
void chunk_set(struct Chunk *chunk, unsigned int x, unsigned int y,
			   unsigned int z, char value);
unsigned char chunk_get(const struct Chunk *chunk, unsigned int x,
						unsigned int y, unsigned int z);
void chunk_free(struct Chunk *chunk);

#endif // _CHUNK_H
