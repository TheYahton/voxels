#define _vector_impl
#include "utils.h"
#include "world.h"
#include "render.h"

VectorImpl(Chunk, ChunkVector)

World world_init() {
	ChunkVector chunks = ChunkVector_init(0, 64);

	for (int x = 0; x < 4; x++) {
		for (int z = 0; z < 4; z++) {
			ChunkVector_append(&chunks, chunk_init(x-2, 0, z-2));
			chunk_worldgen(&chunks.data[z + x*4]);
			chunks.data[z + x*4].mesh = chunk_genmesh(&chunks.data[z + x*4]);
			chunks.data[z + x*4].VAO = render_create_vao(&chunks.data[z + x*4].mesh);
		}
	}
	return (World) {
		chunks,
	};
}

void world_free(World *world) {
	for (unsigned int i = 0; i < world->chunks.size; i++) chunk_free(&world->chunks.data[i]);
	mesh_free(&world->chunks.data->mesh);
}
