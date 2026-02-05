#pragma once

#include "glad/glad.h"
#include "glad/glHelp.h"

class IndexBuffer {
private:
	unsigned int IndexBufferID;
	int count;
public:
	IndexBuffer();
	IndexBuffer(int count, unsigned int* data, GLenum DrawType);
	void AddData(int count, unsigned int* data, GLenum DrawType);
	void Bind();
	void Unbind();
	int getCount();
};