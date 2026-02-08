#pragma once

#include "glad/glad.h"

#include "Material/Shader.h"
#include "Material/Texture.h"
#include "Material/Sampler.h"

class Material {
private:
	Shader* shader = nullptr;
	Texture* texture = nullptr;
	Sampler* sampler = nullptr;
public:
	Material();
	Material(int count, SHADER_CONFIG* shadersList, const char* texturePath, GLenum textureType, GLenum internalFormat, GLenum externalFormat);
	void InitShader(int count, SHADER_CONFIG* shadersList);
	void InitTexture(const char* texturePath, GLenum textureType, GLenum internalFormat, GLenum externalFormat);
	void SetSamplerValues(GLenum paratmer, GLenum value);
	void UseMaterial();
	Shader* getShader();
};