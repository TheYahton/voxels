#define GLAD_GL_IMPLEMENTATION

#include "render.h"
#include "logs.h"
#include "shader.h"
#include <cglm/cglm.h>

uint32_t render_create_shader(void) {
	uint32_t vertex_shader =
		compile_shader("shaders/default.vert", GL_VERTEX_SHADER);
	if (!vertex_shader) {
		return 0;
	}

	uint32_t fragment_shader =
		compile_shader("shaders/default.frag", GL_FRAGMENT_SHADER);
	if (!fragment_shader) {
		return 0;
	}

	uint32_t shader_program =
		compile_shader_program(vertex_shader, fragment_shader);
	if (!shader_program) {
		return 0;
	}

	// После линкинга в шейдерной программе они больше не нужны
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return shader_program;
}

uint32_t render_create_vao(const Mesh *mesh) {
	const Vertices *vertices = &mesh->vertices;
	const UInt32Vector *indices = &mesh->indices;

	// WARNING: A DUNGER STARTS!
	uint32_t VAO; // Vertex Attribute Object(?)
	uint32_t VBO; // Vertex Buffer Object
	uint32_t EBO; // Element (Index) Buffer Object

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices->capacity * sizeof(Vertex),
				 vertices->data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->capacity * sizeof(GLuint),
				 indices->data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
						  (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(Vertex),
						  (GLvoid *)(3 * 4));
	glEnableVertexAttribArray(1);

	glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(Vertex),
						  (GLvoid *)(4 * 4));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	// WARNING: THE DUNGER ENDS!

	return VAO;
}

void render_preparation(int width, int height) {
	glViewport(0, 0, width, height);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void render(const Renderer *renderer) {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (*renderer->polygon_mode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	mat4 projection;
	glm_perspective_default((float)*renderer->width / (float)*renderer->height,
							projection);
	glm_persp_move_far(projection, renderer->render_distance * CHUNK_SIZE);
	int projection_location =
		glGetUniformLocation(renderer->shader_program, "projection");

	int model_location =
		glGetUniformLocation(renderer->shader_program, "model");

	mat4 view;
	glm_mat4_identity(view);
	vec3 up = {1.0f, 0.0f, 0.0f};
	glm_rotate(view, -renderer->camera->direction->y, up);
	vec3 left = {0.0f, 1.0f, 0.0f};
	glm_rotate(view, -renderer->camera->direction->x, left);
	int view_location = glGetUniformLocation(renderer->shader_program, "view");

	glUseProgram(renderer->shader_program);
	glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection[0]);
	glUniformMatrix4fv(view_location, 1, GL_FALSE, view[0]);

	for (size_t i = 0; i < renderer->VAOs.size; i++) {
		if (!renderer->meshes.data[i].visible)
			continue;
		mat4 model;
		vec4 translation = {renderer->camera->position->x, -renderer->camera->position->y,
							renderer->camera->position->z, 0.0f};
		glm_mat4_identity(model);
		glm_translate(model, translation);
		glUniformMatrix4fv(model_location, 1, GL_FALSE, model[0]);
		glBindVertexArray(renderer->VAOs.data[i]);
		glDrawElements(GL_TRIANGLES, renderer->meshes.data[i].indices.size,
					   GL_UNSIGNED_INT,
					   0); // With EBO

		glBindVertexArray(0);
	}
}

int loadGL(GLADloadfunc func) {
	int version = gladLoadGL(func);
	if (version == 0) {
		error("Failed to initialize OpenGL context.");
		return -1;
	}
	info("OpenGL loaded successfully.");
	return 0;
}

#define RENDER_DISTANCE 4

Renderer renderer_init(const Window *window, const Camera *camera) {
	Renderer renderer = {
		&window->polygon_mode,
		render_create_shader(),
		&window->width,
		&window->height,
		RENDER_DISTANCE,
		MeshVector_init(0, 64),
		UInt32Vector_init(0, 64),
		SizeVector_init(0, 64),
		SizeVector_init(0, 64),
		camera,
	};

	return renderer;
}

size_t SizeVector_find(SizeVector *vec, size_t value) {
	for (size_t i = 0; i < vec->size; i++) {
		if (vec->data[i] == value) {
			return i;
		}
	}
	return SIZE_MAX;
}

void chunks_load_unload_system(Renderer *renderer, struct World *world) {
		world_chunk_cube(&renderer->should_load, world, *renderer->camera->position, RENDER_DISTANCE);

		for (size_t i = 0; i < renderer->should_load.size; i++) {
			size_t index = renderer->should_load.data[i];
			Chunk *chunk = &world->chunks.data[index];
			if (SizeVector_find(&renderer->loaded, index) == SIZE_MAX) {
				MeshVector_append(&renderer->meshes, chunk_genmesh(chunk, world));
				size_t mesh_index = renderer->meshes.size - 1;
				UInt32Vector_append(
					&renderer->VAOs, render_create_vao(&renderer->meshes.data[mesh_index]));
				SizeVector_append(&renderer->loaded, index);
				chunk->mesh_index = mesh_index;
			}
		}

		for (size_t i = 0; i < renderer->loaded.size; i++) {
			size_t loaded_index = renderer->loaded.data[i];
			const Chunk *chunk = &world->chunks.data[loaded_index];
			if (SizeVector_find(&renderer->should_load, loaded_index) == SIZE_MAX) {
				renderer->meshes.data[chunk->mesh_index].visible = false;
			} else {
				renderer->meshes.data[chunk->mesh_index].visible = true;
			}
		}

		renderer->should_load.size = 0;

}

void renderer_free(Renderer *renderer) {
	for (size_t i = 0; i < renderer->meshes.size; i++) {
		free(renderer->meshes.data[i].vertices.data);
		free(renderer->meshes.data[i].indices.data);
	}
	free(renderer->meshes.data);
	free(renderer->should_load.data);
	free(renderer->loaded.data);
}
