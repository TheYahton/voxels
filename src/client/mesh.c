#define _VECTOR_IMPL
#include "mesh.h"
#include "chunk.h"
#include "world.h"

#include <string.h>

VectorImpl(Mesh, MeshVector)

#define FloatVector_addTrio(x, y, z)                                           \
	FloatVector_append(&mesh.vertices, x);                                     \
	FloatVector_append(&mesh.vertices, y);                                     \
	FloatVector_append(&mesh.vertices, z);

#define FloatVector_rgb(r, g, b) FloatVector_addTrio(r, g, b)

#define FloatVector_addVertex(x, y, z, r, g, b)                                         \
	FloatVector_addTrio(x, y, z) FloatVector_rgb(r, g, b)

#define FloatVector_addNormal(x, y, z) FloatVector_addTrio(x, y, z)

#define STRANGE_CONSTANT 9

	Mesh chunk_genmesh(const struct Chunk *chunk, const struct World *world) {
	Mesh mesh = {
		.vertices = FloatVector_init(0, 64),
		.indices = UInt32Vector_init(0, 64),
		.visible = true,
	};

	int chunkX = chunk->position.x;
	int chunkY = chunk->position.y;
	int chunkZ = chunk->position.z;

	// I want to create an array of air voxels intersect solid voxels
	// One byte in this array is: 0 0 X+ X- Y+ Y- Z+ Z-
	// where X Y Z are coordinates and + - are representing inc and dec
	uint8_t array[CHUNK_CSIZE];
	memset(array, 0, CHUNK_CSIZE);
	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int y = 0; y < CHUNK_SIZE; y++) {
			for (int z = 0; z < CHUNK_SIZE; z++) {
				char curr = chunk_get(chunk, x, y, z);
				if (curr != 0) {
					uint8_t result = 0;
					char Xinc = world_block_get(
						world, chunkX * CHUNK_SIZE + x + 1,
						chunkY * CHUNK_SIZE + y, chunkZ * CHUNK_SIZE + z);
					if (Xinc == 0 || Xinc == -1) {
						result = 32 | result;
					}
					char Xdec = world_block_get(
						world, chunkX * CHUNK_SIZE + x - 1,
						chunkY * CHUNK_SIZE + y, chunkZ * CHUNK_SIZE + z);
					if (Xdec == 0 || Xdec == -1) {
						result = 16 | result;
					}
					char Yinc = world_block_get(world, chunkX * CHUNK_SIZE + x,
												chunkY * CHUNK_SIZE + y + 1,
												z + CHUNK_SIZE * chunkZ);
					if (Yinc == 0 || Yinc == -1) {
						result = 8 | result;
					}
					char Ydec = world_block_get(world, chunkX * CHUNK_SIZE + x,
												chunkY * CHUNK_SIZE + y - 1,
												z + CHUNK_SIZE * chunkZ);
					if (Ydec == 0 || Ydec == -1) {
						result = 4 | result;
					}
					char Zinc = world_block_get(world, chunkX * CHUNK_SIZE + x,
												chunkY * CHUNK_SIZE + y,
												chunkZ * CHUNK_SIZE + z + 1);
					if (Zinc == 0 || Zinc == -1) {
						result = 2 | result;
					}
					char Zdec = world_block_get(world, chunkX * CHUNK_SIZE + x,
												chunkY * CHUNK_SIZE + y,
												chunkZ * CHUNK_SIZE + z - 1);
					if (Zdec == 0 || Zdec == -1) {
						result = 1 | result;
					}
					array[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] =
						result;
				}
			}
		}
	}

	for (int i = 0; i < CHUNK_SIZE; i++) {
		for (int j = 0; j < CHUNK_SIZE; j++) {
			for (int k = 0; k < CHUNK_SIZE; k++) {
				char curr =
					array[i + j * CHUNK_SIZE + k * CHUNK_SIZE * CHUNK_SIZE];
				int x = i + chunk->position.x * CHUNK_SIZE;
				int y = j + chunk->position.y * CHUNK_SIZE;
				int z = k + chunk->position.z * CHUNK_SIZE;
				float r = 0.1;
				float g = 0.5;
				float b = 0.1;
				if (curr) {
					if ((curr & 32)) {
						float nx = 1.0f;
						float ny = 0.0f;
						float nz = 0.0f;

						size_t size = mesh.vertices.size;
						FloatVector_addVertex(1.0 + x, 0.0 + y, 0.0 + z, r, g, b);
						FloatVector_addNormal(nx, ny, nz);
						FloatVector_addVertex(1.0 + x, 1.0 + y, 0.0 + z, r, g, b);
						FloatVector_addNormal(nx, ny, nz);
						FloatVector_addVertex(1.0 + x, 0.0 + y, 1.0 + z, r, g, b);
						FloatVector_addNormal(nx, ny, nz);
						FloatVector_addVertex(1.0 + x, 1.0 + y, 1.0 + z, r, g, b);
						FloatVector_addNormal(nx, ny, nz);

						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 0);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 1);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 2);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 3);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 2);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 1);
					}
					if ((curr & 16)) {
						float nx = -1.0f;
						float ny = 0.0f;
						float nz = 0.0f;

						size_t size = mesh.vertices.size;
						FloatVector_addVertex(x, 0.0 + y, 0.0 + z, r, g, b);
						FloatVector_addNormal(nx, ny, nz);
						FloatVector_addVertex(x, 1.0 + y, 0.0 + z, r, g, b);
						FloatVector_addNormal(nx, ny, nz);
						FloatVector_addVertex(x, 0.0 + y, 1.0 + z, r, g, b);
						FloatVector_addNormal(nx, ny, nz);
						FloatVector_addVertex(x, 1.0 + y, 1.0 + z, r, g, b);
						FloatVector_addNormal(nx, ny, nz);

						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 0);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 2);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 1);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 3);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 1);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 2);
					}
					if ((curr & 8)) {
						float nx = 0.0f;
						float ny = 1.0f;
						float nz = 0.0f;

						size_t size = mesh.vertices.size;
						FloatVector_addVertex(0.0 + x, 1.0 + y, 0.0 + z, r, g, b);
						FloatVector_addNormal(nx, ny, nz);
						FloatVector_addVertex(1.0 + x, 1.0 + y, 0.0 + z, r, g, b);
						FloatVector_addNormal(nx, ny, nz);
						FloatVector_addVertex(0.0 + x, 1.0 + y, 1.0 + z, r, g, b);
						FloatVector_addNormal(nx, ny, nz);
						FloatVector_addVertex(1.0 + x, 1.0 + y, 1.0 + z, r, g, b);
						FloatVector_addNormal(nx, ny, nz);

						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 0);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 2);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 1);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 3);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 1);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 2);
					}
					if ((curr & 4)) {
						float nx = 0.0f;
						float ny = -1.0f;
						float nz = 0.0f;

						size_t size = mesh.vertices.size;
						FloatVector_addVertex(0.0 + x, y, 0.0 + z, r, g, b);
						FloatVector_addNormal(nx, ny, nz);
						FloatVector_addVertex(1.0 + x, y, 0.0 + z, r, g, b);
						FloatVector_addNormal(nx, ny, nz);
						FloatVector_addVertex(0.0 + x, y, 1.0 + z, r, g, b);
						FloatVector_addNormal(nx, ny, nz);
						FloatVector_addVertex(1.0 + x, y, 1.0 + z, r, g, b);
						FloatVector_addNormal(nx, ny, nz);

						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 0);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 1);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 2);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 3);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 2);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 1);
					}
					if ((curr & 2)) {
						float nx = 0.0f;
						float ny = 0.0f;
						float nz = 1.0f;

						size_t size = mesh.vertices.size;
						FloatVector_addVertex(0.0 + x, 0.0 + y, 1.0 + z, r, g, b);
						FloatVector_addNormal(nx, ny, nz);
						FloatVector_addVertex(0.0 + x, 1.0 + y, 1.0 + z, r, g, b);
						FloatVector_addNormal(nx, ny, nz);
						FloatVector_addVertex(1.0 + x, 0.0 + y, 1.0 + z, r, g, b);
						FloatVector_addNormal(nx, ny, nz);
						FloatVector_addVertex(1.0 + x, 1.0 + y, 1.0 + z, r, g, b);
						FloatVector_addNormal(nx, ny, nz);

						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 0);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 2);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 1);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 3);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 1);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 2);
					}
					if ((curr & 1)) {
						float nx = 0.0f;
						float ny = 0.0f;
						float nz = -1.0f;

						size_t size = mesh.vertices.size;
						FloatVector_addVertex(0.0 + x, 0.0 + y, z, r, g, b);
						FloatVector_addNormal(nx, ny, nz);
						FloatVector_addVertex(0.0 + x, 1.0 + y, z, r, g, b);
						FloatVector_addNormal(nx, ny, nz);
						FloatVector_addVertex(1.0 + x, 0.0 + y, z, r, g, b);
						FloatVector_addNormal(nx, ny, nz);
						FloatVector_addVertex(1.0 + x, 1.0 + y, z, r, g, b);
						FloatVector_addNormal(nx, ny, nz);

						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 0);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 1);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 2);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 3);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 2);
						UInt32Vector_append(&mesh.indices,
											size / STRANGE_CONSTANT + 1);
					}
				}
			}
		}
	}

	return mesh;
}

void mesh_free(Mesh *mesh) {
	free(mesh->vertices.data);
	free(mesh->indices.data);
}
