#pragma once
#include "DPSEngine.h"


class LIB_API	DPSNode : public DPSObject
{
public:
    DPSNode();
	~DPSNode();

	//Parent
	void setParent(DPSNode *parent);
	DPSNode * getParent();

	//Childrens
	DPSNode * findChildByName(std::string nodeName);
	int getNumberOfChildren();
	DPSNode *getChild(int childIndex);

	//Matrix
	void setMatrix(glm::mat4 matrix);
	void setMatrix3x3(glm::mat3 matrix);
	glm::mat4 getMatrix();
	glm::mat4 getWorldMatrix();
	void computeWorldCoordinates(); // compute world matrix --> recursive on children

	//Utility
	virtual types getType();
	void link(DPSNode *node);
	void unlink(int childIndex);

	//Copy
	DPSNode * getDeepCopy();

	virtual void render(glm::mat4 inverseCameraMatrix);

protected:
	glm::mat4 matrix;
	glm::mat4 worldMatrix;
	std::vector<DPSNode *> children;
	DPSNode *parent;
};