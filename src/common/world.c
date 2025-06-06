#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#define __USE_MISC
#include <unistd.h>
#include <math.h>

#include "world.h"
#include "chunk.h"
#include "utils.h"

static int mod(int a, int b) {
  int r = a % b;
  return r < 0 ? r + b : r;
}

static Chunk generateChunk(int cx, int cy, int cz);

struct World world = {0};

void *test(void *ptr) {
  (void) ptr;
  while (!world.exit) {
    pthread_mutex_lock(&world.mutex);
    while (world.tasks.size > 0) {
      struct Task task = world.tasks.data[world.tasks.size-1];
      world.tasks.size--;
      pthread_mutex_unlock(&world.mutex);
      Chunk chunk = generateChunk(task.x, task.y, task.z);
      pthread_mutex_lock(&world.mutex);
      DArray_push(&world.chunks, chunk);
    }
    pthread_mutex_unlock(&world.mutex);
    usleep(10000); // 10ms
  }
  return NULL;
}

struct World *world_init(void) {
  pthread_mutex_init(&world.mutex, NULL);
  pthread_create(&world.thread, NULL, test, NULL);
  return &world;
}

static Chunk generateChunk(int cx, int cy, int cz) {
  Chunk chunk = chunk_init(cx, cy, cz);
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
      chunk.data[i] = voxel;
    }
  }

  return chunk;
}

// Get index in world.chunks by chunk's coordinates
// WARNING: This function is very slow. Use it as little as possible.
static size_t world_getChunk(struct World *world, int x, int y, int z) {
  pthread_mutex_lock(&world->mutex);
  size_t size = world->chunks.size;
  for (size_t i = 0; i < size; i++) {
    Chunk *chunk = &world->chunks.data[i];
    if (chunk->position.x == x && chunk->position.y == y && chunk->position.z == z) {
      pthread_mutex_unlock(&world->mutex);
      return i;
    }
  }
  pthread_mutex_unlock(&world->mutex);
  return -1;
}

// Get voxel type by coordinates in the world.
// WARNING: This function is very slow because it calls `world_getChunk()`
uint8_t world_getVoxel(struct World *world, int x, int y, int z) {
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

void world_orderChunk(struct World *world, int x, int y, int z) {
  bool found = false;
  pthread_mutex_lock(&world->mutex);
  // TODO: switch to smth like Tasks_find(...)
  for (size_t i = 0; i < world->tasks.size; i++) {
    struct Task task = world->tasks.data[i];
    if (task.x == x && task.y == y && task.z == z) { found = true; break; }
  }
  if (!found) { struct Task t = { x, y, z }; DArray_push(&world->tasks, t); }
  pthread_mutex_unlock(&world->mutex);
}

size_t world_getChunkOrGenerate(struct World *world, int x, int y, int z) {
  size_t chunk_index = world_getChunk(world, x, y, z);
  if (chunk_index == SIZE_MAX) world_orderChunk(world, x, y, z);
  return chunk_index;
}

// Adds chunks around given position to vector
// If needed chunks are not generated then it'll generate them.
// WARNING: This function is very slow because it calls `world_getChunk()`
void world_getChunkCube(struct SizeArray *vec, struct World *world, Vec3 around, int radius) {
  int chunkX = floorf((-around.x + CHUNK_SIZE / 2.0f) / CHUNK_SIZE);
  int chunkY = floorf((around.y + CHUNK_SIZE / 2.0f) / CHUNK_SIZE);
  int chunkZ = floorf((-around.z + CHUNK_SIZE / 2.0f) / CHUNK_SIZE);
  for (int x = -radius; x < radius; x++) {
    for (int y = -radius; y < radius; y++) {
      for (int z = -radius; z < radius; z++) {
        size_t chunk_index = world_getChunkOrGenerate(world, chunkX + x, chunkY + y, chunkZ + z);
        if (chunk_index == SIZE_MAX) continue;
	DArray_push(vec, chunk_index);
      }
    }
  }
}

void world_free(struct World *world) {
  world->exit = true;
  pthread_join(world->thread, NULL);
  DArray_free(world->chunks);
  DArray_free(world->tasks);
}
