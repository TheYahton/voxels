#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#include "logs.h"

char *read_file(const char *path) {
	FILE *fptr = fopen(path, "r");
	if (fptr == NULL) {
		return NULL;
	}

	fseek(fptr, 0L, SEEK_END);
	long length = ftell(fptr);
	fseek(fptr, 0L, SEEK_SET);

	char *buffer = (char *)malloc(length + 1); // + 1 for null terminating
	if (!buffer) {
		return NULL;
	}
	fread(buffer, 1, length, fptr);
	fclose(fptr);

	buffer[length] = '\0';

	return buffer;
}

GLuint compile_shader(const char *path, GLenum shader_type) {
	const char *shader_source = read_file(path);
	if (!shader_source) {
		char *string;
		if (0 >
			asprintf(
				&string,
				"Shader compilation failed. The file \"%s\" does not exist.",
				path))
			return 0;
		error(string);
		free(string);
		return 0;
	}

	GLuint shader = glCreateShader(shader_type);
	glShaderSource(shader, 1, &shader_source, NULL);
	glCompileShader(shader);

	free((char *)shader_source);

	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		char *string;
		if (0 > asprintf(&string, "Shader compilation for %s failed.\n%s", path,
						 infoLog))
			return 0;
		error(string);
		free(string);
		return 0;
	}

	char *string;
	if (0 > asprintf(&string, "Shader compilation for %s succeeded. ID: %d",
					 path, shader))
		return 0;
	info(string);
	free(string);

	return shader;
}

GLuint compile_shader_program(GLuint vertex_shader, GLuint fragment_shader) {
	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	GLint success;
	GLchar infoLog[512];
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader_program, 512, NULL, infoLog);
		char *string;
		if (0 > asprintf(&string, "Shader program compilation failed.\n%s",
						 infoLog))
			return 0;
		error(string);
		free(string);
		return 0;
	}

	char *string;
	if (0 > asprintf(&string, "Shader program compilation succeeded. ID: %d",
					 shader_program))
		return 0;
	info(string);
	free(string);

	return shader_program;
}
