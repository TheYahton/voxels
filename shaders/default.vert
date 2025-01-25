#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in uint voxelType;
layout(location = 2) in uint normal;

out vec3 vertexColor;
out vec3 Normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

const vec3 NORMALS[6] = vec3[](vec3(1.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f),
                               vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f),
                               vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f));

const vec3 COLORS[] = vec3[](vec3(0.0f, 0.0f, 0.0f),    // 0 Air
                             vec3(0.5f, 0.5f, 0.5f),    // 1 Stone
                             vec3(0.25f, 0.16f, 0.02f), // 2 Dirt
                             vec3(0.1f, 0.5f, 0.1f),    // 3 Grass
                             vec3(0.92f, 0.93f, 0.67f), // 4 Sand
                             vec3(0.08f, 0.02f, 0.93f)  // 5 Water
);

void main() {
  gl_Position = projection * view * model * vec4(position, 1.0);
  vertexColor = COLORS[voxelType];
  Normal = NORMALS[normal];
}
