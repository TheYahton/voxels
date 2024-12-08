#ifndef _MESH_H
#define _MESH_H

#include "chunk.h"
#include "utils.h"
#include "world.h"

#include <stdbool.h>
#include <GL/gl.h>

typedef struct {
	GLfloat position[3];  // 12 bytes
	GLfloat color[3];     // 12 bytes
	uint32_t normal;      // 4 bytes
} Vertex;  // 36 bytes

Vectorize(Vertex, Vertices)

typedef struct {
	// FloatVector vertices; // 20 bytes
	Vertices vertices;
	UInt32Vector indices; // 20 bytes
	bool visible;		  // 8 bytes
} Mesh;					  // 48 bytes

Mesh chunk_genmesh(const struct Chunk *chunk, const struct World *world);
void mesh_free(Mesh *mesh);

Vectorize(Mesh, MeshVector)

#endif // _MESH_H
