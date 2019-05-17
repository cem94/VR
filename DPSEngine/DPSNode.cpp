#include "DPSEngine.h"


/**
*	Costructor of DPSNode - set parent to nullptr
*
*	@author		DPS
*	@param		void
*	@return		void
*
*/
LIB_API DPSNode::DPSNode() : parent{ nullptr }{}


/**
*	Destructor of DPSNode
*
*	@author		DPS
*	@param		void
*	@return		void
*
*/
LIB_API DPSNode::~DPSNode(){}


/**
*	Find node by name
*
*	@author		DPS
*	@param		string nodeName
*	@return		DPSNode *
*
*/
LIB_API DPSNode * DPSNode::findChildByName(std::string nodeName)
{
	if (name == nodeName)
		return this;
	else {
		DPSNode * n;
		for (DPSNode * node : children) {
			n = node->findChildByName(nodeName);
			if (n != nullptr)
				return n;
		}
		return nullptr;
	}
}

/**
*	Get matrix
*
*	@author		DPS
*	@param		void
*	@return		glm::mat4
*
*/
glm::mat4 DPSNode::getMatrix()
{
	return this->matrix;
}


/**
*	Get world matrix
*
*	@author		DPS
*	@param		void
*	@return		glm::mat4
*
*/
glm::mat4 DPSNode::getWorldMatrix()
{
	return this->worldMatrix;
}


/**
*	Compute world matrix coordinate for all childrens (recursive)
*
*	@author		DPS
*	@param		void
*	@return		void
*
*/
void LIB_API  DPSNode::computeWorldCoordinates()
{
	if (this->parent != nullptr)
		this->worldMatrix = this->parent->getWorldMatrix() * this->getMatrix();

	if (this->getNumberOfChildren() != 0)
		for (int i = 0; i < this->getNumberOfChildren(); i++)
			this->getChild(i)->computeWorldCoordinates(); //call recursion
}


/**
*	Set matrix
*
*	@author		DPS
*	@param		glm::mat4 matrix
*	@return		void
*
*/
void LIB_API DPSNode::setMatrix(glm::mat4 matrix)
{
	this->matrix = matrix;
}


/**
*	Set matrix from leam motions matrix (professor's code)
*   we only change the rotation of the hand
*
*	@author		DPS
*	@param		glm::mat3 matrix
*	@return		void
*
*/
void DPSNode::setMatrix3x3(glm::mat3 matrix)
{
	this->matrix[0] = glm::vec4(matrix[0], 0.0f);
	this->matrix[1] = glm::vec4(matrix[1], 0.0f);
	this->matrix[2] = glm::vec4(matrix[2], 0.0f);
}


/**
*	Get number of children of DPSNode
*
*	@author		DPS
*	@param		void
*	@return		int
*
*/
int LIB_API DPSNode::getNumberOfChildren()
{
	return children.size();
}


/**
*	Get child of DPSnOde
*
*	@author		DPS
*	@param		int childIndex
*	@return		DPSNode*
*
*/
LIB_API DPSNode*  DPSNode::getChild(int childIndex)
{
	if (childIndex < this->getNumberOfChildren())
		return this->children.at(childIndex);

	throw std::invalid_argument("DPSNode: getChild, invalid childIndex");
}


/**
*	Link DPSNode to another DPSNode - used for create tree structure
*
*	@author		DPS
*	@param		gDPSNode* node
*	@return		void
*
*/
void DPSNode::link(DPSNode *node)
{
	if (node->getType() == DPSTYPE_LIGHT) 
		children.insert(children.begin(), node);
	else
		children.push_back(node);
}


/**
*	Unlink DPSNode to another DPSNode - used for destroy tree structure
*
*	@author		DPS
*	@param		int childIndex
*	@return		void
*
*/
void LIB_API DPSNode::unlink(int childIndex)
{
	if (childIndex < getNumberOfChildren()) {
		children.erase(children.begin() + childIndex);
		return;
	}
	throw std::invalid_argument("DPSNode: unlink, invalid childIndex");
}


/**
*	Set parent DPSNode
*
*	@author		DPS
*	@param		DPSNode* parent
*	@return		void
*
*/
void LIB_API DPSNode::setParent(DPSNode *parent)
{
	this->parent = parent;
}


/**
*	Get parent
*
*	@author		DPS
*	@param		void
*	@return		DPSNode*
*
*/
DPSNode *  DPSNode::getParent()
{
	return parent;
}


/**
*	Rendering of DPSNode - implement into subclasses
*
*	@author		DPS
*	@param		glm::mat4 inverseCameraMatrix
*	@return		void
*
*/
void LIB_API DPSNode::render(glm::mat4 inverseCameraMatrix){}


/**
*	Copy the whole three
*
*	@author		DPS
*	@return		DPSNode *
*
*/
DPSNode * DPSNode::getDeepCopy()
{
	static DPSNode * deepCopy = new DPSNode();
	deepCopy->setMatrix(this->getMatrix());
	deepCopy->setParent(this->getParent());

	if (this->getNumberOfChildren() != 0)
		for (int i = 0; i < this->getNumberOfChildren(); i++)
			this->link(this->getChild(i)->getDeepCopy());  //call recursion

	return deepCopy;
}


/**
*	Get types
*
*	@author		DPS
*	@param		void
*	@return		types
*
*/
types LIB_API DPSNode::getType()
{
	return DPSTYPE_NODE;
}