#define RGFW_EXPORT

#include "logs.h"
#include "player.h"
#include "render.h"

int main(void) {
	MyWindow window = createWindow(800, 600);
	if (initWindow(&window) != 0) {
		return -1;
	}

	if (loadGL((GLADloadfunc)getProcAddress) != 0) {
		windowClose(&window);
		return -1;
	}

	struct World world = world_init();
	Player player = {
		{0.0f, 2.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 5.0f,
		{0.0f, 0.0f, 0.0f},
	};
	Camera camera = {&player.position, &player.direction, &player.speed};
	Renderer renderer = renderer_init(&window, &camera);

	render_preparation(window.width, window.height);

	info("The program has been fully initialized. Starting the game loop...");

	double start = 0.0f;
	double end = 0.0f;
	double dt = 0.0f;
	while (!windowShouldClose(&window)) {
		dt = end - start;
		start = getTime();

		float dx = 0.0f;
		float dy = 0.0f;

		// EVENTS
		while (RGFW_window_checkEvent(window.window)) {
			// Mouse rotate camera
			if (window.window->event.type == RGFW_mousePosChanged) {
				dx = window.window->event.point.x;
				dy = window.window->event.point.y;
			}
		}

		// LOGIC
		camera_update(&camera, window.keys, dx, dy, dt);
		chunks_load_unload_system(&renderer, &world);

		// RENDER
		render(&renderer);
		swapBuffer(&window);

		end = getTime();
	}
	world_free(&world);
	renderer_free(&renderer);
	windowClose(&window);
	info("The program has terminated.");

	return 0;
}

// TODO: optimize Mesh generating. Сейчас мы генерируем по 4 вершины на каждую
// грань вокселя. Это 24 вершины на воксель в худшем случае. Но воксель это куб
// и можно обойтись 8 вершинами
// TODO: write a blazingly fast greedy mesher xd lol
// TODO: world generation using simplex noise
// TODO: improve camera rotation (use quaternions O.O)
// TODO (может быть ненужным и даже вредным):
// 1. написать обёртку над вводом GLFW
// 2. иметь несколько window creating backend. Например GLFW, SDL2/SDL3, RGFW
// или что-то в этом духе
// TODO: подумать над мультиплеером. Будет ли это главный сервер и клиенты, или
// что-то типа P2P?
// TODO: сейчас трудно разглядеть трёхмерность какого-либо чанка - всё
// одноцветное и монотонное. Ранее от текстур я решил отказаться, поэтому
// реализую Ambient occlusion
// TODO: mesh.c looks ugly. Improve it somehow pwease OwO
// TODO: generate chunks in second thread?
// TODO: more types of voxels
