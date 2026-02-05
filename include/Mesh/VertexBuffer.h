#pragma once

#include "glad/glad.h"

class VertexBuffer {
private:
	unsigned int VertexBufferID;
	int count;
public:
	VertexBuffer();
	VertexBuffer(int count, float* data, GLenum DrawType);
	void AddData(int count, float* data, GLenum DrawType);
	unsigned int getVertexBufferID();
	void Unbind();
	void Bind();
	int getCount();
};