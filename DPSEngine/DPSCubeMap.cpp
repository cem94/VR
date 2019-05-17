#include "DPSCubeMap.h"

// GLEW:
#include <GL/glew.h>
// FreeImage:
#include <FreeImage.h>


/////////////
// SHADERS //
/////////////

//////////////////////////////////////////
const char *vertShader = R"(
   #version 440 core

   uniform mat4 projection;
   uniform mat4 modelview;

   layout(location = 0) in vec3 in_Position;      

   out vec3 texCoord;

   void main(void)
   {
      texCoord = in_Position;
      gl_Position = projection * modelview * vec4(in_Position, 1.0f);            
   }
)";

//////////////////////////////////////////
const char *fragShader = R"(
   #version 440 core
   
   in vec3 texCoord;
   
   // Texture mapping (cubemap):
   layout(binding = 0) uniform samplerCube cubemapSampler;

   out vec4 fragOutput;

   void main(void)
   {       
      fragOutput = texture(cubemapSampler, texCoord);
   }
)";


DPSCubeMap::DPSCubeMap(std::string * filesName)
{
	/*
	if (sizeof(filesName) / sizeof(filesName[0]) == 6)
		for (int i = 0; i < 6; i++)
			this->cubemapNames[i] = filesName[i];
	*/

	// Compile shaders:
	vs = new Shader();
	vs->loadFromMemory(Shader::TYPE_VERTEX, vertShader);

	fs = new Shader();
	fs->loadFromMemory(Shader::TYPE_FRAGMENT, fragShader);

	shader = new Shader();
	shader->build(vs, fs);
	shader->render();
	shader->bind(0, "in_Position");

	projLoc = shader->getParamLocation("projection");
	mvLoc = shader->getParamLocation("modelview");

	// Load cube data into a VBO:
	glGenBuffers(1, &cubeVboVertices);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVboVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &cubeVboFaces);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeVboFaces);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeFaces), cubeFaces, GL_STATIC_DRAW);
}


DPSCubeMap::~DPSCubeMap()
{
	glDeleteBuffers(1, &cubeVboVertices);
	glDeleteBuffers(1, &cubeVboFaces);
	glDeleteTextures(1, &cubemapId);

	delete shader;
	delete fs;
	delete vs;
}

void DPSCubeMap::buildCubemap()
{
	// Create and bind cubemap:   
	glGenTextures(1, &cubemapId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapId);

	// Set params:
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Set filters:
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Load sides:
	for (int curSide = 0; curSide < 6; curSide++)
	{
		// Load texture:
		FIBITMAP *fBitmap = FreeImage_Load(FreeImage_GetFileType(cubemapNames[curSide].c_str(), 0), cubemapNames[curSide].c_str());
		if (fBitmap == nullptr)
			std::cout << "[ERROR] loading file '" << cubemapNames[curSide] << "'" << std::endl;
		int intFormat = GL_RGB;
		GLenum extFormat = GL_BGR;
		if (FreeImage_GetBPP(fBitmap) == 32)
		{
			intFormat = GL_RGBA;
			extFormat = GL_BGRA;
		}

		// Fix mirroring:
		FreeImage_FlipHorizontal(fBitmap);  // Correct mirroring from cube's inside
		FreeImage_FlipVertical(fBitmap);    // Correct JPG's upside-down

		// Send texture to OpenGL:
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + curSide, 0, intFormat, FreeImage_GetWidth(fBitmap), FreeImage_GetHeight(fBitmap), 0, extFormat, GL_UNSIGNED_BYTE, (void *)FreeImage_GetBits(fBitmap));

		// Free resources:
		FreeImage_Unload(fBitmap);
	}
}


void DPSCubeMap::setProjection(glm::mat4 projectionMatrix)
{
	//Load Projection 
	shader->setMatrix(projLoc, projectionMatrix);
}

void DPSCubeMap::setModelView(glm::mat4 modelMatrix)
{
	// Set model matrix as current OpenGL matrix:
	shader->setMatrix(mvLoc, modelMatrix);
}

void DPSCubeMap::enableShader()
{
	shader->render();
}

void DPSCubeMap::render()
{

	glCullFace(GL_FRONT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapId);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVboVertices);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeVboFaces);
	//glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeFaces), cubeFaces, GL_STATIC_DRAW);

	glDrawElements(GL_TRIANGLES, sizeof(cubeFaces) / sizeof(unsigned short), GL_UNSIGNED_SHORT, nullptr);
	glCullFace(GL_BACK);
}