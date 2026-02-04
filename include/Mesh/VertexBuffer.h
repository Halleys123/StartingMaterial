#pragma once

#include "glad/glad.h"

class VertexBuffer {
private:
	unsigned int VBO;
public:
	VertexBuffer(int count, int size, float* data, GLenum DrawType);
};