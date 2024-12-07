#ifndef _GLAD_H
#define _GLAD_H
#include <glad/gl.h>
#endif // _GLAD_H

#include "camera.h"
#include "mesh.h"
#include "player.h"
#include <stdbool.h>

typedef struct {
	bool *polygon_mode;		 // 1 byte
	uint32_t shader_program; // 4 byte
	int *width, *height;	 // 8 + 8 = 16 bytes
	unsigned int render_distance;  // 4 bytes
} Renderer;

uint32_t render_create_shader(void);
uint32_t render_create_vao(Mesh *mesh);
void pre_render(int width, int height);
void render(Renderer *renderer, const MeshVector *meshes,
			const UInt32Vector *VAOs, Player *player, Camera *camera);
int loadGL(GLADloadfunc func);
