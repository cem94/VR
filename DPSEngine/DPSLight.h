#pragma once
#include "DPSEngine.h"


//Type of the light
typedef enum : int {
	OMNI, DIRECTIONAL, SPOT
} LIGHT_TYPE;


class LIB_API DPSLight : public DPSNode
{
public:
	DPSLight();
	~DPSLight();

	void  turnOn();
	void  turnOff();

	void  setLightType(LIGHT_TYPE light_type);
	void  setColor(glm::vec3 col);
	void  setAmbient(glm::vec4 ambient);
	void  setDiffuse(glm::vec4 diffuse);
	void  setSpecular(glm::vec4 specular);
	void  setPosition(glm::vec3 position);
	void  setDirection(glm::vec3 direction);
	void  setCutoff(float cutoff);
	void  setRadius(float radius);;

	glm::vec4  getAmbient();
	glm::vec4  getDiffuse();
	glm::vec4  getSpecular();
	LIGHT_TYPE  getLightType();
	types getType();
	bool  getIsOn();


private:
	LIGHT_TYPE light_type;
	bool isOn;
	glm::vec3 color;
	glm::vec4 position;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::vec3 direction;
	float lightRadius;
	float cutoff;
};