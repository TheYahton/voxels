#include <math.h>
#define _VECTOR_IMPL
#include "utils.h"

float vec3_length(Vec3 v) { return sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }

Vec3 vec3_sub(Vec3 a, Vec3 b) {
  return (Vec3){
      a.x - b.x,
      a.y - b.y,
      a.z - b.z,
  };
}

Vec3 vec3_mulf(Vec3 v, float s) {
  return (Vec3){
      v.x * s,
      v.y * s,
      v.z * s,
  };
}

Vec3 vec3_divf(Vec3 v, float s) { return vec3_mulf(v, 1.0 / s); }

Vec3 vec3_norm(Vec3 v) {
  float length = vec3_length(v);
  return vec3_divf(v, length);
}

// void print_mat4(mat4 mat) {
// 	for (int i = 0; i < 4; i++) {
// 		for (int j = 0; j < 4; j++)
// 			printf("%f ", mat[i][j]);
// 		printf("\n");
// 	}
// }

VectorImpl(float, FloatVector) VectorImpl(unsigned int, UnsignedIntVector)
    VectorImpl(uint32_t, UInt32Vector) VectorImpl(size_t, SizeVector)
