#include "DPSEngine.h"


/**
*	Costructor of DPSList - set number of light 0
*
*	@author		DPS
*	@param		void
*	@return		void
*
*/
DPSList::DPSList() : numberOfLights{ 0 } {}


/**
*	Destructor of DPSList
*
*	@author		DPS
*	@param		void
*	@return		void
*
*/
DPSList::~DPSList() {
	for (int i = 0; i < getSize(); i++)
		free(this->nodeList.at(i));
}

/**
*	Create the whole three (recursive)
*
*	@author		DPS
*	@param		DPSNode *
*	@return		void
*
*/
void DPSList::pass(DPSNode * object)
{
	this->push(object);

	if (object->getNumberOfChildren() != 0)
		for (int i = 0; i < object->getNumberOfChildren(); i++)
			this->pass(object->getChild(i));  //call recursion
}


/**
*	Get size of DPSList - number of elements
*
*	@author		DPS
*	@param		void
*	@return		int
*
*/
int DPSList::getSize()
{
	return this->nodeList.size();
}


/**
*	Get element at index of DPSList
*
*	@author		DPS
*	@param		int index
*	@return		DPSNode *
*
*/
DPSNode* DPSList::get(int index)
{
	if (index < getSize())
		return nodeList.at(index);

	throw std::invalid_argument("DPSList: invalid index");
}

/**
*	Push new element into DPSList - difference beetwen types (DPSLight -> DPSMesh --> DPSCamera)
*
*	@author		DPS
*	@param		DPSNode *
*	@return		void
*
*/
void LIB_API DPSList::push(DPSNode *node)
{
	types type = node->getType();

	switch (type) {
		//push front
		case DPSTYPE_LIGHT: {
			pushFront(node);
			numberOfLights++;
			break;
		}
		//push past lights
		case DPSTYPE_MESH: {
			insertAt(numberOfLights, node);
			break;
		}
		//push back
		case DPSTYPE_CAMERA: {
			nodeList.push_back(node);
			break;
		}
		//DO NOTHING
		case DPSTYPE_NODE: {
			break;
		}
		default: {
			std::cout << "INVALID TYPE" << std::endl;
		}
	}
}


/**
*	Push back
*
*	@author		DPS
*	@param		DPSNode * object
*	@return		void
*
*/
void LIB_API DPSList::pushBack(DPSNode * object)
{
	nodeList.push_back(object);
}


/**
*	Push front
*
*	@author		DPS
*	@param		DPSNode * object
*	@return		void
*
*/
void LIB_API DPSList::pushFront(DPSNode * object)
{
	nodeList.insert(nodeList.begin(), object);
}


/**
*	Clear
*
*	@author		DPS
*	@param		void
*	@return		void
*
*/
void LIB_API DPSList::clear()
{
	numberOfLights = 0;
	nodeList.clear();
}


/**
*	Insert at
*
*	@author		DPS
*	@param		DPSNode ç object
*	@return		void
*
*/
void DPSList::insertAt(int index, DPSNode * object)
{
	nodeList.insert(nodeList.begin() + index, object);
}


/**
*	Pop
*
*	@author		DPS
*	@param		int index
*	@return		void
*
*/
void DPSList::pop(int index)
{
	if (index < getSize()) {
		nodeList.erase(nodeList.begin() + index);
		return;
	}
	throw std::invalid_argument("DPSList: invalid index");
}


/**
*	return the number of lights
*
*	@author		DPS
*	@param		void
*	@return		int
*
*/
unsigned int DPSList::getNumberOfLights()
{
	return this->numberOfLights;
}