#include "DPSEngine.h"

// Glew (include it before GL.h):
#include <GL/glew.h>

// FreeGLUT:
#include <GL/freeglut.h>

// FreeImage:
#include <FreeImage.h>


//global
static FIBITMAP *bitmap;
std::string DPSTexture::texPath;


/**
*	Costructor of DPSTexture
*
*	@author		DPS
*	@param		void
*	@return		void
*
*/
LIB_API DPSTexture::DPSTexture() {}


/**
*	Destructor of DPSTexture
*
*	@author		DPS
*	@param		void
*	@return		void
*
*/
LIB_API DPSTexture::~DPSTexture()
{
	glDeleteTextures(1, &this->id);
}


/**
*	Set textures path
*	@author		DPS
*	@param		void
*	@return		void
*
*/
void LIB_API DPSTexture::setPath(std::string path)
{
	texPath = path;
}


/**
*	Load the file-texture
*
*	@author		DPS
*	@param		const char * fileName
*	@return		void
*
*/
void LIB_API DPSTexture::loadTexture(const char * fileName)
{
	char *filePath = new char[texPath.length() + strlen(fileName) + 1];
	strcpy(filePath, texPath.c_str());
	strcat(filePath, fileName);

	//cout << endl << "Loading texture from file " << filePath << endl << endl;

	//Generate texture ID
	glGenTextures(1, &textureId);

	//Bind texture ID
	glBindTexture(GL_TEXTURE_2D, textureId);

	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//Generate texture
	bitmap = FreeImage_ConvertTo32Bits(FreeImage_Load(FreeImage_GetFileType(filePath, 0), filePath));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA, FreeImage_GetWidth(bitmap), FreeImage_GetHeight(bitmap), 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(bitmap));

	//MIPMAPS
	glGenerateMipmap(GL_TEXTURE_2D);

	//Unbind texture 
	glBindTexture(GL_TEXTURE_2D, NULL);

	delete[] filePath;
}


/**
*	Initialize FreeImage
*	@author		DPS
*	@param		void
*	@return		void
*
*/
void LIB_API DPSTexture::initializeFreeImage()
{
	FreeImage_Initialise();
}


/**
*	Deinitialize FreeImage
*	@author		DPS
*	@param		void
*	@return		void
*
*/
void LIB_API DPSTexture::deinitializeFreeImage()
{
	FreeImage_Unload(bitmap);
	FreeImage_DeInitialise();
}