#ifndef _mesh
#define _mesh

#include "utils.h"

typedef struct {
	FloatVector vertices;
	UnsignedIntVector indices;
} Mesh;

void mesh_free(Mesh *mesh);

#endif // _mesh
