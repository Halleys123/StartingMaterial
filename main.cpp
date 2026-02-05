#define STB_IMAGE_IMPLEMENTATION

#include "glad/glHelp.h"
#include "glad/glad.h"
#include "math.h"

#include "WndProc.h"
#include "Mesh/Mesh.h"
#include "Material/Shader.h"

// Will someday use my own matrix, for now perspective matrix is also difficult to create, and will take a lot of time to just setup matrix class
// So for now focusing on main opengl then will come back for matrix and all
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "stb_image.h"

#include <stdio.h>
#include <Windows.h>

extern HDC hdc;
extern HGLRC rc;

static const float aspectRatio = 4.0 / 3.0f;

static const int WndHeight = 600;
static const int WndWidth = WndHeight * aspectRatio;

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

	float coords[20] = {
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f, 1.0f
	};

	unsigned int index[6] = {
		0, 1, 2,
		1, 2, 3
	};

	glm::mat4 perspective = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 1.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	perspective = perspective * view;

	SHADER_CONFIG list[2] = { { SHADER_STAGE::VERTEX, "./shaders/item.vert" }, { SHADER_STAGE::FRAGMT, "./shaders/item.frag" } };

	Mesh plane(20, coords, 6, index, GL_STATIC_DRAW);
	plane.setVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	plane.setVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));
	Shader myShader(2, list);

	myShader.UseProgram();
	myShader.ModifyUniform("perspective_matrix", perspective);

	unsigned int texture, sampler;

	int x, y, c;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* image = stbi_load("./textures/ground.jpg", &x, &y, &c, 0);

	glCall(glGenTextures(1, &texture));
	glCall(glBindTexture(GL_TEXTURE_2D, texture));
	glCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, image));

	glCall(glGenSamplers(1, &sampler));

	glCall(glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	glCall(glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	glCall(glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT));
	glCall(glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT));

	glCall(glActiveTexture(GL_TEXTURE0));
	glCall(glBindSampler(0, sampler));

	myShader.ModifyUniform("tex", 0);

	while (running) {
		if (msg.message == WM_QUIT) running = false;
		
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		plane.Draw(GL_TRIANGLES, GL_UNSIGNED_INT);;

		SwapBuffers(hdc);
	}

	return 0;
}