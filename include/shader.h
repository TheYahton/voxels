#include <stdint.h>

uint32_t compile_shader(const char *path, uint32_t shader_type);
uint32_t compile_shader_program(uint32_t vertex_shader,
								uint32_t fragment_shader);
