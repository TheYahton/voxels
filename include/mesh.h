#ifndef _MESH_H
#define _MESH_H

#include "chunk.h"

typedef struct {
  float position[3];
  uint32_t type;
  uint32_t normal;
} Vertex;

struct Vertices {
  size_t size;
  size_t capacity;
  Vertex *data;
};

typedef struct {
  struct Vertices vertices;
  struct UInt32Array indices;
  bool visible;
  Vec3i position;
} Mesh;

struct MeshArray {
  size_t size;
  size_t capacity;
  Mesh *data;
};

Mesh chunk_genmesh(const struct Chunk *chunk);
void mesh_free(Mesh *mesh);

#endif /* MESH_H */
