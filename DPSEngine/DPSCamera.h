#pragma once
#include "DPSEngine.h"


class LIB_API DPSCamera : public DPSNode
{
public:
	DPSCamera();
	~DPSCamera();

	glm::vec3 getEyes();
	void setEyes(glm::vec3 eyes);

	glm::vec3 getCenter();
	void setCenter(glm::vec3 center);

	glm::vec3 getUp();
	void setUp(glm::vec3 up);

	glm::mat4 getProjection();
	void setProjection(glm::mat4  projetion);

	glm::vec3 getNormalDirectionVector();
	glm::mat4 getInverseMatrix();
	types getType();

private:
	glm::vec3 eyes;
	glm::vec3 center;
	glm::vec3 up;
	glm::mat4 projection;
	glm::vec3 normalDirectionVector;
};