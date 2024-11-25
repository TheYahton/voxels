#include <cglm/cglm.h>

#ifndef _UTILS_H
#define _UTILS_H
typedef struct {
	float x, y, z;
} Vec3;

typedef struct {
	int x, y, z;
} Vec3i;

float vec3_length(Vec3);
Vec3 vec3_mulff(Vec3, float);
Vec3 vec3_divf(Vec3, float);
Vec3 vec3_norm(Vec3);

#define Vectorize(type, name)                                                  \
	typedef struct {                                                           \
		type *data;                                                            \
		size_t capacity;                                                       \
		size_t size;                                                           \
		size_t step;                                                           \
	} name;                                                                    \
	name name##_init(size_t capacity, size_t step);                            \
	void name##_append(name *vec, type value);

#ifdef _VECTOR_IMPL
#include <stdlib.h>
#define VectorImpl(type, name)                                                 \
	name name##_init(size_t capacity, size_t step) {                           \
		type *data = (type *)malloc(capacity * sizeof(type));                  \
		return (name){                                                         \
			data,                                                              \
			capacity,                                                          \
			0,                                                                 \
			step,                                                              \
		};                                                                     \
	}                                                                          \
                                                                               \
	void name##_append(name *vec, type value) {                                \
		if (vec->size >= vec->capacity) {                                      \
			vec->capacity += vec->step;                                        \
			vec->data =                                                        \
				(type *)realloc(vec->data, vec->capacity * sizeof(type));      \
		}                                                                      \
		vec->data[vec->size] = value;                                          \
		vec->size++;                                                           \
	}
#endif // _VECTOR_IMPL

#include <stdint.h>
Vectorize(float, FloatVector) Vectorize(unsigned int, UnsignedIntVector)
	Vectorize(uint32_t, UInt32Vector) Vectorize(size_t, SizeVector)

		void print_mat4(mat4 mat);
#endif // _UTILS_H
