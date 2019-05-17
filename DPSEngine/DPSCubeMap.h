#pragma once
#include "DPSEngine.h"


class DPSCubeMap
{
public:
	DPSCubeMap(std::string *);
	~DPSCubeMap();

	void buildCubemap();

	void setProjection(glm::mat4);
	void setModelView(glm::mat4);
	void enableShader();

	void render();

private:
	// Textures:
	unsigned int cubemapId;

	// Shaders:
	Shader *vs = nullptr;
	Shader *fs = nullptr;
	Shader *shader = nullptr;

	int projLoc = -1;
	int mvLoc = -1;

	//Default texture
	std::string cubemapNames[6] =
	{
		"../../resources/posx.jpg",
	   "../../resources/negx.jpg",
	   "../../resources/posy.jpg",
	   "../../resources/negy.jpg",
	   "../../resources/posz.jpg",
	   "../../resources/negz.jpg",
	};

	// Cube VBO:
	unsigned int globalVao = 0;
	unsigned int cubeVboVertices = 0;
	float cubeVertices[24] = // Vertex and tex. coords are the same
	{
	   -1.0f,  1.0f,  1.0f,
	   -1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
	   -1.0f,  1.0f, -1.0f,
	   -1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
	};
	unsigned int cubeVboFaces = 0;
	unsigned short cubeFaces[36] =
	{
	   0, 1, 2,
	   0, 2, 3,
	   3, 2, 6,
	   3, 6, 7,
	   4, 0, 3,
	   4, 3, 7,
	   6, 5, 4,
	   7, 6, 4,
	   4, 5, 1,
	   4, 1, 0,
	   1, 5, 6,
	   1, 6, 2,
	};
};