#ifndef _GLAD_H
#define _GLAD_H
#include <glad/gl.h>
#endif // _GLAD_H

#include "camera.h"
#include "mesh.h"
#include "window.h"
#include <stdbool.h>

typedef struct {
	const bool *polygon_mode;		 // 1 byte
	uint32_t shader_program; // 4 byte
	const int *width, *height;	 // 8 + 8 = 16 bytes
	unsigned int render_distance;  // 4 bytes
	MeshVector meshes;
	UInt32Vector VAOs;
	SizeVector loaded; // chunks
	SizeVector should_load; // chunks
} Renderer;

void chunks_load_unload_system(Renderer *renderer, struct World *world, const Camera *camera);
uint32_t render_create_shader(void);
uint32_t render_create_vao(const Mesh *mesh);
void render_preparation(int width, int height);
Renderer renderer_init(const Window *window);
void renderer_free(Renderer *renderer);
void render(const Renderer *renderer, Camera *camera);
int loadGL(GLADloadfunc func);
