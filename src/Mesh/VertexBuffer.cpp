#include "Mesh/VertexBuffer.h"
#include "glad/glHelp.h"

VertexBuffer::VertexBuffer(int count, int size, float* array, GLenum DrawType) {
	glCall(glGenBuffers(1, &this->VBO));
	glCall(glBindBuffer(GL_ARRAY_BUFFER, this->VBO));
	glCall(glBufferData(GL_ARRAY_BUFFER, size, array, DrawType));
	glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}