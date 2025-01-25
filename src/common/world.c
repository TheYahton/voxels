#define _VECTOR_IMPL
#include "world.h"
#include "chunk.h"
#include "utils.h"

#include <math.h>

VectorImpl(Chunk, ChunkVector)

    static int mod(int a, int b) {
  int r = a % b;
  return r < 0 ? r + b : r;
}

struct World world_init(void) {
  return (struct World){ChunkVector_init(64, 64)};
}

static size_t world_generateChunk(struct World *world, int cx, int cy, int cz) {
  ChunkVector_append(&world->chunks, chunk_init(cx, cy, cz));
  size_t index = world->chunks.size - 1;
  Chunk *chunk = &world->chunks.data[index];
  VoxelType voxel = mod(cx, 5) + 1;

  for (size_t i = 0; i < CHUNK_CSIZE; i++) {
    int z = i / (CHUNK_SIZE * CHUNK_SIZE);
    int ci = i - (z * CHUNK_SIZE * CHUNK_SIZE);
    int y = ci / CHUNK_SIZE;
    int x = ci % CHUNK_SIZE;

    int dx = x - CHUNK_SIZE / 2;
    int dy = y - CHUNK_SIZE / 2;
    int dz = z - CHUNK_SIZE / 2;
    int squared_dist = dx * dx + dy * dy + dz * dz;
    if (squared_dist < CHUNK_SIZE * CHUNK_SIZE / 16) {
      chunk->data[i] = voxel;
    }
  }

  return index;
}

// Get index in world.chunks by chunk's coordinates
// WARNING: This function is very slow. Use it as little as possible.
static size_t world_getChunk(const struct World *world, int x, int y, int z) {
  for (size_t i = 0; i < world->chunks.size; i++) {
    Chunk *chunk = &world->chunks.data[i];
    if (chunk->position.x == x && chunk->position.y == y &&
        chunk->position.z == z) {
      return i;
    }
  }
  return -1;
}

// Get index in world.chunks by chunk's coordinates or generate if it's not
// found
static size_t world_getOrGenerateChunk(struct World *world, int x, int y,
                                       int z) {
  size_t chunk_index = world_getChunk(world, x, y, z);
  if (chunk_index != SIZE_MAX)
    return chunk_index;
  return world_generateChunk(world, x, y, z);
}

// Get voxel type by coordinates in the world.
// WARNING: This function is very slow because it calls `world_getChunk()`
uint8_t world_getVoxel(const struct World *world, int x, int y, int z) {
  uint8_t voxelX = mod(x, CHUNK_SIZE);
  uint8_t voxelY = mod(y, CHUNK_SIZE);
  uint8_t voxelZ = mod(z, CHUNK_SIZE);
  int chunkX = floorf((float)x / CHUNK_SIZE);
  int chunkY = floorf((float)y / CHUNK_SIZE);
  int chunkZ = floorf((float)z / CHUNK_SIZE);
  size_t chunk_index = world_getChunk(world, chunkX, chunkY, chunkZ);
  Chunk *chunk = NULL;
  if (chunk_index != SIZE_MAX) {
    chunk = &world->chunks.data[chunk_index];
  }
  return chunk_get(chunk, voxelX, voxelY, voxelZ);
}

// Set voxel type by coordinates in the world.
// WARNING: This function is very slow because it calls `world_getChunk()`
void world_setVoxel(struct World *world, int x, int y, int z, uint8_t value) {
  uint8_t voxelX = mod(x, CHUNK_SIZE);
  uint8_t voxelY = mod(y, CHUNK_SIZE);
  uint8_t voxelZ = mod(z, CHUNK_SIZE);
  int chunkX = floorf((float)x / CHUNK_SIZE);
  int chunkY = floorf((float)y / CHUNK_SIZE);
  int chunkZ = floorf((float)z / CHUNK_SIZE);
  size_t chunk_index = world_getChunk(world, chunkX, chunkY, chunkZ);
  Chunk *chunk = NULL;
  if (chunk_index != SIZE_MAX) {
    chunk = &world->chunks.data[chunk_index];
  }
  chunk_set(chunk, voxelX, voxelY, voxelZ, value);
}

// Adds chunks around given position to vector
// If needed chunks are not generated then it'll generate it.
// WARNING: This function is very slow because it calls `world_getChunk()`
void world_getChunkCube(SizeVector *vec, struct World *world, Vec3 around,
                        int radius) {
  int gen = 8; // max chunk generation per function call

  int chunkX = floorf((-around.x + CHUNK_SIZE / 2.0f) / CHUNK_SIZE);
  int chunkY = floorf((around.y + CHUNK_SIZE / 2.0f) / CHUNK_SIZE);
  int chunkZ = floorf((-around.z + CHUNK_SIZE / 2.0f) / CHUNK_SIZE);
  for (int x = -radius; x < radius; x++) {
    for (int y = -radius; y < radius; y++) {
      for (int z = -radius; z < radius; z++) {
        size_t chunk_index =
            world_getChunk(world, chunkX + x, chunkY + y, chunkZ + z);
        if (chunk_index != SIZE_MAX) {
          SizeVector_append(vec, chunk_index);
        } else if (gen > 0) {
          SizeVector_append(vec, world_generateChunk(world, chunkX + x,
                                                     chunkY + y, chunkZ + z));
          gen--;
        }
      }
    }
  }
}

void world_free(struct World *world) {
  for (size_t i = 0; i < world->chunks.size; i++)
    chunk_free(&world->chunks.data[i]);
  free(world->chunks.data);
}
