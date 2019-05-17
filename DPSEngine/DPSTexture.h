#include "DPSEngine.h"


class LIB_API DPSTexture : public DPSObject
{
public:
	DPSTexture();
	~DPSTexture();

	void loadTexture(const char* fileName);
	static void setPath(std::string path);
	static void initializeFreeImage();
	static void deinitializeFreeImage();

	unsigned int textureId;

private:
	static std::string texPath;
};