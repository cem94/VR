#pragma once
#include "DPSEngine.h"


static unsigned int idGenerator = 0;


class LIB_API DPSObject
{
public:
	DPSObject();
	DPSObject(std::string name);
	~DPSObject();

	unsigned int getId();

	void setName(std::string name);
	std::string getName();

	virtual types getType();
protected:
	unsigned int id;
	std::string name;
};