#pragma once
#include "DPSEngine.h"


class LIB_API DPSOvoReader
{
public:
	DPSOvoReader(const char * src);
	~DPSOvoReader();
	DPSNode *load(std::vector<DPSTexture*> *textures, std::vector<DPSMaterial*> *materials);

private:
	void readChunk(FILE* dat, DPSNode* parent, std::vector<DPSTexture*> *textures, std::vector<DPSMaterial*> *materials);

	DPSNode *rootNode;
	const char * src;
};