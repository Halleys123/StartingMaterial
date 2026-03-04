#include "glad/glHelp.h"
#include "glad/glad.h"
#include "math.h"

#include "WndProc.h"
#include "Mesh/Mesh.h"
#include "Material/Shader.h"
#include "Material/Texture.h"
#include "Material/Sampler.h"
#include "Material/Material.h"
#include "Lights/PointLight.h"
#include "Utils/generateCylinder.h"

// Will someday use my own matrix, for now perspective matrix is also difficult to create, and will take a lot of time to just setup matrix class
// So for now focusing on main opengl then will come back for matrix and all
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <Windows.h>

glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 camPosition = glm::vec3(0.0f, 1.0f, 3.0f); 
glm::vec3 camLookAt = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 camFront = glm::normalize(camLookAt - camPosition);
glm::vec3 camRight = glm::normalize(glm::cross(camFront, worldUp));
glm::vec3 camUp = glm::normalize(glm::cross(camRight, camFront));

extern HDC hdc;
extern HGLRC rc;

static const float aspectRatio = 16.0f / 10.0f;

static const int WndHeight = 1280;
static const int WndWidth = WndHeight * aspectRatio;

#ifdef _WIN32
extern "C" {
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

void UpdateCameraVectors() {
	camFront = glm::normalize(camFront);
	camRight = glm::normalize(glm::cross(camFront, worldUp));
	camUp = glm::normalize(glm::cross(camRight, camFront));
}

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

	float side = 2.0f;
	float half = side / 2.0f;
	float centerX = 0.0f, centerY = 0.0f, centerZ = 0.0f;
	float x0 = centerX + half, x1 = centerX - half;
	float y0 = centerY + half, y1 = centerY - half;
	float z0 = centerZ + half, z1 = centerZ - half;

	// Cube vertices: 24 vertices * 8 floats (3 position + 3 normal + 2 UV) = 192 floats
	float cubeVertices[192] = {
		// Positions        // Normals        // UVs

		// Bottom (-Z)
		x1,y1,z1,  0,0,-1,  1,0,
		x0,y1,z1,  0,0,-1,  0,0,
		x0,y0,z1,  0,0,-1,  0,1,
		x1,y0,z1,  0,0,-1,  1,1,

		// Back (-Z)
		x0,y1,z0,  0,0,1, 0,0,
		x1,y1,z0,  0,0,1, 1,0,
		x1,y0,z0,  0,0,1, 1,1,
		x0,y0,z0,  0,0,1, 0,1,

		// Right (+X)
		x0,y1,z1, 1,0,0,  1,0,
		x0,y1,z0, 1,0,0,  0,0,
		x0,y0,z0, 1,0,0,  0,1,
		x0,y0,z1, 1,0,0,  1,1,

		// Left (-X)
		x1,y1,z0, -1,0,0,  1,0,
		x1,y1,z1, -1,0,0,  0,0,
		x1,y0,z1, -1,0,0,  0,1,
		x1,y0,z0, -1,0,0,  1,1,

		// Bottom (-Y)
		x1,y1,z0,  0,-1,0,  1,0,
		x0,y1,z0,  0,-1,0,  0,0,
		x0,y1,z1,  0,-1,0,  0,1,
		x1,y1,z1,  0,-1,0,  1,1,

		// Top (+Y)
		x1,y0,z1,  0,1,0, 1,1,
		x0,y0,z1,  0,1,0, 0,1,
		x0,y0,z0,  0,1,0, 0,0,
		x1,y0,z0,  0,1,0, 1,0
	};

	// Indices for 12 triangles (6 faces * 2 triangles each)
	unsigned int cubeIndices[36] = {
		// Front face
		1, 0, 2,  2, 0, 3,
		// Back face
		5, 4, 6,  6, 4, 7,
		// Left face
		9, 8, 10,  10, 8, 11,
		// Right face
		13, 12, 14,  14, 12, 15,
		// Top face
		17, 16, 18,  18, 16, 19,
		// Bottom face
		21, 20, 22,  22, 20, 23
	};

	float groundCoords[32] = {
		// Coords				Normal				UV	
		-30.0f,	-1.1f,-30.0f,	0.0f, 1.0f, 0.0f,	0.0f, 0.0f,
		-30.0f, -1.1f, 30.0f,	0.0f, 1.0f, 0.0f,	0.0f, 10.0f,
		 30.0f,	-1.1f,-30.0f,	0.0f, 1.0f, 0.0f,	10.0f, 0.0f,
		 30.0f,	-1.1f, 30.0f,	0.0f, 1.0f, 0.0f,	10.0f, 10.0f,
	};
	unsigned int groundIndex[6] = {
		0, 1, 2,
		1, 2, 3
	};

	float* myCoords = nullptr;
	unsigned int* myIndices = nullptr;
	int cCount, iCount;

	generateCylinder(myCoords, myIndices, cCount, iCount, 2.0f, 0.5f);

	glm::mat4 perspective = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
	glm::mat4 view = glm::lookAt(camPosition, camLookAt, camUp);

	glm::mat4 camera = perspective * view;

	SHADER_CONFIG list[2] = { { SHADER_STAGE::VERTEX, "./shaders/item.vert" }, { SHADER_STAGE::FRAGMT, "./shaders/item.frag" } };

	Mesh plane(192, cubeVertices, 36, cubeIndices);
	Mesh ground(32, groundCoords, 6, groundIndex);
	Mesh cylinder(cCount, myCoords, iCount, myIndices);
	//Material material()

	Shader myShader(2, list);
	Sampler sampler;

	Texture ground_tex("./textures/river_bed.jpg", GL_TEXTURE_2D, GL_RGB, GL_RGB);
	Texture ground_tex_rough("./textures/river_bed_spec.png", GL_TEXTURE_2D, GL_RGB, GL_RGBA);
	Texture tex("./textures/box.png", GL_TEXTURE_2D, GL_RGB, GL_RGBA);
	Texture tex_spec("./textures/box_spec.png", GL_TEXTURE_2D, GL_RGB, GL_RGBA);

	PointLight::Init();
	PointLight light2(myShader, glm::vec3(-2.0f, 2.0f, 1.0f), glm::vec3(1.0f, 0.5f, 0.5f), 0.5f);
	PointLight light1(myShader, glm::vec3(2.0f, 2.0f, 1.0f), glm::vec3(1.0f), 0.5f);
	PointLight light6(myShader, glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(1.0f), 0.5f);
	PointLight light3(myShader, glm::vec3(-2.0f, 0.0f, -2.0f), glm::vec3(1.0f, 0.5f, 0.0f), 0.5f);
	PointLight light4(myShader, glm::vec3(-4.0f, 3.0f, 2.0f), glm::vec3(0.0f, 0.5f, 0.0f), 0.5f);
	PointLight light5(myShader, glm::vec3(5.5f, 4.0f, -2.0f), glm::vec3(0.7f, 0.5f, 1.0f), 0.5f);
	PointLight light7(myShader, glm::vec3(3.5f, 8.0f, -2.0f), glm::vec3(1.0f, 0.25f, 1.0f), 2.5f);

	plane.setVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
	plane.setVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
	plane.setVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));

	ground.setVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
	ground.setVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
	ground.setVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));
	
	cylinder.setVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
	cylinder.setVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
	cylinder.setVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));

	myShader.UseProgram();
	myShader.ModifyUniform("tex", 0);
	myShader.ModifyUniform("tex_spec", 1);
	myShader.ModifyUniform("perspective_matrix", camera);

	sampler.SetValue(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	sampler.SetValue(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	sampler.SetValue(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	sampler.SetValue(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	
	LARGE_INTEGER frequency, last, now;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&last);

	static const float targetFrameTime = 1.0 / 90.0;
	static const float sensitivity = 0.1f;

	glm::vec3 velocity = glm::vec3(0.0f);
	glm::vec3 maxSpeed = glm::vec3(2.0f, 2.0f, 2.0f);
	static const float acceleration = 30.0f;
	static const float drag = 5.0f;  // Higher = slower, more drag

	float pitch = 00.0f, yaw = -90.0f;
	
	ShowCursor(false);
	UpdateCameraVectors();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	//glEnable(GL_STENCIL_TEST);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	//glStencilMask(0xFF);
	//glStencilFunc(GL_ALWAYS, 1, 0xFF);
	//glDepthFunc(GL_NEVER);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glm::mat4 cylinderModel = glm::translate(glm::mat4(1.0), glm::vec3(3.0));
	glm::mat4 model = glm::translate(glm::mat4(1.0), glm::vec3(0.0));

	float quadVertices[24] = {
		// positions // texCoords
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};

	unsigned int quadIndex[6] = {
		0, 1, 2, 3, 4, 5
	};

	unsigned int fbo, texObj;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	glGenTextures(1, &texObj);
	glBindTexture(GL_TEXTURE_2D, texObj);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WndWidth, WndHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texObj, 0);

	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WndWidth, WndHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		OutputDebugString("ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	SHADER_CONFIG scrShaderConf[] = { { SHADER_STAGE::VERTEX, "./shaders/framebuffer.vert" }, { SHADER_STAGE::FRAGMT, "./shaders/framebuffer.frag" } };
	Shader screenShader(2, scrShaderConf);
	screenShader.ModifyUniform("screenTexture", 0); // ensure sampler uses texture unit 0

	Mesh quadMesh(24, quadVertices, 6, quadIndex);
	quadMesh.setVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);
	quadMesh.setVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));

	while (running) {

		QueryPerformanceCounter(&now);

		long long elapsed = now.QuadPart - last.QuadPart;
		float delta = (float)elapsed / (float)frequency.QuadPart;

		last = now;
		
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT) {
				running = false;
				break;
			}
		}

		POINT curPoint;
		GetCursorPos(&curPoint);

		// Get window center
		RECT windowRect;
		GetWindowRect(hWnd, &windowRect);
		int centerX = windowRect.left + (windowRect.right - windowRect.left) / 2;
		int centerY = windowRect.top + (windowRect.bottom - windowRect.top) / 2;

		// Calculate mouse offset from center
		float xoffset = (float)(curPoint.x - centerX);
		float yoffset = (float)(centerY - curPoint.y);

		// Reset cursor to center
		if (GetForegroundWindow() == hWnd)
			SetCursorPos(centerX, centerY);
		
		if (xoffset != 0.0f || yoffset != 0.0f) {
			xoffset *= sensitivity;
			yoffset *= sensitivity;

			yaw += xoffset * 0.75f;
			pitch += yoffset * 0.75f;
		}

		if (GetAsyncKeyState('W') & 0x8000) velocity += camFront * acceleration * delta;
		if (GetAsyncKeyState('S') & 0x8000) velocity -= camFront * acceleration * delta;
		if (GetAsyncKeyState('D') & 0x8000) velocity += camRight * acceleration * delta;
		if (GetAsyncKeyState('A') & 0x8000) velocity -= camRight * acceleration * delta;
		if (GetAsyncKeyState('Q') & 0x8000) velocity += camUp * acceleration * delta;
		if (GetAsyncKeyState('E') & 0x8000) velocity -= camUp * acceleration * delta;
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000) yaw += 0.15f;
		if (GetAsyncKeyState(VK_LEFT) & 0x8000) yaw -= 0.15f;
		if (GetAsyncKeyState(VK_UP) & 0x8000) pitch += 0.15f;
		if (GetAsyncKeyState(VK_DOWN) & 0x8000) pitch -= 0.15f;

		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;

		velocity -= velocity * drag * delta;
		if (glm::length(velocity) < 0.01f) {
			velocity = glm::vec3(0.0f);
		}
		camPosition += velocity * delta;

		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		camFront = glm::normalize(direction);
		UpdateCameraVectors();

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glViewport(0, 0, WndWidth, WndHeight);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		view = glm::lookAt(camPosition, camPosition + camFront, camUp);
		camera = perspective * view;

		PointLight::Update(camPosition, camera);
		
		myShader.UseProgram();
		myShader.ModifyUniform("model", model);
		myShader.ModifyUniform("perspective_matrix", camera);
		myShader.ModifyUniform("viewPos", camPosition);

		tex.Bind(0, sampler);
		tex_spec.Bind(1, sampler);
		plane.Draw(GL_TRIANGLES, GL_UNSIGNED_INT);

		ground_tex.Bind(0, sampler);
		ground_tex_rough.Bind(1, sampler);
		ground.Draw(GL_TRIANGLES, GL_UNSIGNED_INT);

		myShader.ModifyUniform("model", cylinderModel);
		cylinder.Draw(GL_TRIANGLES, GL_UNSIGNED_INT);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, WndWidth, WndHeight);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE); // make sure the quad isn’t culled

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        screenShader.UseProgram();
        glActiveTexture(GL_TEXTURE0);
        glBindSampler(0, 0); // unbind the mipmap sampler so texObj isn't treated as incomplete
		glBindTexture(GL_TEXTURE_2D, texObj);

		quadMesh.Draw(GL_TRIANGLES, GL_UNSIGNED_INT);

		SwapBuffers(hdc);
	}

	return 0;
}