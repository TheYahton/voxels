#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "camera.h"
#include "chunk.h"
#include "logs.h"
#include "mesh.h"
#include "player.h"
#include "render.h"
#include "shader.h"
#include "utils.h"
#include "window.h"

int main() {
	Window window = createWindow(800, 600);
	if (initWindow(&window) != 0) {
		error("Окно не было инициализировано ,_,");
		glfwTerminate();
		return -1;
	}
	info("Окно было инициализировано");

	if (!glewInit()) {
		error("Не удалось инициализировать GLEW");
		glfwTerminate();
		return -1;
	}
	info("GLEW был инициализирован");

	unsigned int shader_program = render_create_shader();

	Chunk chunk = chunk_init();
	chunk_worldgen(chunk);
	Mesh mesh = chunk_genmesh(chunk);
	GLuint VAO = render_create_vao(mesh, window.width, window.height);
	mesh_free(mesh);
	chunk_free(chunk);

	Player player = {
		{0.0f, 1.0f, -5.0f},
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f},
		3.0f,
	};
	Camera camera = {
		0.0f,
		0.0f,
		0.0f,
	};
	Renderer renderer = {
		&window.polygon_mode, shader_program, VAO,
		&window.width,		  &window.height,
	};

	info("Программа была полностью инициализирована. Запуск игрового цикла...");

	float start = 0.0f;
	float end = 0.0f;
	float dt = 0.0f;
	double lastX = window.width / 2.0f;
	double lastY = window.height / 2.0f;
	while (!windowShouldClose(&window)) {
		dt = end - start;
		start = glfwGetTime();

		// LOGIC
		glfwPollEvents();

		// Mouse rotate camera
		double xpos, ypos;
		getCursorPos(&window, &xpos, &ypos);
		float dx = (xpos - lastX);
		float dy = (ypos - lastY);
		lastX = xpos;
		lastY = ypos;

		camera_update(&camera, window.keys, dx, dy, dt);
		player_move(&player, window.keys, camera.direction, dt);

		// RENDER
		render(&renderer, &mesh, &player, &camera);
		swapBuffer(&window);

		end = glfwGetTime();
	}
	glfwTerminate();
	info("Программа завершена");

	return 0;
}

// TODO: optimize Mesh generating. Сейчас мы генерируем по 4 вершины на каждую
// грань вокселя. Это 24 вершины на воксель в худшем случае. Но воксель это куб
// и можно обойтись 8 вершинами
// TODO: add ability to change color of voxel.
// TODO: write a blazingly fast greedy mesher xd lol
// TODO: a system of loading-unloading chunks depending on the distance from the
// player
// TODO: world generation
// TODO: improve camera rotation (use quaternions O.O)
