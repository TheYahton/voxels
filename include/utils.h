#ifndef UTILS_H
#define UTILS_H

typedef struct {
  float x, y, z;
} Vec3;

typedef struct {
  int x, y, z;
} Vec3i;

float vec3_length(Vec3);
Vec3 vec3_sub(Vec3, Vec3);
Vec3 vec3_mulf(Vec3, float);
Vec3 vec3_divf(Vec3, float);
Vec3 vec3_norm(Vec3);
Vec3i vec3i_muli(Vec3i, int);

struct UInt32Array {
  size_t size;
  size_t capacity;
  uint32_t *data;
};

struct SizeArray {
  size_t size;
  size_t capacity;
  size_t *data;
};

#define DArray_get(arr, i) *((arr)->data + (i))
#define DArray_getLast(arr) DArray_get((arr), (arr)->size - 1)
// #define DArray_push(arr, item) DArray_append(arr, item, 1)
#define DArray_push(arr, ...) do {\
  if ((arr)->size + 1 > (arr)->capacity) DArray_extend((arr), 1 - ((arr)->capacity - (arr)->size));\
  (arr)->data[(arr)->size++] = (__VA_ARGS__);\
} while (0)
// Copy the data somewhere before next push or append.
#define DArray_pop(arr) DArray_get(arr, --(arr)->size)
#define DArray_free(arr) free((arr).data)
#define DArray_extend(arr, n) do {\
  (arr)->capacity += (n);\
  (arr)->data = realloc((arr)->data, sizeof((arr)->data[0]) * (arr)->capacity);\
} while (0)
#define DArray_append(arr, src, n) do {\
  if ((arr)->size + (n) > (arr)->capacity) DArray_extend((arr), (n) - ((arr)->capacity - (arr)->size));\
  memcpy(DArray_get((arr), (arr)->size), (src), (n) * sizeof((arr)->data[0]));\
  (arr)->size += (n);\
} while (0)

#endif // UTILS_H
