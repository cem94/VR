#pragma once
#include "DPSEngine.h"

/*

************************************

*		  *			  *			   *

*   Node  *   Matrix  *  Material  *

*		  *			  *			   *

************************************

*/


/*

IN DPSList AGGIUNGERE
private:
	std::list<DPSRowList> rows;

public:

	DPSRowList add(DPSNode* node, glm::mat4 matrix, DPSMaterial* material); // aggiungere una row

	void remove(DPSRowList row); Gestire in qualche modo con un index/oggetto

	void clear(); // rimuove tutto dalla lista
*/

class LIB_API DPSRowList
{
public:
	DPSRowList();
	DPSRowList(DPSNode* node, glm::mat4 position, DPSMaterial* material); // SERVE ?
	~DPSRowList();

	DPSNode* getNode() {};
	glm::mat4 getMatrix() {};

	DPSMaterial* getMaterial() {};

	void setNode(DPSNode* node) {};
	void setMatrix(glm::mat4 matrix) {};
	void setMaterial(DPSMaterial* material) {};

private:
	DPSNode* node;
	glm::mat4 matrix;
	DPSMaterial* material;
};