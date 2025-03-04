#define _VECTOR_IMPL
#include "world.h"
#include "chunk.h"
#include "utils.h"

#include <stdio.h>
#include <pthread.h>
#define __USE_MISC
#include <unistd.h>
#include <math.h>

VectorImpl(Chunk *, ChunkVector)
VectorImpl(struct Task, Tasks)

static int mod(int a, int b) {
  int r = a % b;
  return r < 0 ? r + b : r;
}

static Chunk *generateChunk(int cx, int cy, int cz);

void *test(void *ptr) {
  struct World *world = ptr;
  bool running = true;
  while (running) {
    if (world->exit) running = false;
    pthread_mutex_lock(&world->mutex);
    if (world->tasks.size > 0) {
      struct Task task = world->tasks.data[world->tasks.size-1];
      world->tasks.size--;
      pthread_mutex_unlock(&world->mutex);
      Chunk *chunk = generateChunk(task.x, task.y, task.z);
      pthread_mutex_lock(&world->mutex);
      ChunkVector_append(&world->chunks, chunk);
    }
    pthread_mutex_unlock(&world->mutex);
  }
  return NULL;
}

struct World *world_init(void) {
  struct World *world = malloc(sizeof(struct World));
  pthread_t thread;
  pthread_create(&thread, NULL, test, world);
  pthread_mutex_t m;
  pthread_mutex_init(&m, NULL);
  *world = (struct World) {
    ChunkVector_init(0, 64),
    Tasks_init(64, 64),
    thread,
    m,
    false,
  };
  return world;
}

static Chunk *generateChunk(int cx, int cy, int cz) {
  Chunk *chunk = chunk_init(cx, cy, cz);
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

  return chunk;
}

// Get index in world.chunks by chunk's coordinates
// WARNING: This function is very slow. Use it as little as possible.
static size_t world_getChunk(struct World *world, int x, int y, int z) {
  pthread_mutex_lock(&world->mutex);
  size_t size = world->chunks.size;
  for (size_t i = 0; i < size; i++) {
    Chunk *chunk = world->chunks.data[i];
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
    chunk = world->chunks.data[chunk_index];
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
    chunk = world->chunks.data[chunk_index];
  }
  chunk_set(chunk, voxelX, voxelY, voxelZ, value);
}

// Adds chunks around given position to vector
// If needed chunks are not generated then it'll generate it.
// WARNING: This function is very slow because it calls `world_getChunk()`
void world_getChunkCube(SizeVector *vec, struct World *world, Vec3 around, int radius) {
  int chunkX = floorf((-around.x + CHUNK_SIZE / 2.0f) / CHUNK_SIZE);
  int chunkY = floorf((around.y + CHUNK_SIZE / 2.0f) / CHUNK_SIZE);
  int chunkZ = floorf((-around.z + CHUNK_SIZE / 2.0f) / CHUNK_SIZE);
  for (int x = -radius; x < radius; x++) {
    for (int y = -radius; y < radius; y++) {
      for (int z = -radius; z < radius; z++) {
        int rx = chunkX + x, ry = chunkY + y, rz = chunkZ + z;
        size_t chunk_index = world_getChunk(world, rx, ry, rz);
        if (chunk_index != SIZE_MAX) {
          SizeVector_append(vec, chunk_index);
        } else {
          bool here = false;
          pthread_mutex_lock(&world->mutex);
          for (size_t i = 0; i < world->tasks.size; i++) {
            struct Task task = world->tasks.data[i];
            if (task.x == rx && task.y == ry && task.z == rz) here = true;
          }
          if (!here) Tasks_append(&world->tasks, (struct Task) {chunkX + x, chunkY + y, chunkZ + z});
          pthread_mutex_unlock(&world->mutex);
        }
      }
    }
  }
}

void world_free(struct World *world) {
  world->exit = true;
  pthread_join(world->thread, NULL);
  for (size_t i = 0; i < world->chunks.size; i++)
    chunk_free(world->chunks.data[i]);
  free(world->chunks.data);
}
