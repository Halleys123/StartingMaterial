#pragma once

#include "Mesh/Mesh.h"
#include "glm/glm.hpp"
#include "Material/Shader.h"
#include "Utils/generateSphere.h"

class PointLight {
private:
	static int activePointLights;
	static float lightPositions[30];
	static float lightColors[30];
	static float lightStrength[10];
	static Mesh* mesh;
	static bool initialized;

	static int lightSphereVertexCount;
	static float* lightSphereVertex;
	static Shader* lightShader;

	int id = 0;
	
public:
	PointLight(Shader& shader, glm::vec3 lightPos, glm::vec3 lightColor, float strength);
	~PointLight();
	static void Init();
	static void Update(glm::vec3& camPosition, glm::mat4& camera);
};