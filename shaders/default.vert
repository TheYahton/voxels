#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in uint normal;

out vec3 vertexColor;
out vec3 Normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

const vec3 NORMALS[6] = vec3[](
    vec3( 1.0f,  0.0f,  0.0f),
    vec3(-1.0f,  0.0f,  0.0f),
    vec3( 0.0f,  1.0f,  0.0f),
    vec3( 0.0f, -1.0f,  0.0f),
    vec3( 0.0f,  0.0f,  1.0f),
    vec3( 0.0f,  0.0f, -1.0f)
);

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    vertexColor = color;
    Normal = NORMALS[normal];
}
