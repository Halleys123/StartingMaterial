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
		0, 1, 2,  0, 2, 3,
		// Back face
		4, 5, 6,  4, 6, 7,
		// Left face
		8, 9, 10,  8, 10, 11,
		// Right face
		12, 13, 14,  12, 14, 15,
		// Top face
		16, 17, 18,  16, 18, 19,
		// Bottom face
		20, 21, 22,  20, 22, 23
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

	float radius = 1.0f;
	int longitude = 36;
	int latitude = 18;

	glm::mat4 perspective = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
	glm::mat4 view = glm::lookAt(camPosition, camLookAt, camUp);

	glm::mat4 camera = perspective * view;

	SHADER_CONFIG list[2] = { { SHADER_STAGE::VERTEX, "./shaders/item.vert" }, { SHADER_STAGE::FRAGMT, "./shaders/item.frag" } };

	Mesh plane(192, cubeVertices, 36, cubeIndices, GL_STATIC_DRAW);
	Mesh ground(32, groundCoords, 6, groundIndex);
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

	plane.setVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
	plane.setVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
	plane.setVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));

	ground.setVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
	ground.setVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
	ground.setVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));

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

	while (running) {

		QueryPerformanceCounter(&now);

		long long elapsed = now.QuadPart - last.QuadPart;
		float delta = (float)elapsed / (float)frequency.QuadPart;

		last = now;

		if (msg.message == WM_QUIT) running = false;
		
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
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

			yaw += xoffset;
			pitch += yoffset;
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

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		view = glm::lookAt(camPosition, camPosition + camFront, camUp);
		camera = perspective * view;

		PointLight::Update(camPosition, camera);

		myShader.UseProgram();
		myShader.ModifyUniform("perspective_matrix", camera);
		myShader.ModifyUniform("viewPos", camPosition);

		ground_tex.Bind(0, sampler);
		ground_tex_rough.Bind(1, sampler);
		ground.Draw(GL_TRIANGLES, GL_UNSIGNED_INT);
		
		tex.Bind(0, sampler);
		tex_spec.Bind(1, sampler);
		plane.Draw(GL_TRIANGLES, GL_UNSIGNED_INT);

		SwapBuffers(hdc);
	}

	return 0;
}