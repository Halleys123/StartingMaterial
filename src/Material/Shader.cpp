#include "Material/Shader.h"
#include "glad/glad.h"
#include "glad/glHelp.h"

#include <stdio.h>
#include <Windows.h>

GLenum Shader::OPENGL_TYPES[6] = {
	GL_VERTEX_SHADER,           // Matches SHADER_STAGE::VERTEX (0)
	GL_TESS_CONTROL_SHADER,     // Matches SHADER_STAGE::TSCTRL (1)
	GL_TESS_EVALUATION_SHADER,  // Matches SHADER_STAGE::TSEVAL (2)
	GL_GEOMETRY_SHADER,         // Matches SHADER_STAGE::GMETRY (3)
	GL_FRAGMENT_SHADER,         // Matches SHADER_STAGE::FRAGMT (4)
	GL_COMPUTE_SHADER           // Matches SHADER_STAGE::CMPUTE (5)
};

Shader::Shader(int count, SHADER_CONFIG* shadersList) {
	ShaderProgram = glCreateProgram();
	for (int i = 0; i < count; i += 1) {
		LoadShader(shadersList[i].shaderPath, shadersList[i].type);
	}
	this->LinkAllShaders();
}


void Shader::LoadShader(const char* filePath, SHADER_STAGE TYPE) {
	// 1. Load file from src
	if (!filePath) {
		OutputDebugString("No input file specified");
		__debugbreak();
	}

	FILE* file;
	fopen_s(&file, filePath, "r");
	int FileLength, index = 0;

	if (!file) {
		OutputDebugString("No file found at given location.");
		__debugbreak();
	}

	fseek(file, 0, SEEK_END);
	FileLength = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* shaderSrc = (char*)malloc(sizeof(char) * FileLength);

	if (!shaderSrc) {
		OutputDebugString("Unable to malloc space for file");
		__debugbreak();
		return;
	}

	char c;
	while ((c = fgetc(file)) != EOF) {
		shaderSrc[index] = c;
		index += 1;
	}

	shaderSrc[index] = '\0';
	OutputDebugString(shaderSrc);

	fclose(file);

	// 2. Compile shader and check for errors.
	const unsigned int ShaderType = Shader::OPENGL_TYPES[TYPE];

	glCall(this->ShaderIDs[TYPE] = glCreateShader(ShaderType));
	glCall(glShaderSource(ShaderIDs[TYPE], 1, &shaderSrc, nullptr));
	glCall(glCompileShader(ShaderIDs[TYPE]));

	int compileStatus;
	glCall(glGetShaderiv(ShaderIDs[TYPE], GL_COMPILE_STATUS, &compileStatus));

	if (!compileStatus) {
		int length;
		glCall(glGetShaderiv(ShaderIDs[TYPE], GL_INFO_LOG_LENGTH, &length));
		char* message = (char*)malloc(sizeof(char) * length);
		glCall(glGetShaderInfoLog(ShaderIDs[TYPE], length, NULL, message));
		glCall(OutputDebugString(message));
		__debugbreak();
	}

	loadedShaders[TYPE] = true;
}

void Shader::LinkAllShaders() {
	for (int i = 0; i < 6; i += 1) {
		if (!this->loadedShaders[i]) continue;
		glCall(glAttachShader(ShaderProgram, this->ShaderIDs[i]));
	}
	glCall(glLinkProgram(ShaderProgram));

	int linkStatus;
	glCall(glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &linkStatus));

	if (!linkStatus) {
		int length;
		glCall(glGetProgramiv(ShaderProgram, GL_INFO_LOG_LENGTH, &length));
		char* message = (char*)malloc(sizeof(char) * length);
		glCall(glGetProgramInfoLog(ShaderProgram, length, NULL, message));
		OutputDebugString(message);
		__debugbreak();
	}
}

void Shader::UseProgram() {
	glCall(glUseProgram(ShaderProgram));
}

void Shader::ModifyUniform(const char* UniformName, glm::mat4& matrix) {
	UseProgram();
	glCall(int uniform = glGetUniformLocation(ShaderProgram, UniformName));
	if (uniform == -1) {
		OutputDebugString("No such uniform found");
		__debugbreak();
	}

	glCall(glUniformMatrix4fv(uniform, 1, GL_FALSE, &matrix[0][0]));
}

void Shader::ModifyUniform(const char* UniformName, int value) {
	UseProgram();
	glCall(int uniform = glGetUniformLocation(ShaderProgram, UniformName));
	if (uniform == -1) {
		OutputDebugString("No such uniform found");
		__debugbreak();
	}

	glCall(glUniform1i(uniform, value));
}