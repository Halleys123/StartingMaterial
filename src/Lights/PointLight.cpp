#include "Lights/PointLight.h"
#include "glm/gtc/matrix_transform.hpp"

int PointLight::activePointLights = 0;
int PointLight::lightSphereVertexCount = 0;
float PointLight::lightPositions[30] = {};
float PointLight::lightColors[30] = {};
float PointLight::lightStrength[10] = {};
Mesh* PointLight::mesh = nullptr;
float* PointLight::lightSphereVertex = nullptr;
Shader* PointLight::lightShader = nullptr;
bool PointLight::initialized = false;

void PointLight::Init() {
	PointLight::activePointLights = 0;
	PointLight::lightSphereVertexCount = 0;
	PointLight::mesh = new Mesh[10];
	PointLight::lightSphereVertex = generateSphere(1.0f, 12, 12, PointLight::lightSphereVertexCount);
	SHADER_CONFIG list[2] = { { SHADER_STAGE::VERTEX, "./shaders/light.vert" }, { SHADER_STAGE::FRAGMT, "./shaders/light.frag" } };
	PointLight::lightShader = new Shader(2, list);

}

PointLight::PointLight(Shader& shader, glm::vec3 lightPos, glm::vec3 lightColor, float strength) {
	id = PointLight::activePointLights;

	const int base = PointLight::activePointLights * 3;

	PointLight::lightPositions[base + 0] = lightPos.x;
	PointLight::lightPositions[base + 1] = lightPos.y;
	PointLight::lightPositions[base + 2] = lightPos.z;

	PointLight::lightColors[base + 0] = lightColor.r;
	PointLight::lightColors[base + 1] = lightColor.g;
	PointLight::lightColors[base + 2] = lightColor.b;

	PointLight::lightStrength[PointLight::activePointLights] = strength;

	PointLight::mesh[PointLight::activePointLights] =
		Mesh(PointLight::lightSphereVertexCount * 3, PointLight::lightSphereVertex, 0, nullptr, GL_STATIC_DRAW);

	PointLight::mesh[PointLight::activePointLights].setVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	PointLight::activePointLights += 1;

	shader.ModifyUniform("activePointLights", PointLight::activePointLights);
	shader.ModifyUniformArray("pointLightPosition", PointLight::activePointLights, PointLight::lightPositions);
	shader.ModifyUniformArray("pointLightColor", PointLight::activePointLights, PointLight::lightColors);
	shader.ModifyUniform1fArray("pointLightStrength", PointLight::activePointLights, PointLight::lightStrength);
}

PointLight::~PointLight() {
	PointLight::activePointLights -= 1;
}

void PointLight::Update(glm::vec3& camPosition, glm::mat4& cameraMatrix) {
	PointLight::lightShader->UseProgram();
	PointLight::lightShader->ModifyUniform("perspective_matrix", cameraMatrix);

	for (int i = 0; i < PointLight::activePointLights; i += 1) {
		const int base = i * 3;

		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(
			PointLight::lightPositions[base + 0],
			PointLight::lightPositions[base + 1],
			PointLight::lightPositions[base + 2]
		));
		model = glm::scale(model, glm::vec3(0.1f)); // sphere size

		PointLight::lightShader->ModifyUniform("model", model);
		PointLight::lightShader->ModifyUniform("lightColor", PointLight::lightColors[base], PointLight::lightColors[base + 1], PointLight::lightColors[base + 2]);
		PointLight::mesh[i].DrawArray(GL_LINES, GL_UNSIGNED_BYTE);
	}
}