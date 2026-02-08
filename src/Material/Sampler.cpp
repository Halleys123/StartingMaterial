#include "Material/Sampler.h"
#include "glad/glHelp.h"

Sampler::Sampler() {
	glCall(glGenSamplers(1, &sampler))
}

void Sampler::SetValue(GLenum param, GLenum value) {
	glCall(glSamplerParameteri(sampler, param, value));
}

void Sampler::Bind(unsigned int TextureUnit) {
	glBindSampler(TextureUnit, sampler);
}