#pragma once

#include "Mesh/IndexBuffer.h"
#include "Mesh/VertexBuffer.h"
#include "Mesh/VertexArray.h"

class Mesh {
private:

	VertexArray VAO;
	IndexBuffer IBO;
	VertexBuffer VBO;

public:
	Mesh(int vertexCount, float* vertex, int indexCount, unsigned int* index, GLenum DrawType);
	void Draw(GLenum ShapeToDraw, GLenum DataType);
	void setVertexAttribPointer(unsigned int location, int elementCount, GLenum dataType, GLenum shouldNormalize, int stride, const void* StartFrom);
};