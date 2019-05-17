#include "DPSEngine.h"


/**
*	Costructor of DPSCamera
*
*	@author		DPS
*	@param		void
*	@return		void
*
*/
DPSCamera::DPSCamera(){}


/**
*	Destructor of DPSCamera
*
*	@author		DPS
*	@param		void
*	@return		void
*
*/
DPSCamera::~DPSCamera(){}


/**
*	Get the inverse matrix of DPSCamera
*
*	@author		DPS
*	@param		void
*	@return		glm::mat4
*
*/
glm::mat4 DPSCamera::getInverseMatrix()
{
	return glm::inverse(matrix);
}


/**
*	Get the type of DPSCamera
*
*	@author		DPS
*	@param		void
*	@return		types (enum)
*
*/
types DPSCamera::getType()
{
	return DPSTYPE_CAMERA;
}


/**
*	Set the eye value of DPSCamera
*
*	@author		DPS
*	@param		glm::vec3 eyes
*	@return		void
*
*/
void DPSCamera::setEyes(glm::vec3 eyes)
{
	this->eyes = eyes;
}


/**
*	Set the center value of DPSCamera
*
*	@author		DPS
*	@param		glm::vec3 center
*	@return		void
*
*/
void DPSCamera::setCenter(glm::vec3 center)
{
	this->center = center;
}


/**
*	Set the up value of DPSCamera
*
*	@author		DPS
*	@param		glm::vec3 up
*	@return		void
*
*/
void DPSCamera::setUp(glm::vec3 up)
{
	this->up = up;
}


/**
*	Set the projection matrix value of DPSCamera
*
*	@author		DPS
*	@param		glm::mat4 projetion
*	@return		void
*
*/
void DPSCamera::setProjection(glm::mat4 projetion)
{
	this->projection = projetion;
}


/**
*   Get eye value of DPSCamera
*
*	@author		DPS
*	@param		void
*	@return		glm::vec3
*
*/
glm::vec3 DPSCamera::getEyes()
{
	return this->eyes;
}


/**
*   Get center value of DPSCamera
*
*	@author		DPS
*	@param		void
*	@return		glm::vec3
*
*/
glm::vec3 DPSCamera::getCenter()
{
	return this->center;
}


/**
*   Get up value of DPSCamera
*
*	@author		DPS
*	@param		void
*	@return		glm::vec3
*
*/
glm::vec3 DPSCamera::getUp()
{
	return this->up;
}


/**
*   Get nromal direction value of DPSCamera
*
*	@author		DPS
*	@param		void
*	@return		glm::vec3
*
*/
glm::vec3 DPSCamera::getNormalDirectionVector()
{
	return this->normalDirectionVector;
}


/**
*   Get projection value of DPSCamera
*
*	@author		DPS
*	@param		void
*	@return		glm::mat4
*
*/
glm::mat4  DPSCamera::getProjection()
{
	return this->projection;
}