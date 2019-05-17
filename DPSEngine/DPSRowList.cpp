#include "DPSEngine.h"

 DPSRowList::DPSRowList()
{
}



/* SERVE ? */

DPSRowList::DPSRowList(DPSNode* node, glm::mat4 position, DPSMaterial* material)

{



}

 DPSRowList::~DPSRowList()

{



}



DPSNode*  DPSRowList::getNode()

{

	return node;

}



glm::mat4  DPSRowList::getMatrix()

{

	return matrix;

}



DPSMaterial*  DPSRowList::getMaterial()

{

	return material;

}



void  DPSRowList::setNode(DPSNode* node)

{

	this->node = node;

}



void  DPSRowList::setMatrix(glm::mat4)

{

	this->matrix = matrix;

}



void  DPSRowList::setMaterial(DPSMaterial* material)

{

	this->material = material;

}