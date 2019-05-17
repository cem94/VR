#include "DPSEngine.h"

// Glew (include it before GL.h):
#include <GL/glew.h>

// FreeGLUT:
#include <GL/freeglut.h>


/**
*	Costructor of DPSMesh
*
*	@author		DPS
*	@param		void
*	@return		void
*
*/
LIB_API DPSMesh::DPSMesh() : material{ nullptr }, light{ true }{}

/**
*	Destructor of DPSMesh
*
*	@author		DPS
*	@param		void
*	@return		void
*
*/
LIB_API DPSMesh::~DPSMesh(){}


/**
*	Get material
*
*	@author		DPS
*	@param		void
*	@return		DPSMaterial *
*
*/
DPSMaterial * DPSMesh::getMaterial()
{
	return this->material;
}


/**
*	Set material of DPSMesh
*
*	@author		DPS
*	@param		DPSMaterial* mat
*	@return		void
*
*/
void LIB_API DPSMesh::setMaterial(DPSMaterial* mat)
{
	this->material = mat;
}


/**
*	Rendering of DPSMesh
*
*	@author		DPS
*	@param		glm::mat4 inverseCameraMatrix
*	@return		void
*
*/
void LIB_API DPSMesh::render(glm::mat4 inverseCameraMatrix)
{
	DPSEngine::loadMaterial(material);

	DPSEngine::loadTexture(material->getTexture());

	DPSEngine::enableLight(isLightEnabled());

	DPSEngine::setModelView(inverseCameraMatrix * this->getWorldMatrix());

	glBindBuffer(GL_ARRAY_BUFFER, vertexVbo);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, normalVbo);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, textureVbo);
	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, facesVbo);
	glDrawElements(GL_TRIANGLES, numberOfFaces * 3, GL_UNSIGNED_INT, nullptr);
}


/**
*	Check if is trasparent
*
*	@author		DPS
*	@param		void
*	@return		bool
*
*/
bool DPSMesh::isTransparent()
{
	if (this->material != nullptr) {
		if (this->material->getTrasparency() != 1.0f)
			return true;
		else 
			return false;
	}
	else 
		return false;
}


/**
*	Check if light is enabled
*
*	@author		DPS
*	@param		void
*	@return		bool
*
*/
bool DPSMesh::isLightEnabled()
{
	return this->light;
}


/**
*	Use light
*
*	@author		DPS
*	@param		bool state
*	@return		void
*
*/
void LIB_API DPSMesh::useLight(bool state)
{
	this->light = state;

}

/**
*	DPS TYPES
*
*	@author		DPS
*	@param		void
*	@return		DPS::TYPES
*
*/
types LIB_API DPSMesh::getType()
{
	return DPSTYPE_MESH;
}