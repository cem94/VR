#pragma once
#include "DPSEngine.h"


class LIB_API DPSMaterial : public DPSObject
{
public:
	DPSMaterial();
	~DPSMaterial();

	float getShininess();
	void setShininess(float roughness);

	DPSTexture* getTexture();
	void setTexture(DPSTexture* tex);

	float getTrasparency();
	glm::vec3 getAlbedo();
	void setProprierties(glm::vec3 albedo, float alpha);

	glm::vec4 getAmbient();
	void setAmbient(glm::vec4 ambient);

	glm::vec4 getDiffuse();
	void setDiffuse(glm::vec4 diffiuse);

	glm::vec4 getSpecular();
	void setSpecular(glm::vec4 specular);

	bool isTransparent();

private:
	DPSTexture* texture;
	std::string name;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::vec3 albedo;
	float transparency;
	float shininess;
};