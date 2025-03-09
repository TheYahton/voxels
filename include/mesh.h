#ifndef _MESH_H
#define _MESH_H

#include "chunk.h"

#include <stdbool.h>

typedef struct {
  float position[3]; // 12 bytes
  uint32_t type;     // 4 bytes
  uint32_t normal;   // 4 bytes
} Vertex;            // 20 bytes

Vectorize(Vertex, Vertices);

typedef struct {
  Vertices vertices;    // 20 bytes
  UInt32Vector indices; // 20 bytes
  bool visible;         // 8 bytes
  Vec3i position;       // 12 bytes
} Mesh;                 // 60 bytes

Mesh chunk_genmesh(const struct Chunk *chunk);
void mesh_free(Mesh *mesh);

Vectorize(Mesh, MeshVector);

#endif // _MESH_H
