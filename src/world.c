#define _vector_impl
#include "world.h"
#include "chunk.h"
#include "render.h"
#include "utils.h"

#define WORLD_SIZE 1024
#define WORLD_QSIZE (WORLD_SIZE * WORLD_SIZE * WORLD_SIZE)

VectorImpl(Chunk, ChunkVector)

	struct World world_init() {
	World world = {ChunkVector_init(0, 64)};

	for (int i = 0; i < 64; i++) {
		unsigned int x = i % 8 - 4;
		unsigned int z = i / 8 - 4;
		world_chunk_generate(&world, x, 0, z);
	}

	for (unsigned int i = 0; i < world.chunks.size; i++) {
		world_mesh_generate(&world, i);
	}

	return world;
}

void world_chunk_generate(World *world, int x, int y, int z) {
	ChunkVector_append(&world->chunks, chunk_init(world, x, y, z));
	unsigned int index = world->chunks.size - 1;
	world->chunks.data[index].world = world;
	chunk_worldgen(&world->chunks.data[index]);
}

void world_mesh_generate(World *world, unsigned int index) {
	world->chunks.data[index].mesh = chunk_genmesh(&world->chunks.data[index]);
	world->chunks.data[index].VAO =
		render_create_vao(&world->chunks.data[index].mesh);
}

Chunk *world_chunk_get(World *world, int x, int y, int z) {
	for (unsigned int i = 0; i < world->chunks.size; i++) {
		Chunk *chunk = &world->chunks.data[i];
		if (chunk->position.x == x && chunk->position.y == y &&
			chunk->position.z == z) {
			return chunk;
		}
	}
	return NULL;
}

inline int mod(int a, int b) {
	int r = a % b;
	return r < 0 ? r + b : r;
}
unsigned char world_block_get(World *world, int x, int y, int z) {
	unsigned int blockX = mod(x, 32);
	unsigned int blockY = mod(y, 32);
	unsigned int blockZ = mod(z, 32);
	int chunkX = x / 32;
	int chunkY = y / 32;
	int chunkZ = z / 32;
	return chunk_get(world_chunk_get(world, chunkX, chunkY, chunkZ), blockX,
					 blockY, blockZ);
}

void world_free(World *world) {
	for (unsigned int i = 0; i < world->chunks.size; i++)
		chunk_free(&world->chunks.data[i]);
	mesh_free(&world->chunks.data->mesh);
}
