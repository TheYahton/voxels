#define _VECTOR_IMPL
#include "world.h"
#include "chunk.h"
#include "render.h"
#include "utils.h"

#include <math.h>

VectorImpl(Chunk, ChunkVector)

	struct World world_init(void) {
	struct World world = {ChunkVector_init(0, 64)};

	for (int i = 0; i < 256; i++) {
		size_t x = i % 16 - 8;
		size_t z = i / 16 - 8;
		world_chunk_generate(&world, x, 0, z);
	}

	return world;
}

void world_chunk_generate(struct World *world, int x, int y, int z) {
	ChunkVector_append(&world->chunks, chunk_init(x, y, z));
	size_t index = world->chunks.size - 1;
	Chunk *chunk = &world->chunks.data[index];

	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int z = 0; z < CHUNK_SIZE; z++) {
			// int y_level = sin((float)x / 5.0f) * 3 + 5.0f;
			int y_level = (sinf((float)x / 10.0f) * sinf((float)z / 10.0f) + 1.0f) * 5;
			for (int y = 0; y < y_level; y++) {
				BlockType block;
				if (y < 5) block = Water;
				else if (y == 5) block = Sand;
				else block = Grass;
				chunk_set(chunk, x, y, z, block);
			}
		}
	}
}

Chunk *world_chunk_get(const struct World *world, int x, int y, int z) {
	for (size_t i = 0; i < world->chunks.size; i++) {
		Chunk *chunk = &world->chunks.data[i];
		if (chunk->position.x == x && chunk->position.y == y &&
			chunk->position.z == z) {
			return chunk;
		}
	}
	return NULL;
}

static int mod(int a, int b) {
	int r = a % b;
	return r < 0 ? r + b : r;
}

uint8_t world_block_get(const struct World *world, int x, int y, int z) {
	uint8_t blockX = mod(x, CHUNK_SIZE);
	uint8_t blockY = mod(y, CHUNK_SIZE);
	uint8_t blockZ = mod(z, CHUNK_SIZE);
	int chunkX = floorf((float)x / CHUNK_SIZE);
	int chunkY = floorf((float)y / CHUNK_SIZE);
	int chunkZ = floorf((float)z / CHUNK_SIZE);
	return chunk_get(world_chunk_get(world, chunkX, chunkY, chunkZ), blockX,
					 blockY, blockZ);
}

void world_block_set(struct World *world, int x, int y, int z, uint8_t value) {
	uint8_t blockX = mod(x, CHUNK_SIZE);
	uint8_t blockY = mod(y, CHUNK_SIZE);
	uint8_t blockZ = mod(z, CHUNK_SIZE);
	int chunkX = floorf((float)x / CHUNK_SIZE);
	int chunkY = floorf((float)y / CHUNK_SIZE);
	int chunkZ = floorf((float)z / CHUNK_SIZE);
	chunk_set(world_chunk_get(world, chunkX, chunkY, chunkZ), blockX, blockY,
			  blockZ, value);
}

void world_chunk_circle(SizeVector *vec, const struct World *world, float x,
						float y, float z, int radius) {
	for (size_t i = 0; i < world->chunks.size; i++) {
		const Vec3i *chunkPos = &world->chunks.data[i].position;
		float dx = CHUNK_SIZE * chunkPos->x - x + CHUNK_SIZE / 2.0;
		float dy = CHUNK_SIZE * chunkPos->y - y + CHUNK_SIZE / 2.0;
		float dz = CHUNK_SIZE * chunkPos->z - z + CHUNK_SIZE / 2.0;
		float dist = sqrtf(dx * dx + dy * dy + dz * dz);
		if (dist < radius * CHUNK_SIZE) {
			SizeVector_append(vec, i);
		}
	}
}

void world_free(struct World *world) {
	for (size_t i = 0; i < world->chunks.size; i++)
		chunk_free(&world->chunks.data[i]);
	free(world->chunks.data);
}
