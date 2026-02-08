#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"

enum SHADER_STAGE {
	VERTEX = 0,
	TSCTRL = 1,
	TSEVAL = 2,
	GMETRY = 3,
	FRAGMT = 4,
	CMPUTE = 5
};

typedef struct SHADER_CONFIG {
	SHADER_STAGE type;
	const char* shaderPath;
} SHADER_CONFIG;

class Shader {
private:	
	// Internal use to convert SHADER_TYPE to OPENGL_SHADER
	static GLenum OPENGL_TYPES[6];

	// Other use case
	unsigned int ShaderProgram;
	unsigned int ShaderIDs[6] = { 0 }; // Used for saving vertex, fragment shader for compilation
	bool loadedShaders[6] = { false };
private:
	void LoadShader(const char* src, SHADER_STAGE TYPE);
	void LinkAllShaders();
public:
	Shader(int count, SHADER_CONFIG* shadersList);
	void UseProgram();
	void ModifyUniform(const char* UnfiromName, glm::mat4& matrix);
	void ModifyUniform(const char* UniformName, glm::vec3& value);
	void ModifyUniform(const char* UniformName, float x, float y, float z);
	void ModifyUniformArray(const char* UniformName, int count, float* values);
	void ModifyUniform1fArray(const char* UniformName, int count, float value[]);
	void ModifyUniform(const char* UniformName, int value);
	void ModifyUniform(const char* UniformName, float value);
};
