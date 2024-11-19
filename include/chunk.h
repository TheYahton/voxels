#ifndef _CHUNK_H
#define _CHUNK_H

#include "mesh.h"
#include "utils.h"

#define CHUNK_SIZE 32
#define CHUNK_CSIZE (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE)

typedef struct World World;

struct Chunk {
	World *world;		 // ptr is 8 bytes
	Vec3i position;		 // Vec3i is 3 * 4 bytes = 12 bytes
	unsigned char *data; // ptr is 8 bytes
	Mesh mesh;			 // 40 bytes
	unsigned int VAO;	 // 4 bytes
}; // 72 bytes

struct Chunk chunk_init(World *world, int x, int y, int z);
void chunk_set(struct Chunk *chunk, unsigned int x, unsigned int y,
			   unsigned int z, char value);
unsigned char chunk_get(const struct Chunk *chunk, unsigned int x,
						unsigned int y, unsigned int z);
void chunk_free(struct Chunk *chunk);

Mesh chunk_genmesh(const struct Chunk *chunk);
#endif // _CHUNK_H
