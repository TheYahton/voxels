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

// Gives a pointer to Ith element of the array
#define DArray_get(arr, i) ((arr)->data + (i))

// Gives a pointer to the last element of the array
// P.S. Array's size should be greater than zero
#define DArray_getLast(arr) DArray_get((arr), (arr)->size - 1)

// Adds an element at the end of the array
#define DArray_push(arr, ...) do {\
  DArray_reserve((arr), (arr)->size + 1);\
  (arr)->data[(arr)->size++] = (__VA_ARGS__);\
} while (0)

// Gives a pointer to the last element of the array AND shrinks it by 1
// P.S. Copy the data somewhere before next push or append.
#define DArray_pop(arr) DArray_get(arr, --(arr)->size)

// Destroys the array
#define DArray_free(arr) free((arr).data)

// Increases the array's capacity to hold at least N elements
#define DArray_reserve(arr, n) do {\
  if ((n) > (arr)->capacity) (arr)->capacity = (n);\
  (arr)->data = realloc((arr)->data, sizeof((arr)->data[0]) * (arr)->capacity);\
} while (0)

// Increases the array's capacity by N
#define DArray_extend(arr, n) do {\
  (arr)->capacity += (n);\
  (arr)->data = realloc((arr)->data, sizeof((arr)->data[0]) * (arr)->capacity);\
} while (0)

// Adds many elements from src
#define DArray_append(arr, src, n) do {\
  DArray_reserve((arr), (arr)->size + (n));\
  memcpy((arr)->data + (arr)->size, (src), (n) * sizeof((arr)->data[0]));\
  (arr)->size += (n);\
} while (0)

#define DArray_isEmpty(arr) ((arr).size == 0)

#endif // UTILS_H
