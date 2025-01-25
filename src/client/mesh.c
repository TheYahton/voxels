#define _VECTOR_IMPL
#include "mesh.h"
#include "chunk.h"
#include "world.h"

#include <string.h>

#define positive_indices(vec, size)                                            \
  UInt32Vector_append(vec, size + 0);                                          \
  UInt32Vector_append(vec, size + 1);                                          \
  UInt32Vector_append(vec, size + 2);                                          \
  UInt32Vector_append(vec, size + 3);                                          \
  UInt32Vector_append(vec, size + 2);                                          \
  UInt32Vector_append(vec, size + 1);

#define negative_indices(vec, size)                                            \
  UInt32Vector_append(vec, size + 0);                                          \
  UInt32Vector_append(vec, size + 2);                                          \
  UInt32Vector_append(vec, size + 1);                                          \
  UInt32Vector_append(vec, size + 3);                                          \
  UInt32Vector_append(vec, size + 1);                                          \
  UInt32Vector_append(vec, size + 2);

VectorImpl(Mesh, MeshVector) VectorImpl(Vertex, Vertices)

// temp = world_getVoxel(world, chunkX + x + a, chunkY + y + b, chunkZ + z + c);
#define getFace(a, b, c, n)                                                    \
  temp = chunk_get(chunk, x + a, y + b, z + c);                                \
  if (temp == 0 || temp == -1) {                                               \
    result = n | result;                                                       \
  }

    Mesh chunk_genmesh(const struct Chunk *chunk, const struct World *world) {
  Mesh mesh = {
      .vertices = Vertices_init(sizeof(Vertex) * 4, sizeof(Vertex) * 4),
      .indices = UInt32Vector_init(0, 64),
      .visible = true,
  };

  int chunkX = chunk->position.x * CHUNK_SIZE;
  int chunkY = chunk->position.y * CHUNK_SIZE;
  int chunkZ = chunk->position.z * CHUNK_SIZE;

  // I want to create an array of air voxels intersect solid voxels
  // One byte in this array is: 0 0 X+ X- Y+ Y- Z+ Z-
  // where X Y Z are coordinates and + - are representing inc and dec
  uint8_t array[CHUNK_CSIZE] = {0};
  for (int x = 0; x < CHUNK_SIZE; x++) {
    for (int y = 0; y < CHUNK_SIZE; y++) {
      for (int z = 0; z < CHUNK_SIZE; z++) {
        char curr = chunk_get(chunk, x, y, z);
        if (curr != 0) {
          uint8_t result = 0;
          char temp;
          // Всякий раз когда вызывается world_getVoxel внутри вызывается очень
          // дорогая функция world_getChunk Нужно либо оптимизировать
          // world_getChunk, либо уменьшить кол-во вызовов world_getVoxel во
          // время генерации меша. Кажется очевидным что большинство граней
          // вокселей, проверяемых ниже нами, находится в пределах одного чанка.
          // А еще кажется очевидным что нам нужно не более 7 чанков (1 наш и 6
          // соседей: x+, x-, y+, y-, z+, z-) для того чтобы найти блок за
          // границей нашего чанка. Зачем же тогда нам вызывать дорогую функцию
          // world_getChunk много раз, если мы знаем какие чанки нам нужны? P.S.
          // но оптимизировать world_getChunk было бы неплохо ;]
          getFace(1, 0, 0, 32);
          getFace(-1, 0, 0, 16);
          getFace(0, 1, 0, 8);
          getFace(0, -1, 0, 4);
          getFace(0, 0, 1, 2);
          getFace(0, 0, -1, 1);
          array[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] = result;
        }
      }
    }
  }

  for (int i = 0; i < CHUNK_SIZE; i++) {
    for (int j = 0; j < CHUNK_SIZE; j++) {
      for (int k = 0; k < CHUNK_SIZE; k++) {
        char curr = array[i + j * CHUNK_SIZE + k * CHUNK_SIZE * CHUNK_SIZE];
        int x = i + chunkX;
        int y = j + chunkY;
        int z = k + chunkZ;
        // VoxelType voxel = world_getVoxel(world, x, y, z);
        VoxelType voxel = chunk_get(chunk, i, j, k);
        if (curr) {
          if ((curr & 32)) {
            size_t size = mesh.vertices.size;

            Vertices_append(&mesh.vertices,
                            (Vertex){{x + 1, y + 0, z + 0}, voxel, 0});
            Vertices_append(&mesh.vertices,
                            (Vertex){{x + 1, y + 1, z + 0}, voxel, 0});
            Vertices_append(&mesh.vertices,
                            (Vertex){{x + 1, y + 0, z + 1}, voxel, 0});
            Vertices_append(&mesh.vertices,
                            (Vertex){{x + 1, y + 1, z + 1}, voxel, 0});

            positive_indices(&mesh.indices, size);
          }
          if ((curr & 16)) {
            size_t size = mesh.vertices.size;

            Vertices_append(&mesh.vertices,
                            (Vertex){{x + 0, y + 0, z + 0}, voxel, 1});
            Vertices_append(&mesh.vertices,
                            (Vertex){{x + 0, y + 1, z + 0}, voxel, 1});
            Vertices_append(&mesh.vertices,
                            (Vertex){{x + 0, y + 0, z + 1}, voxel, 1});
            Vertices_append(&mesh.vertices,
                            (Vertex){{x + 0, y + 1, z + 1}, voxel, 1});

            negative_indices(&mesh.indices, size);
          }
          if ((curr & 8)) {
            size_t size = mesh.vertices.size;

            Vertices_append(&mesh.vertices,
                            (Vertex){{x + 0, y + 1, z + 0}, voxel, 2});
            Vertices_append(&mesh.vertices,
                            (Vertex){{x + 0, y + 1, z + 1}, voxel, 2});
            Vertices_append(&mesh.vertices,
                            (Vertex){{x + 1, y + 1, z + 0}, voxel, 2});
            Vertices_append(&mesh.vertices,
                            (Vertex){{x + 1, y + 1, z + 1}, voxel, 2});

            positive_indices(&mesh.indices, size);
          }
          if ((curr & 4)) {
            size_t size = mesh.vertices.size;

            Vertices_append(&mesh.vertices,
                            (Vertex){{x + 0, y + 0, z + 0}, voxel, 3});
            Vertices_append(&mesh.vertices,
                            (Vertex){{x + 0, y + 0, z + 1}, voxel, 3});
            Vertices_append(&mesh.vertices,
                            (Vertex){{x + 1, y + 0, z + 0}, voxel, 3});
            Vertices_append(&mesh.vertices,
                            (Vertex){{x + 1, y + 0, z + 1}, voxel, 3});

            negative_indices(&mesh.indices, size);
          }
          if ((curr & 2)) {
            size_t size = mesh.vertices.size;

            Vertices_append(&mesh.vertices,
                            (Vertex){{x + 0, y + 0, z + 1}, voxel, 4});
            Vertices_append(&mesh.vertices,
                            (Vertex){{x + 0, y + 1, z + 1}, voxel, 4});
            Vertices_append(&mesh.vertices,
                            (Vertex){{x + 1, y + 0, z + 1}, voxel, 4});
            Vertices_append(&mesh.vertices,
                            (Vertex){{x + 1, y + 1, z + 1}, voxel, 4});

            negative_indices(&mesh.indices, size);
          }
          if ((curr & 1)) {
            size_t size = mesh.vertices.size;

            Vertices_append(&mesh.vertices,
                            (Vertex){{x + 0, y + 0, z + 0}, voxel, 5});
            Vertices_append(&mesh.vertices,
                            (Vertex){{x + 0, y + 1, z + 0}, voxel, 5});
            Vertices_append(&mesh.vertices,
                            (Vertex){{x + 1, y + 0, z + 0}, voxel, 5});
            Vertices_append(&mesh.vertices,
                            (Vertex){{x + 1, y + 1, z + 0}, voxel, 5});

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
