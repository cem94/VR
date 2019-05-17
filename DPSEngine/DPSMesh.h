#include "DPSEngine.h"


class LIB_API DPSMesh : public DPSNode
{
public:
	DPSMesh();
	~DPSMesh();

	DPSMaterial * getMaterial();

	void setMaterial(DPSMaterial* mat);
	void render(glm::mat4 inverseCameraMatrix) override;
	bool isTransparent();
	bool isLightEnabled();
	void useLight(bool state);
	types getType();

	//VBO
	unsigned int numberOfVertices = 0;

	unsigned int meshVao;

	unsigned int vertexVbo;
	unsigned int normalVbo;
	unsigned int textureVbo;

	unsigned int facesVbo;
	unsigned int numberOfFaces = 0;

private:
	DPSMaterial* material;
	bool light = true;
};