#version 330 core

in vec3 vertexColor;
in vec3 Normal;

out vec4 color;

const vec3 lightColor = vec3(1.0f, 0.98f, 0.75f);
const vec3 lightDir = normalize(vec3(1.0f, 0.9f, 0.6f));
const vec3 ambientColor = vec3(0.8f, 0.8f, 1.0f);
const float ambientStrength = 0.5f;

void main() {
  vec3 ambient = ambientStrength * ambientColor;
  vec3 norm = normalize(Normal);
  float diff = max(dot(norm, lightDir), 0.0f);
  vec3 diffuse = diff * lightColor;

  vec3 result = (ambient + diffuse) * vertexColor;
  color = vec4(result, 1.0f);
}
