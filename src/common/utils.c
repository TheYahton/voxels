#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

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

Vec3i vec3i_muli(Vec3i v, int s) {
  return (Vec3i){
      v.x * s,
      v.y * s,
      v.z * s,
  };
}
