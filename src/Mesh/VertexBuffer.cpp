#include "Mesh/VertexBuffer.h"
#include "glad/glHelp.h"

VertexBuffer::VertexBuffer() : count(0), VertexBufferID(0)  {}

VertexBuffer::VertexBuffer(int count, float* array, GLenum DrawType) {
	this->AddData(count, array, DrawType);
}

void VertexBuffer::AddData(int count, float* array, GLenum DrawType) {
	glCall(glGenBuffers(1, &this->VertexBufferID));
	glCall(glBindBuffer(GL_ARRAY_BUFFER, this->VertexBufferID));
	glCall(glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), array, DrawType));
	glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));

	this->count = count;
}

void VertexBuffer::Bind() {
	glBindBuffer(GL_ARRAY_BUFFER, this->VertexBufferID);
}

void VertexBuffer::Unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

unsigned int VertexBuffer::getVertexBufferID() {
	return VertexBufferID;
}