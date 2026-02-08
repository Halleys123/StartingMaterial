#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

out vec3 frag_pos;
out vec3 frag_color;
out vec3 frag_normal;
out vec2 frag_uv;

uniform mat4 perspective_matrix;

void main() {
	frag_pos = position;
	frag_color = position;
	frag_normal =  normal;
	frag_uv = uv;

	gl_Position = perspective_matrix * vec4(position, 1.0);
}