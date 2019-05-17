#include "DPSEngine.h"

// Glew (include it before GL.h):
#include <GL/glew.h>

// FreeGLUT:
#include <GL/freeglut.h>


/**
*	Costructor of DPSMaterial
*
*	@author		DPS
*	@param		void
*	@return		void
*
*/
LIB_API DPSMaterial::DPSMaterial() : texture{nullptr}{}


/**
*	Destructor of DPSMaterial
*
*	@author		DPS
*	@param		void
*	@return		void
*
*/
LIB_API DPSMaterial::~DPSMaterial(){}


/**
*	Set shininess of DPSMaterial
*
*	@author		DPS
*	@param		float roughness
*	@return		void
*
*/
LIB_API void DPSMaterial::setShininess(float roughness)
{
    shininess = (1 - sqrt(roughness)) * 128;
}

/**
*	Set ambient/diffuse/specular of DPSMaterial with alpha
*
*	@author		DPS
*	@param		glm::vec3 albedo, float trasparency
*	@return		void
*
*/
void DPSMaterial::setProprierties(glm::vec3 albedo , float alpha)
{
	this->albedo = albedo;
	this->transparency = alpha;
	this->ambient = glm::vec4(albedo *0.3f ,this->transparency);
	this->diffuse = glm::vec4(albedo *0.5f , this->transparency);
	this->specular = glm::vec4(albedo *0.6f,this->transparency);
}


/**
*	Set ambient
*
*	@author		DPS
*	@param		glm::vec4 ambient
*	@return		void
*
*/
void DPSMaterial::setAmbient(glm::vec4 ambient)
{
	this->ambient = ambient;
}


/**
*	Set diffuse
*
*	@author		DPS
*	@param		glm::vec4 diffuse
*	@return		void
*
*/
void DPSMaterial::setDiffuse(glm::vec4 diffiuse)
{
	this->diffuse = diffiuse;
}


/**
*	Set specular
*
*	@author		DPS
*	@param		glm::vec4 specular
*	@return		void
*
*/
void DPSMaterial::setSpecular(glm::vec4 specular){
	this->specular = specular;
}


/**
*	Set texture
*
*	@author		DPS
*	@param		DPSTexture * tex
*	@return		void
*
*/
void DPSMaterial::setTexture(DPSTexture * tex){
    this->texture = tex;
}


/**
*	Get ambient of DPSMaterial
*
*	@author		DPS
*	@param		void
*	@return		glm::vec4
*
*/
glm::vec4 LIB_API DPSMaterial::getAmbient()
{ 
	return this->ambient; 
}


/**
*	Get diffuse of DPSMaterial
*
*	@author		DPS
*	@param		void
*	@return		glm::vec4
*
*/
glm::vec4 LIB_API DPSMaterial::getDiffuse() 
{
	return this->diffuse; 
}

/**
*	Get specular of DPSMaterial
*
*	@author		DPS
*	@param		void
*	@return		glm::vec4
*
*/
glm::vec4 LIB_API DPSMaterial::getSpecular()
{ 
	return this->specular; 
}


/**
*	Get shininess of DPSMaterial
*
*	@author		DPS
*	@param		void
*	@return		float
*
*/
float LIB_API DPSMaterial::getShininess()
{ 
	return this->shininess; 
}


/**
*	Get texture of DPSMaterial
*
*	@author		DPS
*	@param		void
*	@return		DPSTexture*
*
*/
DPSTexture LIB_API * DPSMaterial::getTexture() 
{ 
	return this->texture; 
}


/**
*	Get trasparency
*
*	@author		DPS
*	@param		void
*	@return		float
*
*/
float LIB_API DPSMaterial::getTrasparency()
{
	return this->transparency;
}


/**
*	Get albedo
*
*	@author		DPS
*	@param		void
*	@return		glm::vec3
*
*/
glm::vec3 LIB_API DPSMaterial::getAlbedo()
{
	return this->albedo;
}


/**
*	Check if is trasparent
*
*	@author		DPS
*	@param		void
*	@return		bool
*
*/
bool DPSMaterial::isTransparent()
{
	if(this->getTrasparency() == 1.0f)
		return false;
	return true;
}