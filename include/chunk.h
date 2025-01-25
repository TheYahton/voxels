#ifndef _CHUNK_H
#define _CHUNK_H

#include "utils.h"

typedef enum {
  Air = 0,
  Stone,
  Dirt,
  Grass,
  Sand,
  Water,
  TypeCount,
} VoxelType;

#define CHUNK_SIZE 32
#define CHUNK_CSIZE (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE)

struct Chunk {
  Vec3i position;    // Vec3i is 3 * 4 bytes = 12 bytes
  uint8_t *data;     // ptr is 8 bytes
  size_t mesh_index; // uint is 4 bytes
}; // 24 bytes

struct Chunk chunk_init(int x, int y, int z);
void chunk_set(struct Chunk *chunk, uint8_t x, uint8_t y, uint8_t z,
               char value);
uint8_t chunk_get(const struct Chunk *chunk, uint8_t x, uint8_t y, uint8_t z);
void chunk_free(struct Chunk *chunk);

#endif // _CHUNK_H
