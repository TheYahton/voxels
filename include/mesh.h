#ifndef _MESH_H
#define _MESH_H

#include "chunk.h"
#include "utils.h"
#include "world.h"

typedef struct {
	FloatVector vertices;	   // 20 bytes
	UnsignedIntVector indices; // 20 bytes
	bool visible;              // 8 bytes
} Mesh;						   // 48 bytes

Mesh chunk_genmesh(const struct Chunk *chunk, const struct World *world);
void mesh_free(Mesh *mesh);

Vectorize(Mesh, MeshVector)

#endif // _MESH_H
