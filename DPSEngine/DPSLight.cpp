#include "DPSEngine.h"

// Glew (include it before GL.h):
#include <GL/glew.h>

// FreeGLUT:
#include <GL/freeglut.h>


/**
*	Costructor of DPSLight - increment number of lights
*
*	@author		DPS
*	@param		void
*	@return		void
*
*/
LIB_API DPSLight::DPSLight()
{
	this->position = glm::vec4(0.0f);
}


/**
*	Destructor of DPSLight - decrement number of lights
*
*	@author		DPS
*	@param		void
*	@return		void
*
*/
LIB_API DPSLight::~DPSLight()
{
	DPSLight::turnOff();
}


/**
*	Enable light
*
*	@author		DPS
*	@param		void
*	@return		void
*
*/
void LIB_API DPSLight::turnOn()
{
	isOn = true;
}


/**
*	Disable light
*
*	@author		DPS
*	@param		void
*	@return		void
*
*/
void LIB_API DPSLight::turnOff()
{
	isOn = false;
}


/**
*	Set the type of light
*
*	@author		DPS
*	@param		void
*	@return		void
*
*/
void LIB_API DPSLight::setLightType(LIGHT_TYPE light_type)
{
	this->light_type = light_type;
	if (this->light_type == OMNI)
		this->cutoff = 180.0f;
	else if (this->light_type == SPOT) {
		//Default value for spot cutoff is 15.0f, can be changed with setCutoff
		this->cutoff = 15.0f;
		//Default value for direction is 1.0f, 1.0f, 1.0f, can be changed with setDirection
		this->direction = glm::vec3(1.0f, 1.0f, 1.0f);
	}
}


/**
*	Set color of light
*
*	@author		DPS
*	@param		glm::vec3 col
*	@return		void
*
*/
void LIB_API DPSLight::setColor(glm::vec3 col)
{
	this->color = col;
	this->ambient = glm::vec4(this->color, 1.0f);
	this->diffuse = glm::vec4(this->color, 1.0f);
	this->specular = glm::vec4(this->color, 1.0f);
}


/**
*	Set direction of light
*
*	@author		DPS
*	@param		glm::vec3 direction
*	@return		void
*
*/
void LIB_API DPSLight::setDirection(glm::vec3 direction)
{
	this->direction = direction;
}


/**
*	Set ambient of light
*
*	@author		DPS
*	@param		glm::vec4 ambient
*	@return		void
*
*/
void LIB_API DPSLight::setAmbient(glm::vec4 ambient)
{
	this->ambient = ambient;
}


/**
*	Set cutoff of light
*
*	@author		DPS
*	@param		float cutoff
*	@return		void
*
*/
void LIB_API DPSLight::setCutoff(float cutoff) {
	if (light_type == SPOT) 
		this->cutoff = cutoff;
}


/**
*	Set radius of light
*
*	@author		DPS
*	@param		float radius
*	@return		void
*
*/
void LIB_API DPSLight::setRadius(float radius)
{
	this->lightRadius = radius;
}


/**
*	Get ambient
*
*	@author		DPS
*	@param		void
*	@return		glm::vec4
*
*/
glm::vec4 LIB_API DPSLight::getAmbient()
{
	return ambient;
}


/**
*	Get diffuse
*
*	@author		DPS
*	@param		void
*	@return		glm::vec4
*
*/
glm::vec4 LIB_API DPSLight::getDiffuse()
{
	return diffuse;
}


/**
*	Get specular
*
*	@author		DPS
*	@param		void
*	@return		glm::vec4
*
*/
glm::vec4 LIB_API DPSLight::getSpecular()
{
	return specular;
}


/**
*	Get type of light
*
*	@author		DPS
*	@param		void
*	@return		LIGHT_TYPE
*
*/
LIGHT_TYPE LIB_API DPSLight::getLightType()
{
	return light_type;
}


/**
*	Get type
*
*	@author		DPS
*	@param		void
*	@return		types
*
*/
types LIB_API DPSLight::getType()
{
	return DPSTYPE_LIGHT;
}


/**
*	Get isOn
*
*	@author		DPS
*	@param		void
*	@return		vool
*
*/
bool LIB_API DPSLight::getIsOn()
{
	return isOn;
}


/**
*	Set diffuse
*
*	@author		DPS
*	@param		glm::vec4 diffuse
*	@return		void
*
*/
void LIB_API DPSLight::setDiffuse(glm::vec4 diffuse)
{
	this->diffuse = diffuse;
}


/**
*	Set specular
*
*	@author		DPS
*	@param		glm::vec4 specualr
*	@return		void
*
*/
void LIB_API DPSLight::setSpecular(glm::vec4 specular)
{
	this->specular = specular;
}


/**
*	Set position - difference beetwen type
*
*	@author		DPS
*	@param		glm::vec3 position
*	@return		void
*
*/
void LIB_API DPSLight::setPosition(glm::vec3 position)
{
	if (light_type == OMNI) 
		//Light type is omnidirectional, w component of position must be 1.0f
		this->position = glm::vec4(position, 1.0f);
	else if (light_type == SPOT) 
		//Light type is spot, w component of position must be 1.0f
		this->position = glm::vec4(position, 1.0f);
	else if (light_type == DIRECTIONAL)
		//Light type is directional, w component of position must be 0.0f
		this->position = glm::vec4(position, 0.0f);
}