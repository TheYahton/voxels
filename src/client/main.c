#include <limits.h>

#include "camera.h"
#include "logs.h"
#include "player.h"
#include "render.h"
#include "shader.h"
#include "utils.h"
#include "window.h"
#include "world.h"

#include <stdlib.h>

#define RENDER_DISTANCE 4

size_t SizeVector_find(SizeVector *vec, size_t value) {
	for (size_t i = 0; i < vec->size; i++) {
		if (vec->data[i] == value) {
			return i;
		}
	}
	return SIZE_MAX;
}

int main(void) {
	Window window = createWindow(800, 600);
	if (initWindow(&window) != 0) {
		return -1;
	}

	if (loadGL(glfwGetProcAddress) != 0) {
		windowClose(&window);
		return -1;
	}

	uint32_t shader_program = render_create_shader();

	struct World world = world_init();
	MeshVector meshes = MeshVector_init(0, 64);
	UInt32Vector VAOs = UInt32Vector_init(0, 64);
	SizeVector loaded_chunks = SizeVector_init(0, 64);
	SizeVector should_load = SizeVector_init(0, 64);

	Player player = {
		{0.0f, 2.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 5.0f,
		{0.0f, 0.0f, 0.0f},
	};
	Camera camera = {&player.position, &player.direction};
	Renderer renderer = {
		&window.polygon_mode,
		shader_program,
		&window.width,
		&window.height,
		RENDER_DISTANCE,
	};

	pre_render(window.width, window.height);

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

		world_chunk_circle(&should_load, &world, player.position.x,
						   player.position.y, player.position.z, RENDER_DISTANCE);

		for (size_t i = 0; i < should_load.size; i++) {
			size_t index = should_load.data[i];
			Chunk *chunk = &world.chunks.data[index];
			if (SizeVector_find(&loaded_chunks, index) == SIZE_MAX) {
				MeshVector_append(&meshes, chunk_genmesh(chunk, &world));
				size_t mesh_index = meshes.size - 1;
				UInt32Vector_append(
					&VAOs, render_create_vao(&meshes.data[mesh_index]));
				SizeVector_append(&loaded_chunks, index);
				chunk->mesh_index = mesh_index;
			}
		}

		for (size_t i = 0; i < loaded_chunks.size; i++) {
			size_t loaded_index = loaded_chunks.data[i];
			const Chunk *chunk = &world.chunks.data[loaded_index];
			if (SizeVector_find(&should_load, loaded_index) == SIZE_MAX) {
				meshes.data[chunk->mesh_index].visible = false;
			} else {
				meshes.data[chunk->mesh_index].visible = true;
			}
		}

		should_load.size = 0;

		// RENDER
		render(&renderer, &meshes, &VAOs, &player, &camera);
		swapBuffer(&window);

		end = getTime();
	}
	world_free(&world);
	for (size_t i = 0; i < meshes.size; i++) {
		free(meshes.data[i].vertices.data);
		free(meshes.data[i].indices.data);
	}
	free(meshes.data);
	free(should_load.data);
	free(loaded_chunks.data);
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
// TODO: separate chunks load-unload system into function
// TODO: generate chunks in second thread?
// TODO: more types of voxels
