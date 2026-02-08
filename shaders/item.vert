#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

out vec3 frag_pos;
out vec3 frag_color;
out vec3 frag_normal;
out vec2 frag_uv;

uniform mat4 perspective_matrix;
uniform mat4 model;

void main() {
	frag_pos = vec3(model * vec4(position, 1.0));
	frag_color = position;
	frag_normal = transpose(inverse(mat3(model))) * normal;
	frag_uv = uv;

	gl_Position = perspective_matrix * model * vec4(position, 1.0);
}