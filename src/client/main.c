#include <limits.h>

#include "camera.h"
#include "logs.h"
#include "player.h"
#include "render.h"
#include "shader.h"
#include "utils.h"
#include "window.h"
#include "world.h"

int main(void) {
	Window window = createWindow(800, 600);
	if (initWindow(&window) != 0) {
		return -1;
	}

	if (loadGL(glfwGetProcAddress) != 0) {
		windowClose(&window);
		return -1;
	}

	struct World world = world_init();
	Player player = {
		{0.0f, 2.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 5.0f,
		{0.0f, 0.0f, 0.0f},
	};
	Camera camera = {&player.position, &player.direction};
	Renderer renderer = renderer_init(&window);

	render_preparation(window.width, window.height);

	info("The program has been fully initialized. Starting the game loop...");

	double start = 0.0f;
	double end = 0.0f;
	double dt = 0.0f;
	double lastX = window.width / 2.0f;
	double lastY = window.height / 2.0f;
	while (!windowShouldClose(&window)) {
		dt = end - start;
		start = getTime();

		// LOGIC
		windowPollEvents(&window);

		// Mouse rotate camera
		double xpos, ypos;
		getCursorPos(&window, &xpos, &ypos);
		float dx = (xpos - lastX);
		float dy = (ypos - lastY);
		lastX = xpos;
		lastY = ypos;

		camera_update(&camera, window.keys, dx, dy, dt);
		bool player_keys[6] = {
			window.keys[GLFW_KEY_W],
			window.keys[GLFW_KEY_A],
			window.keys[GLFW_KEY_S],
			window.keys[GLFW_KEY_D],
			window.keys[GLFW_KEY_SPACE],
			window.keys[GLFW_KEY_LEFT_SHIFT],
		};
		player_move(&player, player_keys, dt);

		chunks_load_unload_system(&renderer, &world, player.position);

		// RENDER
		render(&renderer, &player, &camera);
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
