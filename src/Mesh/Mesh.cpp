#include "Mesh/Mesh.h"
#include "glad/glHelp.h"

Mesh::Mesh() : VertexCount(0), UseElements(false) {
}

Mesh::Mesh(int vertexCount, float* vertex, int indexCount, unsigned int* index, GLenum DrawType) : VertexCount(vertexCount), UseElements(false) {
	Init(vertexCount, vertex, indexCount, index, DrawType);
}

void Mesh::Init(int vertexCount, float* vertex, int indexCount, unsigned int* index, GLenum DrawType) {
	VertexCount = vertexCount;

	VAO.Bind();
	VBO.AddData(vertexCount, vertex, DrawType);
	VBO.Bind();
	if (indexCount && index) {
		UseElements = true;
		IBO.AddData(indexCount, index, DrawType);
	}
	IBO.Bind();
	VAO.Unbind();
	VBO.Unbind();
	if (indexCount && index) {
		IBO.Unbind();
	}
}

void Mesh::Draw(GLenum ShapeToDraw, GLenum DataType) {
	if (UseElements) {
		DrawElements(ShapeToDraw, DataType);
	} else 
		DrawArray(ShapeToDraw, DataType);
}

void Mesh::DrawElements(GLenum ShapeToDraw, GLenum DataType) {
	VAO.Bind();
	IBO.Bind();
	glCall(glDrawElements(ShapeToDraw, IBO.getCount(), DataType, 0));
	this->VBO.Unbind();
	IBO.Unbind();
	VAO.Unbind();
}

void Mesh::DrawArray(GLenum ShapeToDraw, GLenum DataType) {
	VAO.Bind();
	glCall(glDrawArrays(ShapeToDraw, 0, VertexCount / 3));
	this->VBO.Unbind();
	VAO.Unbind();
}

void Mesh::setVertexAttribPointer(unsigned int location, int elementCount, GLenum dataType, GLenum shouldNormalize, int stride, const void* StartFrom) {
	VAO.setVertexAttribPointer(VBO, location, elementCount, dataType, shouldNormalize, stride, StartFrom);
}