#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "chunk.h"
#include "mesh.h"
#include "logging.h"

#define CUBE_FACES 6

#define VERTICES_PER_FACE 4
#define MAX_VERTICES (CHUNK_CSIZE * CUBE_FACES * VERTICES_PER_FACE)

#define INDICES_PER_FACE 6
#define MAX_INDICES  (CHUNK_CSIZE * CUBE_FACES * INDICES_PER_FACE)

struct Vertices temp_vertices = {0};
struct UInt32Array temp_indices = {0};

void mesh_initTempBuffers(void) {
    DArray_reserve(&temp_vertices, MAX_VERTICES);
    logging_log(LL_DEBUG, "Allocated %.1f MB for %zu elements in temp_vertices",
        (float)(sizeof(temp_vertices.data[0]) * temp_vertices.capacity) / (1024 * 1024),
        temp_vertices.capacity);

    DArray_reserve(&temp_indices, MAX_INDICES);
    logging_log(LL_DEBUG, "Allocated %.1f MB for %zu elements in temp_indices",
        (float)(sizeof(temp_indices.data[0]) * temp_vertices.capacity) / (1024 * 1024),
        temp_indices.capacity);
}

void mesh_deinitTempBuffers(void) {
    DArray_free(temp_vertices);
    DArray_free(temp_indices);
}

#define clockwise_indices(vec, size)                                           \
  DArray_push(vec, size + 0);                                          \
  DArray_push(vec, size + 1);                                          \
  DArray_push(vec, size + 2);                                          \
  DArray_push(vec, size + 3);                                          \
  DArray_push(vec, size + 2);                                          \
  DArray_push(vec, size + 1);

#define aclockwise_indices(vec, size)                                          \
  DArray_push(vec, size + 0);                                          \
  DArray_push(vec, size + 2);                                          \
  DArray_push(vec, size + 1);                                          \
  DArray_push(vec, size + 3);                                          \
  DArray_push(vec, size + 1);                                          \
  DArray_push(vec, size + 2);

#define getFace(a, b, c, n)                                                    \
  temp = chunk_get(chunk, x + a, y + b, z + c);                                \
  if (temp == 0 || temp == -1) {                                               \
    result = n | result;                                                       \
  }

Mesh chunk_genmesh(struct Chunk *chunk) {
  // An array of touches of air and solid voxels.
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

  for (int x = 0; x < CHUNK_SIZE; x++) {
    for (int y = 0; y < CHUNK_SIZE; y++) {
      for (int z = 0; z < CHUNK_SIZE; z++) {
        char curr = array[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE];
        // FIX: если chunk_get вместо world_getVoxel то на границах чанков проблемы.
        // VoxelType voxel = world_getVoxel(world, x, y, z);
        VoxelType voxel = chunk_get(chunk, x, y, z);
        // #define magic(n, a, b, c) DArray_push(&mesh.vertices, (Vertex){{x + a, y + b, z + c}, voxel, n})
        #define magic(n, a, b, c) DArray_push(&temp_vertices, (Vertex){{x + a, y + b, z + c}, voxel, n})
        if (curr) {
          if ((curr & 32)) {
            magic(0, 1, 0, 0);
            magic(0, 1, 1, 0);
            magic(0, 1, 0, 1);
            magic(0, 1, 1, 1);
            clockwise_indices(&temp_indices, temp_vertices.size - 4);
          }
          if ((curr & 16)) {
            magic(1, 0, 0, 0);
            magic(1, 0, 1, 0);
            magic(1, 0, 0, 1);
            magic(1, 0, 1, 1);
            aclockwise_indices(&temp_indices, temp_vertices.size - 4);
          }
          if ((curr & 8)) {
            magic(2, 0, 1, 0);
            magic(2, 0, 1, 1);
            magic(2, 1, 1, 0);
            magic(2, 1, 1, 1);
            clockwise_indices(&temp_indices, temp_vertices.size - 4);
          }
          if ((curr & 4)) {
            magic(3, 0, 0, 0);
            magic(3, 0, 0, 1);
            magic(3, 1, 0, 0);
            magic(3, 1, 0, 1);
            aclockwise_indices(&temp_indices, temp_vertices.size - 4);
          }
          if ((curr & 2)) {
            magic(4, 0, 0, 1);
            magic(4, 0, 1, 1);
            magic(4, 1, 0, 1);
            magic(4, 1, 1, 1);
            aclockwise_indices(&temp_indices, temp_vertices.size - 4);
          }
          if ((curr & 1)) {
            magic(5, 0, 0, 0);
            magic(5, 0, 1, 0);
            magic(5, 1, 0, 0);
            magic(5, 1, 1, 0);
            clockwise_indices(&temp_indices, temp_vertices.size - 4);
          }
        }
        #undef magic
      }
    }
  }

  Mesh mesh = {
      .vertices = {0},
      .indices = {0},
      .visible = true,
      vec3i_muli(chunk->position, CHUNK_SIZE),
  };

  DArray_append(&mesh.vertices, temp_vertices.data, temp_vertices.size);
  DArray_append(&mesh.indices, temp_indices.data, temp_indices.size);

  temp_vertices.size = 0;
  temp_indices.size = 0;

  return mesh;
}

void mesh_free(Mesh *mesh) {
  DArray_free(mesh->vertices);
  DArray_free(mesh->indices);
}
