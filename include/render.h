#ifndef RENDER_H
#define RENDER_H

#include "camera.h"
#include "mesh.h"
#include "world.h"

typedef struct {
  uint32_t shader_program;
  unsigned int render_distance;
  struct MeshArray meshes;
  struct UInt32Array VAOs;
  struct SizeArray loaded;
  struct SizeArray should_load;
  const Camera *camera;
} Renderer;

void chunks_load_unload_system(Renderer *renderer, struct World *world);
uint32_t render_create_shader(void);
uint32_t render_create_vao(const Mesh *mesh);
Renderer renderer_init(const Camera *camera);
void renderer_free(Renderer *renderer);
void render(const Renderer *renderer, int width, int height);
int loadGL(GLADloadfunc func);

#endif /* RENDER_H */
