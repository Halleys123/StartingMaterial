#pragma once

#include "glad/glad.h"

class Sampler {
private:
	unsigned int sampler;
public:
	Sampler();
	void SetValue(GLenum paratmer, GLenum value);
	void Bind(unsigned int TextureUnit = 0);
};