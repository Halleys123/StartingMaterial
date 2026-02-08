#version 430 core

layout (location = 0) in vec3 position;

out vec3 frag_pos;

uniform mat4 perspective_matrix;
uniform mat4 model;
uniform float distanceFromCam;

void main() {
	gl_Position = perspective_matrix * model * vec4(position, 1.0);
}