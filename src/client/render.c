#define _GNU_SOURCE
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#include "render.h"
#include "logs.h"
#include "shader.h"

#include "../../cglm/include/cglm/cglm.h"

#define DEFAULT_RENDER_DISTANCE 4

uint32_t render_create_shader(void) {
  uint32_t vertex_shader = compile_shader("shaders/default.vert", GL_VERTEX_SHADER);
  if (!vertex_shader) return 0;

  uint32_t fragment_shader = compile_shader("shaders/default.frag", GL_FRAGMENT_SHADER);
  if (!fragment_shader) return 0;

  uint32_t shader_program = compile_shader_program(vertex_shader, fragment_shader);
  if (!shader_program) return 0;

  // После линкинга в шейдерной программе они больше не нужны
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  return shader_program;
}

uint32_t render_create_vao(const Mesh *mesh) {
  const struct Vertices *vertices = &mesh->vertices;
  const struct UInt32Array *indices = &mesh->indices;

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

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)0);
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

void render(const Renderer *renderer, int width, int height) {
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // glPolygonMode(GL_FRONT_AND_BACK, renderer->polygon_mode ? GL_FILL : GL_LINE);

  mat4 projection;
  glm_perspective_default((float)width / (float)height, projection);
  glm_persp_move_far(projection, renderer->render_distance * CHUNK_SIZE);
  int projection_location = glGetUniformLocation(renderer->shader_program, "projection");

  int model_location = glGetUniformLocation(renderer->shader_program, "model");

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
    const Mesh *mesh = &renderer->meshes.data[i];
    if (!mesh->visible) continue;
    mat4 model;
    vec4 translation = {renderer->camera->position->x + mesh->position.x,
                       -renderer->camera->position->y + mesh->position.y,
                        renderer->camera->position->z + mesh->position.z, 0.0f};
    glm_mat4_identity(model);
    glm_translate(model, translation);
    glUniformMatrix4fv(model_location, 1, GL_FALSE, model[0]);
    glBindVertexArray(renderer->VAOs.data[i]);
    glDrawElements(GL_TRIANGLES, mesh->indices.size,
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

  char *string;
  if (0 > asprintf(&string, "OpenGL loaded successfully. Version: %d", version)) return 0;
  info(string);
  free(string);
  return 0;
}

Renderer renderer_init(const Camera *camera) {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  return (Renderer) {
    .shader_program = render_create_shader(),
    .render_distance = DEFAULT_RENDER_DISTANCE,
    .meshes = {0},
    .VAOs = {0},
    .loaded = {0},
    .should_load = {0},
    .camera = camera,
  };
}

size_t SizeArray_find(struct SizeArray *vec, size_t value) {
  for (size_t i = 0; i < vec->size; i++) {
    if (vec->data[i] == value) {
      return i;
    }
  }
  return SIZE_MAX;
}

void chunks_load_unload_system(Renderer *renderer, struct World *world) {
  Vec3 last_pos = *renderer->camera->position;
  world_getChunkCube(&renderer->should_load, world, last_pos, renderer->render_distance);

  for (size_t i = 0; i < renderer->should_load.size; i++) {
    size_t index = renderer->should_load.data[i];
    pthread_mutex_lock(&world->mutex);
    Chunk *chunk = &world->chunks.data[index];
    if (SizeArray_find(&renderer->loaded, index) == SIZE_MAX) {
      DArray_push(&renderer->meshes, chunk_genmesh(chunk));
      size_t mesh_index = renderer->meshes.size - 1;
      DArray_push(&renderer->VAOs, render_create_vao(&renderer->meshes.data[mesh_index]));
      chunk->mesh_index = mesh_index;
      DArray_push(&renderer->loaded, index);
    }
    pthread_mutex_unlock(&world->mutex);
  }

  for (size_t i = 0; i < renderer->loaded.size; i++) {
    size_t loaded_index = renderer->loaded.data[i];
    pthread_mutex_lock(&world->mutex);
    const Chunk *chunk = &world->chunks.data[loaded_index];
    if (SizeArray_find(&renderer->should_load, loaded_index) == SIZE_MAX) {
      renderer->meshes.data[chunk->mesh_index].visible = false;
    } else {
      renderer->meshes.data[chunk->mesh_index].visible = true;
    }
    pthread_mutex_unlock(&world->mutex);
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
