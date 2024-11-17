#ifndef _MESH_H
#define _MESH_H

#include "utils.h"

typedef struct {
	FloatVector vertices;	   // 20 bytes
	UnsignedIntVector indices; // 20 bytes
} Mesh;						   // 40 bytes

void mesh_free(Mesh *mesh);

#endif // _MESH_H
