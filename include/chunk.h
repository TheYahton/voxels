#ifndef _chunk
#define _chunk

#include "mesh.h"
#include "utils.h"

#define CSIZE 32
#define CQSIZE (CSIZE * CSIZE * CSIZE)

typedef struct World World;

struct Chunk {
	World *world;
	Vec3i position;
	unsigned char *data;
	Mesh mesh;
	unsigned int VAO;
};

struct Chunk chunk_init(World *world, int x, int y, int z);
void chunk_set(struct Chunk *chunk, unsigned int x, unsigned int y,
			   unsigned int z,
			   char value); // void chunk set x, y and z (крутая рифма)
unsigned char chunk_get(const struct Chunk *chunk, unsigned int x,
						unsigned int y, unsigned int z);
void chunk_worldgen(struct Chunk *chunk);
void chunk_free(struct Chunk *chunk);

Mesh chunk_genmesh(const struct Chunk *chunk);
#endif // _chunk
