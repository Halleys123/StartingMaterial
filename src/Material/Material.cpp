#include "Material/Material.h"

Material::Material() : sampler(nullptr), texture(nullptr), shader(nullptr) {
	sampler = new Sampler();
}

Material::Material(int count, SHADER_CONFIG* shadersList, const char* texturePath, GLenum textureType, GLenum internalFormat, GLenum externalFormat) :	sampler(nullptr), texture(nullptr), shader(nullptr) 
{
	texture = new Texture(texturePath, textureType, internalFormat, externalFormat);
	sampler = new Sampler();
	shader = new Shader(count, shadersList);
}

void Material::SetSamplerValues(GLenum paratmer, GLenum value) {
	sampler->SetValue(paratmer, value);
}

void Material::UseMaterial() {
	texture->Bind(0, *sampler);
	shader->UseProgram();
}

void Material::InitShader(int count, SHADER_CONFIG* shadersList) {
	shader = new Shader(count, shadersList);
}

void Material::InitTexture(const char* texturePath, GLenum textureType, GLenum internalFormat, GLenum externalFormat) {
	texture = new Texture(texturePath, textureType, internalFormat, externalFormat);
}

Shader* Material::getShader() {
	return this->shader;
}