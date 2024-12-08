#ifndef _GLAD_H
#define _GLAD_H
#include <glad/gl.h>
#endif // _GLAD_H

#include "camera.h"
#include "mesh.h"
#include "player.h"
#include "window.h"
#include <stdbool.h>

typedef struct {
	bool *polygon_mode;		 // 1 byte
	uint32_t shader_program; // 4 byte
	int *width, *height;	 // 8 + 8 = 16 bytes
	unsigned int render_distance;  // 4 bytes
	MeshVector meshes;
	UInt32Vector VAOs;
	SizeVector loaded; // chunks
	SizeVector should_load; // chunks
} Renderer;

void chunks_load_unload_system(Renderer *renderer, struct World *world, Vec3 around);
uint32_t render_create_shader(void);
uint32_t render_create_vao(Mesh *mesh);
void render_preparation(int width, int height);
Renderer renderer_init(Window *window);
void renderer_free(Renderer *renderer);
void render(Renderer *renderer, Player *player, Camera *camera);
int loadGL(GLADloadfunc func);
