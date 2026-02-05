#pragma once

#include "Mesh/VertexBuffer.h"
#include "glad/glad.h"

class VertexArray {
private:
	unsigned int VAO;
public:
	VertexArray();
	void Bind();
	void Unbind();
	void setVertexAttribPointer(VertexBuffer& VertexBufferObject, unsigned int location, int elementCount, GLenum dataType, GLboolean shouldNormalize, int stride, const void* StartFrom);
};