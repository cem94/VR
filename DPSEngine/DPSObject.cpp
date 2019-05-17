#include "DPSEngine.h"


/**
*	Costructor of DPSObject - increment idGenarator
*
*	@author		DPS
*	@param		void
*	@return		void
*
*/
LIB_API DPSObject::DPSObject()
{
    this->id = idGenerator;
	idGenerator++;
}


/**
*	Costructor of DPSObject - increment idGenarator
*
*	@author		DPS
*	@param		string name
*	@return		void
*
*/
LIB_API DPSObject::DPSObject(std::string name) :  name{name}
{
	this->id = idGenerator;
	idGenerator++;
}


/**
*	Destructor of DPSObject
*
*	@author		DPS
*	@param		void
*	@return		void
*
*/
LIB_API DPSObject::~DPSObject(){}


/**
*	Get id of DPSObject
*
*	@author		DPS
*	@param		void
*	@return		unsigned int
*
*/
unsigned int LIB_API DPSObject::getId()
{
	return this->id;
}


/**
*	Get name of DPSObject
*
*	@author		DPS
*	@param		void
*	@return		string
*
*/
std::string LIB_API DPSObject::getName()
{
	return this->name;
}


/**
*	Set name of DPSObject
*
*	@author		DPS
*	@param		string name
*	@return		void
*
*/
void LIB_API DPSObject::setName(std::string name)
{
    this->name = name;
}


/**
*	Get types of DPSObject
*
*	@author		DPS
*	@param		void
*	@return		types
*
*/
types LIB_API DPSObject::getType()
{
	return DPSTYPE_OBJECT;
}