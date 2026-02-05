#include "Mesh/IndexBuffer.h"

IndexBuffer::IndexBuffer() : count(0), IndexBufferID(0) {}
IndexBuffer::IndexBuffer(int count, unsigned int* data, GLenum DrawType) {
	this->AddData(count, data, DrawType);
}

void IndexBuffer::AddData(int count, unsigned int* data, GLenum DrawType) {
	glCall(glGenBuffers(1, &this->IndexBufferID));
	glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IndexBufferID);
	glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * count, data, DrawType)));
	//glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

	this->count = count;
}


void IndexBuffer::Bind() {
	glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IndexBufferID));
}

void IndexBuffer::Unbind() {
	glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

int IndexBuffer::getCount() {
	return this->count;
}