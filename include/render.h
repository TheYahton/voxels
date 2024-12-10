#ifndef _GLAD_H
#define _GLAD_H
#include <glad/gl.h>
#endif // _GLAD_H

#define RGFW_EXPORT
#include "camera.h"
#include "mesh.h"
#include "window.h"
#include <stdbool.h>

typedef struct {
	const bool *polygon_mode;
	uint32_t shader_program;
	const int *width, *height;
	unsigned int render_distance;
	MeshVector meshes;
	UInt32Vector VAOs;
	SizeVector loaded; // chunks
	SizeVector should_load; // chunks
	const Camera *camera;
} Renderer;

void chunks_load_unload_system(Renderer *renderer, struct World *world);
uint32_t render_create_shader(void);
uint32_t render_create_vao(const Mesh *mesh);
void render_preparation(int width, int height);
Renderer renderer_init(const MyWindow *window, const Camera *camera);
void renderer_free(Renderer *renderer);
void render(const Renderer *renderer);
int loadGL(GLADloadfunc func);
