#include "camera.h"
#include "mesh.h"
#include "player.h"
#include "world.h"

typedef struct {
	bool *polygon_mode;
	unsigned int shader_program;
	int *width, *height;
} Renderer;

unsigned int render_create_shader(void);
unsigned int render_create_vao(Mesh *mesh);
void pre_render(int width, int height);
void render(Renderer *renderer, struct World *world, Player *player,
			Camera *camera);
