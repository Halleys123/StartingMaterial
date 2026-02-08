#include "Utils/generateCylinder.h"
#include <stdlib.h>
#include <math.h>

void generateCylinder(float*& vertexData, unsigned int*& indices, int& floatCount, int& indexCount, float height, float radius) {
	const int n = 36;
	const float PI = 3.14159265359f;
	const int stride = 8; // 3 pos + 3 norm + 2 uv

	/*
	   Total Vertices needed for clean shading/UVs:
	   - Sides: (n + 1) * 2  (We use n+1 to close the UV seam)
	   - Top Cap: n + 1 (Edge + Center)
	   - Bottom Cap: n + 1 (Edge + Center)
	   Total = 4n + 4 vertices
	*/
	int totalVertices = (n + 1) * 4;
	floatCount = totalVertices * stride;
	vertexData = (float*)malloc(sizeof(float) * floatCount);

	indexCount = (n * 2 * 3) + (n * 3) + (n * 3); // Sides + Top + Bottom
	indices = (unsigned int*)malloc(sizeof(unsigned int) * indexCount);

	float hHeight = height / 2.0f;
	int vIdx = 0;

	// --- 1. SIDE WALLS ---
	for (int i = 0; i <= n; i++) {
		float ratio = (float)i / (float)n;
		float angle = ratio * 2.0f * PI;
		float x = cos(angle);
		float z = sin(angle); // Using Z as depth for standard orientation

		// Top Side Vertex
		vertexData[vIdx++] = x * radius;    // Pos
		vertexData[vIdx++] = hHeight;
		vertexData[vIdx++] = z * radius;
		vertexData[vIdx++] = x;             // Norm (pointing out)
		vertexData[vIdx++] = 0.0f;
		vertexData[vIdx++] = z;
		vertexData[vIdx++] = ratio;         // U
		vertexData[vIdx++] = 1.0f;          // V

		// Bottom Side Vertex
		vertexData[vIdx++] = x * radius;
		vertexData[vIdx++] = -hHeight;
		vertexData[vIdx++] = z * radius;
		vertexData[vIdx++] = x;
		vertexData[vIdx++] = 0.0f;
		vertexData[vIdx++] = z;
		vertexData[vIdx++] = ratio;
		vertexData[vIdx++] = 0.0f;
	}

	// --- 2. CAPS (Positions & Normals) ---
	int topCapStart = vIdx / stride;
	for (int i = 0; i <= n; i++) { // Top edge + center
		float angle = (float)i / (float)n * 2.0f * PI;
		float x = (i == n) ? 0 : cos(angle);
		float z = (i == n) ? 0 : sin(angle);

		vertexData[vIdx++] = x * radius;
		vertexData[vIdx++] = hHeight;
		vertexData[vIdx++] = z * radius;
		vertexData[vIdx++] = 0.0f;          // Norm (pointing UP)
		vertexData[vIdx++] = 1.0f;
		vertexData[vIdx++] = 0.0f;
		vertexData[vIdx++] = x * 0.5f + 0.5f; // UV mapping for circle
		vertexData[vIdx++] = z * 0.5f + 0.5f;
	}

	int botCapStart = vIdx / stride;
	for (int i = 0; i <= n; i++) { // Bottom edge + center
		float angle = (float)i / (float)n * 2.0f * PI;
		float x = (i == n) ? 0 : cos(angle);
		float z = (i == n) ? 0 : sin(angle);

		vertexData[vIdx++] = x * radius;
		vertexData[vIdx++] = -hHeight;
		vertexData[vIdx++] = z * radius;
		vertexData[vIdx++] = 0.0f;          // Norm (pointing DOWN)
		vertexData[vIdx++] = -1.0f;
		vertexData[vIdx++] = 0.0f;
		vertexData[vIdx++] = x * 0.5f + 0.5f;
		vertexData[vIdx++] = z * 0.5f + 0.5f;
	}

	// --- 3. INDICES ---
	int iIdx = 0;

	// Sides (Quads)
	for (int i = 0; i < n; i++) {
		int v = i * 2;
		indices[iIdx++] = v;
		indices[iIdx++] = v + 1;
		indices[iIdx++] = v + 2;

		indices[iIdx++] = v + 2;
		indices[iIdx++] = v + 1;
		indices[iIdx++] = v + 3;
	}

	// Top Cap (Triangle Fan)
	int topCenter = topCapStart + n;
	for (int i = 0; i < n; i++) {
		indices[iIdx++] = topCenter;
		indices[iIdx++] = topCapStart + i;
		// If it's the last triangle, connect back to the start of the cap
		indices[iIdx++] = topCapStart + ((i + 1) % n);
	}

	// Bottom Cap (Triangle Fan)
	int botCenter = botCapStart + n;
	for (int i = 0; i < n; i++) {
		indices[iIdx++] = botCenter;
		// If it's the last triangle, connect back to the start of the cap
		indices[iIdx++] = botCapStart + ((i + 1) % n);
		indices[iIdx++] = botCapStart + i;
	}

}

