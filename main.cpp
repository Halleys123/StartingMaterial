#include "glad/glHelp.h"
#include "glad/glad.h"
#include "WndProc.h"

#include <stdio.h>
#include <Windows.h>

extern HDC hdc;
extern HGLRC rc;

static const float aspectRatio = 4.0 / 3.0f;

static const int WndHeight = 400;
static const int WndWidth = WndHeight * aspectRatio;

const char* VertexShaderSrc =
"#version 330 core\n"
"layout(location = 0) in vec3 VertexPos;\n"
"out vec3 frag_pos;\n"
"void main() {\n"
"    frag_pos = VertexPos;\n"
"    gl_Position = vec4(VertexPos, 1.0);\n"
"}\n\0";

const char* FragmentShaderSrc =
"#version 330 core\n"
"in vec3 frag_pos;\n"
"out vec4 FragColor;\n"
"void main() {\n"
"    FragColor = vec4(frag_pos, 1.0);\n"
"}\n\0";

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmd) {
	WNDCLASS wnd = { 0 };

	wnd.hInstance = hInstance;
	wnd.lpszClassName = "Materials";
	wnd.lpfnWndProc = WndProc;
	wnd.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClass(&wnd)) {
		OutputDebugString("Error in Registering Class.");
		__debugbreak();
	}

	HWND hWnd = CreateWindow(wnd.lpszClassName, "Materials", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, WndWidth, WndHeight, NULL, NULL, hInstance, NULL);

	if (!hWnd) {
		OutputDebugString("No Window Handle found");
		__debugbreak();
	}

	bool running = true;
	MSG msg = { 0 };

	float coords[12] = {
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
	};

	unsigned int index[6] = {
		0, 1, 2,
		1, 2, 3
	};

	unsigned int VAO, VBO, IBO;

	glCall(glGenVertexArrays(1, &VAO));
	glCall(glBindVertexArray(VAO));

	glCall(glGenBuffers(1, &IBO));
	glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO));
	glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, index, GL_STATIC_DRAW));

	// Try moving it above vbo setup
	glCall(glEnableVertexAttribArray(0));
	glCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0));

	glCall(glBindVertexArray(0));

	unsigned int ShaderProgram, VertexShader, FragmentShader;

	glCall(ShaderProgram = glCreateProgram());
	
	glCall(VertexShader = glCreateShader(GL_VERTEX_SHADER));
	glCall(FragmentShader = glCreateShader(GL_FRAGMENT_SHADER));
	
	glCall(glShaderSource(VertexShader, 1, &VertexShaderSrc, NULL));
	glCall(glShaderSource(FragmentShader, 1, &FragmentShaderSrc, NULL));

	glCall(glCompileShader(VertexShader));
	glCall(glCompileShader(FragmentShader));

	glCall(glAttachShader(ShaderProgram, VertexShader));
	glCall(glAttachShader(ShaderProgram, FragmentShader));

	glCall(glLinkProgram(ShaderProgram));

	glCall(glDeleteShader(VertexShader));
	glCall(glDeleteShader(FragmentShader));

	unsigned int texture;

	GLboolean isProgram = glIsProgram(ShaderProgram);
	if (!isProgram) {
		OutputDebugStringA("ERROR: ShaderProgram is NOT a valid program object!\n");
		__debugbreak();
	}
	else {
		OutputDebugStringA("ShaderProgram is a valid program object.\n");
	}

	glCall(glUseProgram(ShaderProgram));
	glBindVertexArray(VAO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	while (running) {
		if (msg.message == WM_QUIT) running = false;
		
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, index);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		SwapBuffers(hdc);
	}

	return 0;
}