#ifndef _chunk
#define _chunk

#include "mesh.h"
#include "utils.h"

#define CSIZE 32
#define CQSIZE (CSIZE * CSIZE * CSIZE)

typedef struct {
	Vec3i position;
	unsigned char *data;
	Mesh mesh;
	unsigned int VAO;
} Chunk;

Chunk chunk_init(int x, int y, int z);
void chunk_set(Chunk *chunk, unsigned int x, unsigned int y, unsigned int z,
			   char value); // void chunk set x, y and z (крутая рифма)
unsigned char chunk_get(const Chunk *chunk, unsigned int x, unsigned int y,
						unsigned int z);
void chunk_worldgen(Chunk *chunk);
void chunk_free(Chunk *chunk);

Mesh chunk_genmesh(const Chunk *chunk);
#endif // _chunk
