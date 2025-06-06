#ifndef CHUNK_H
#define CHUNK_H

#include "utils.h"

typedef enum {
  Air = 0,
  Stone,
  Dirt,
  Grass,
  Sand,
  Water,
  TypeCount
} VoxelType;

#define CHUNK_SIZE 32
#define CHUNK_CSIZE (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE)

struct Chunk {
  Vec3i position;
  size_t mesh_index;
  uint8_t data[CHUNK_CSIZE];
};

struct Chunk chunk_init(int x, int y, int z);
void chunk_set(struct Chunk *chunk, uint8_t x, uint8_t y, uint8_t z,
               char value);
uint8_t chunk_get(const struct Chunk *chunk, uint8_t x, uint8_t y, uint8_t z);
void chunk_free(struct Chunk *chunk);

#endif /* CHUNK_H */
