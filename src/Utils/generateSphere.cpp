#include "Utils/generateSphere.h"

float* generateSphere(float radius, int longitudeDivision, int latitudeDivision, int& outVertexCount) {
	const float PI = 3.14159265359f;

	// outVertexCount is number of vertices (6 per quad)
	outVertexCount = longitudeDivision * latitudeDivision * 6;
		
	// Position-only vertices: Pos(3)
	int totalFloats = outVertexCount * 3;
	float* data = new float[totalFloats];

	float sectorStep = 2.0f * PI / longitudeDivision;
	float stackStep = PI / latitudeDivision; // Latitude is 0 to PI

	int dataIndex = 0;

	for (int i = 0; i < latitudeDivision; ++i) {
		float stackAngle = PI / 2 - i * stackStep;        // Starting from top (PI/2) to bottom (-PI/2)
		float nextStackAngle = PI / 2 - (i + 1) * stackStep;

		// Compute XZ radius for the current stack and the next stack
		float xy = radius * cosf(stackAngle);
		float z = radius * sinf(stackAngle);

		float nextXY = radius * cosf(nextStackAngle);
		float nextZ = radius * sinf(nextStackAngle);

		for (int j = 0; j < longitudeDivision; ++j) {
			float sectorAngle = j * sectorStep;
			float nextSectorAngle = (j + 1) * sectorStep;

			// --- Vertex Calculations ---

			// We need 4 points for a quad: 
			// k1--k2
			// |  / |
			// k3--k4

			// Point 1 (Current Stack, Current Sector)
			float x1 = xy * cosf(sectorAngle);
			float y1 = xy * sinf(sectorAngle);
			float z1 = z;
			float s1 = (float)j / longitudeDivision;
			float t1 = (float)i / latitudeDivision;

			// Point 2 (Current Stack, Next Sector)
			float x2 = xy * cosf(nextSectorAngle);
			float y2 = xy * sinf(nextSectorAngle);
			float z2 = z;
			float s2 = (float)(j + 1) / longitudeDivision;
			float t2 = (float)i / latitudeDivision;

			// Point 3 (Next Stack, Current Sector)
			float x3 = nextXY * cosf(sectorAngle);
			float y3 = nextXY * sinf(sectorAngle);
			float z3 = nextZ;
			float s3 = (float)j / longitudeDivision;
			float t3 = (float)(i + 1) / latitudeDivision;

			// Point 4 (Next Stack, Next Sector)
			float x4 = nextXY * cosf(nextSectorAngle);
			float y4 = nextXY * sinf(nextSectorAngle);
			float z4 = nextZ;
			float s4 = (float)(j + 1) / longitudeDivision;
			float t4 = (float)(i + 1) / latitudeDivision;

			// --- Generate Triangles (Counter-Clockwise) ---

			// Helper lambda to write a single vertex
			auto pushVertex = [&](float x, float y, float z, float s, float t) {
				(void)s;
				(void)t;
				data[dataIndex++] = x;
				data[dataIndex++] = y;
				data[dataIndex++] = z;
			};

			// Triangle 1 (k1 -> k3 -> k2)
			pushVertex(x1, y1, z1, s1, t1);
			pushVertex(x3, y3, z3, s3, t3);
			pushVertex(x2, y2, z2, s2, t2);

			// Triangle 2 (k2 -> k3 -> k4)
			pushVertex(x2, y2, z2, s2, t2);
			pushVertex(x3, y3, z3, s3, t3);
			pushVertex(x4, y4, z4, s4, t4);
		}
	}

	return data;
}