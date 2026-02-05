#include "Mesh/Mesh.h"
#include "glad/glHelp.h"

Mesh::Mesh(int vertexCount, float* vertex, int indexCount, unsigned int* index, GLenum DrawType) {
	VAO.Bind();
	VBO.AddData(vertexCount, vertex, DrawType);
	VBO.Bind();
	IBO.AddData(indexCount, index, DrawType);
	IBO.Bind();
	VAO.Unbind();
	VBO.Unbind();
	IBO.Unbind();
}

void Mesh::Draw(GLenum ShapeToDraw, GLenum DataType) {
	VAO.Bind();
	IBO.Bind();
	glCall(glDrawElements(ShapeToDraw, IBO.getCount(), DataType, 0));
	this->VBO.Unbind();
	IBO.Unbind();
	VAO.Unbind();
}

void Mesh::setVertexAttribPointer(unsigned int location, int elementCount, GLenum dataType, GLenum shouldNormalize, int stride, const void* StartFrom) {
	VAO.setVertexAttribPointer(VBO, location, elementCount, dataType, shouldNormalize, stride, StartFrom);
}