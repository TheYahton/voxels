#include <GLFW/glfw3.h>
#include <stdbool.h>

#include "logs.h"
#include "shader.h"
#include "window.h"

void swapBuffer(Window *window) { glfwSwapBuffers(window->window); }

bool windowShouldClose(Window *window) {
	return glfwWindowShouldClose(window->window);
}

void getCursorPos(Window *window, double *xpos, double *ypos) {
	glfwGetCursorPos(window->window, xpos, ypos);
}

static void key_callback(GLFWwindow *window, int key,
						 int scancode __attribute__((unused)), int action,
						 int mode __attribute__((unused))) {
	Window *my_window = glfwGetWindowUserPointer(window);

	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			my_window->keys[key] = true;
		} else if (action == GLFW_RELEASE) {
			my_window->keys[key] = false;
		}
	}

	if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	} else if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		my_window->polygon_mode = !my_window->polygon_mode;
	}
}

static void resize_callback(GLFWwindow *window, int new_width, int new_height) {
	Window *my_window = glfwGetWindowUserPointer(window);
	my_window->width = new_width;
	my_window->height = new_height;
	glViewport(0, 0, new_width, new_height);
}

Window createWindow(int width, int height) {
	return (Window){NULL, width, height, {false}, false};
}

int initWindow(Window *window) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, true);
	info("GLFW был инициализирован");

	window->window =
		glfwCreateWindow(window->width, window->height, "Voxels", NULL, NULL);
	if (window->window == NULL) {
		error("Не удалось создать GLFW окно");
		return -1;
	}
	info("Окно было инициализировано");

	glfwSetWindowUserPointer(window->window, window);

	glfwGetFramebufferSize(window->window, &window->width, &window->height);

	glfwSetInputMode(window->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	info("Курсор был выключен");

	if (glfwRawMouseMotionSupported()) {
		glfwSetInputMode(window->window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		info("Включен параметр raw mouse motion");
	} else {
		warning("Система не поддерживает raw mouse motion");
	}

	glfwMakeContextCurrent(window->window);
	info("Контекст OpenGL был привязан к окну");

	glfwSetKeyCallback(window->window, key_callback);
	info("Функция key_callback отлавливает пользовательский ввод");

	glfwSetFramebufferSizeCallback(window->window, resize_callback);
	info("Функция resize_callback отлавливает изменения размера окна");

	return 0;
}
