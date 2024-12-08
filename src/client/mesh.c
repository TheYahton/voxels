#define _VECTOR_IMPL
#include "mesh.h"
#include "chunk.h"
#include "world.h"

#include <string.h>

#define positive_indices(vec, size)\
UInt32Vector_append(vec, size + 0);\
UInt32Vector_append(vec, size + 1);\
UInt32Vector_append(vec, size + 2);\
UInt32Vector_append(vec, size + 3);\
UInt32Vector_append(vec, size + 2);\
UInt32Vector_append(vec, size + 1);

#define negative_indices(vec, size)\
UInt32Vector_append(vec, size + 0);\
UInt32Vector_append(vec, size + 2);\
UInt32Vector_append(vec, size + 1);\
UInt32Vector_append(vec, size + 3);\
UInt32Vector_append(vec, size + 1);\
UInt32Vector_append(vec, size + 2);

VectorImpl(Mesh, MeshVector)
VectorImpl(Vertex, Vertices)

typedef struct {
	unsigned char r, g, b;
} Color;

const Color COLORS[TypeCount] = {
	{0, 0, 0},       // 0 Air
	{127, 127, 127}, // 1 Stone
	{64, 41, 5},     // 2 Dirt
	{25, 127, 25},   // 3 Grass
	{236, 237, 171}, // 4 Sand
	{21, 39, 237},   // 5 Water
};

void get_block_color(BlockType type, float *r, float *g , float *b) {
	Color color = COLORS[type];
	*r = (float)color.r / 255.0f;
	*g = (float)color.g / 255.0f;
	*b = (float)color.b / 255.0f;
}

	Mesh chunk_genmesh(const struct Chunk *chunk, const struct World *world) {
	Mesh mesh = {
		.vertices = Vertices_init(0, sizeof(Vertex) * 4),
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
				float r;
				float g;
				float b;
				get_block_color(chunk->data[i + j * CHUNK_SIZE + k * CHUNK_SIZE * CHUNK_SIZE], &r, &g, &b);
				if (curr) {
					if ((curr & 32)) {
						size_t size = mesh.vertices.size;

						Vertices_append(&mesh.vertices, (Vertex) {{x + 1, y + 0, z + 0}, {r, g, b}, 0});
						Vertices_append(&mesh.vertices, (Vertex) {{x + 1, y + 1, z + 0}, {r, g, b}, 0});
						Vertices_append(&mesh.vertices, (Vertex) {{x + 1, y + 0, z + 1}, {r, g, b}, 0});
						Vertices_append(&mesh.vertices, (Vertex) {{x + 1, y + 1, z + 1}, {r, g, b}, 0});

						positive_indices(&mesh.indices, size);
					}
					if ((curr & 16)) {
						size_t size = mesh.vertices.size;

						Vertices_append(&mesh.vertices, (Vertex) {{x + 0, y + 0, z + 0}, {r, g, b}, 1});
						Vertices_append(&mesh.vertices, (Vertex) {{x + 0, y + 1, z + 0}, {r, g, b}, 1});
						Vertices_append(&mesh.vertices, (Vertex) {{x + 0, y + 0, z + 1}, {r, g, b}, 1});
						Vertices_append(&mesh.vertices, (Vertex) {{x + 0, y + 1, z + 1}, {r, g, b}, 1});

						negative_indices(&mesh.indices, size);
					}
					if ((curr & 8)) {
						size_t size = mesh.vertices.size;

						Vertices_append(&mesh.vertices, (Vertex) {{x + 0, y + 1, z + 0}, {r, g, b}, 2});
						Vertices_append(&mesh.vertices, (Vertex) {{x + 0, y + 1, z + 1}, {r, g, b}, 2});
						Vertices_append(&mesh.vertices, (Vertex) {{x + 1, y + 1, z + 0}, {r, g, b}, 2});
						Vertices_append(&mesh.vertices, (Vertex) {{x + 1, y + 1, z + 1}, {r, g, b}, 2});

						positive_indices(&mesh.indices, size);
					}
					if ((curr & 4)) {
						size_t size = mesh.vertices.size;

						Vertices_append(&mesh.vertices, (Vertex) {{x + 0, y + 0, z + 0}, {r, g, b}, 3});
						Vertices_append(&mesh.vertices, (Vertex) {{x + 0, y + 0, z + 1}, {r, g, b}, 3});
						Vertices_append(&mesh.vertices, (Vertex) {{x + 1, y + 0, z + 0}, {r, g, b}, 3});
						Vertices_append(&mesh.vertices, (Vertex) {{x + 1, y + 0, z + 1}, {r, g, b}, 3});

						negative_indices(&mesh.indices, size);
					}
					if ((curr & 2)) {
						size_t size = mesh.vertices.size;

						Vertices_append(&mesh.vertices, (Vertex) {{x + 0, y + 0, z + 1}, {r, g, b}, 4});
						Vertices_append(&mesh.vertices, (Vertex) {{x + 0, y + 1, z + 1}, {r, g, b}, 4});
						Vertices_append(&mesh.vertices, (Vertex) {{x + 1, y + 0, z + 1}, {r, g, b}, 4});
						Vertices_append(&mesh.vertices, (Vertex) {{x + 1, y + 1, z + 1}, {r, g, b}, 4});

						negative_indices(&mesh.indices, size);
					}
					if ((curr & 1)) {
						size_t size = mesh.vertices.size;

						Vertices_append(&mesh.vertices, (Vertex) {{x + 0, y + 0, z + 0}, {r, g, b}, 5});
						Vertices_append(&mesh.vertices, (Vertex) {{x + 0, y + 1, z + 0}, {r, g, b}, 5});
						Vertices_append(&mesh.vertices, (Vertex) {{x + 1, y + 0, z + 0}, {r, g, b}, 5});
						Vertices_append(&mesh.vertices, (Vertex) {{x + 1, y + 1, z + 0}, {r, g, b}, 5});

						positive_indices(&mesh.indices, size);
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
