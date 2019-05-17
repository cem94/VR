#pragma once
#include "DPSNode.h"


class LIB_API  DPSList : public DPSObject
{
public:
	DPSList();
	~DPSList();

	//Add elements
	void pass(DPSNode *object); // add new element and his hierarchy

	//Utility
	int getSize();
	DPSNode* get(int index);
	void clear();
	void pop(int index);
	unsigned int getNumberOfLights();

	//Render Order
	void push(DPSNode *object);
	void pushBack(DPSNode *object);
	void pushFront(DPSNode *object);
	void insertAt(int index, DPSNode *object);

private:
	unsigned int numberOfLights;

	//Variables
	std::vector<DPSNode *> nodeList;
};