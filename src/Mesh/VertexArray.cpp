#include "Mesh/VertexArray.h"
#include "glad/glHelp.h"

VertexArray::VertexArray() {
	glCall(glGenVertexArrays(1, &this->VAO));
	this->Bind();
}

void VertexArray::Bind() {
	glCall(glBindVertexArray(this->VAO));
}

void VertexArray::Unbind() {
	glCall(glBindVertexArray(0));
}


void VertexArray::setVertexAttribPointer(VertexBuffer& VertexBufferObject, unsigned int location, int elementCount, GLenum dataType, GLboolean shouldNormalize, int stride, const void* pointer) {
	Bind();
	VertexBufferObject.Bind();
	glCall(glEnableVertexAttribArray(location));
	glCall(glVertexAttribPointer(location, elementCount, dataType, shouldNormalize, stride, pointer));
	Unbind();
	VertexBufferObject.Unbind();
}