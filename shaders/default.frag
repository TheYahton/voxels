#version 330 core

in vec3 vertexColor;
in vec3 Normal;

out vec4 color;

const vec3 lightColor = vec3(1.0f);
const vec3 lightDir = normalize(vec3(1.0f, 0.9f, 0.6f));

void main()
{
	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * lightColor;
	vec3 norm = normalize(Normal);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	vec3 result = (ambient + diffuse) * vertexColor;
	color = vec4(result, 1.0f);
}
