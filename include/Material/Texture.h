#pragma once

#include "glad/glad.h"
#include "Material/Sampler.h"

class Texture {
private:
	unsigned int texture;
	GLenum textureType;
public:
	Texture(const char* texturePath, GLenum textureType, GLenum internalFormat, GLenum externalFormat);
	void Bind(unsigned int TextureUnit, Sampler& sampler);
};