#define STB_IMAGE_IMPLEMENTATION

#include "Material/Texture.h"
#include "glad/glHelp.h"
#include "stb_image.h"

Texture::Texture(const char* texturePath, GLenum textureType, GLenum internalFormat, GLenum externalFormat) : textureType(textureType) {
	if (!texturePath) {
		OutputDebugString("No texture path provided\n");
		__debugbreak();
	}

	int x, y, c;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* image = stbi_load(texturePath, &x, &y, &c, 0);

	glCall(glGenTextures(1, &texture));
	glCall(glBindTexture(textureType, texture));
	glCall(glTexImage2D(textureType, 0, internalFormat, x, y, 0, externalFormat, GL_UNSIGNED_BYTE, image));
	glGenerateMipmap(GL_TEXTURE_2D);
	glCall(glBindTexture(textureType, 0));

	stbi_image_free(image);
}

void Texture::Bind(unsigned int TextureUnit, Sampler& sampler) {
	glCall(glActiveTexture(GL_TEXTURE0 + TextureUnit));
	sampler.Bind(TextureUnit);
	glCall(glBindTexture(textureType, texture));
}