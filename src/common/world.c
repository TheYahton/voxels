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
	return (struct World) {ChunkVector_init(0, 64)};
}

static size_t world_chunk_generate(struct World *world, int x, int y, int z) {
	ChunkVector_append(&world->chunks, chunk_init(x, y, z));
	size_t index = world->chunks.size - 1;
	Chunk *chunk = &world->chunks.data[index];
	BlockType block = mod(x, 5) + 1;

	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int z = 0; z < CHUNK_SIZE; z++) {
			for (int y = 0; y < CHUNK_SIZE; y++) {
				int dx = x - CHUNK_SIZE / 2;
				int dy = y - CHUNK_SIZE / 2;
				int dz = z - CHUNK_SIZE / 2;
				int squared_dist = dx * dx + dy * dy + dz * dz;
				if (squared_dist < CHUNK_SIZE * CHUNK_SIZE / 16) {
					chunk_set(chunk, x, y, z, block);
				}
			}
		}
	}

	return index;
}

// Get index in world.chunks by chunk's coordinates
static size_t world_chunk_get(const struct World *world, int x, int y, int z) {
	for (size_t i = 0; i < world->chunks.size; i++) {
		Chunk *chunk = &world->chunks.data[i];
		if (chunk->position.x == x && chunk->position.y == y &&
			chunk->position.z == z) {
			return i;
		}
	}
	return -1;
}

// Get index in world.chunks by chunk's coordinates or generate if it's not found
static size_t world_chunk_get_or_generate(struct World *world, int x, int y, int z) {
	size_t chunk_index = world_chunk_get(world, x, y, z);
	if (chunk_index != SIZE_MAX) return chunk_index;
	return world_chunk_generate(world, x, y, z);
}

// Get block type by block's coordinates in the world.
uint8_t world_block_get(const struct World *world, int x, int y, int z) {
	uint8_t blockX = mod(x, CHUNK_SIZE);
	uint8_t blockY = mod(y, CHUNK_SIZE);
	uint8_t blockZ = mod(z, CHUNK_SIZE);
	int chunkX = floorf((float)x / CHUNK_SIZE);
	int chunkY = floorf((float)y / CHUNK_SIZE);
	int chunkZ = floorf((float)z / CHUNK_SIZE);
	size_t chunk_index = world_chunk_get(world, chunkX, chunkY, chunkZ);
	Chunk *chunk = NULL;
	if (chunk_index != SIZE_MAX) {
		chunk = &world->chunks.data[chunk_index];
	}
	return chunk_get(chunk, blockX, blockY, blockZ);
}

// Set block type by block's coordinates in the world.
void world_block_set(struct World *world, int x, int y, int z, uint8_t value) {
	uint8_t blockX = mod(x, CHUNK_SIZE);
	uint8_t blockY = mod(y, CHUNK_SIZE);
	uint8_t blockZ = mod(z, CHUNK_SIZE);
	int chunkX = floorf((float)x / CHUNK_SIZE);
	int chunkY = floorf((float)y / CHUNK_SIZE);
	int chunkZ = floorf((float)z / CHUNK_SIZE);
	size_t chunk_index = world_chunk_get(world, chunkX, chunkY, chunkZ);
	Chunk *chunk = NULL;
	if (chunk_index != SIZE_MAX) {
		chunk = &world->chunks.data[chunk_index];
	}
	chunk_set(chunk, blockX, blockY, blockZ, value);
}

// Adds chunks around given position to vector
// If needed chunks are not generated then it'll generate it.
void world_chunk_cube(SizeVector *vec, struct World *world, Vec3 around, int radius) {
	int chunkX = floorf((-around.x + CHUNK_SIZE / 2.0f) / CHUNK_SIZE);
	int chunkY = floorf((around.y + CHUNK_SIZE / 2.0f) / CHUNK_SIZE);
	int chunkZ = floorf((-around.z + CHUNK_SIZE / 2.0f) / CHUNK_SIZE);
	for (int x = -radius; x < radius; x++) {
		for (int y = -radius; y < radius; y++) {
			for (int z = -radius; z < radius; z++) {
				size_t index = world_chunk_get_or_generate(world, chunkX + x, chunkY + y, chunkZ + z);
				SizeVector_append(vec, index);
			}
		}
	}
}

void world_free(struct World *world) {
	for (size_t i = 0; i < world->chunks.size; i++)
		chunk_free(&world->chunks.data[i]);
	free(world->chunks.data);
}
