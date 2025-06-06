#ifndef _MESH_H
#define _MESH_H

#include "chunk.h"

typedef struct {
  float position[3];
  uint32_t type;
  uint32_t normal;
} Vertex;

Vectorize(Vertex, Vertices);

typedef struct {
  Vertices vertices;
  UInt32Vector indices;
  bool visible;
  Vec3i position;
} Mesh;

Mesh chunk_genmesh(const struct Chunk *chunk);
void mesh_free(Mesh *mesh);

Vectorize(Mesh, MeshVector);

#endif /* MESH_H */
