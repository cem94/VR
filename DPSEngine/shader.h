#pragma once
#include "DPSEngine.h"


// Commodity macro:
#define SHADER_STRINGIFY(x) #x


class Shader
{
public:
	// Constants:
	static const unsigned int MAX_LOGSIZE = 4096;  ///< Max output size in char for a shader log

	 // Enums:
	enum ///< Kind of shader
	{
		TYPE_UNDEFINED = 0,
		TYPE_VERTEX,
		TYPE_FRAGMENT,
		TYPE_PROGRAM,
		TYPE_LAST
	};

	// Const/dest:	 
	Shader();
	~Shader();

	// Get/set:
	int getParamLocation(const char *name);
	void setMatrix(int param, const glm::mat4 &mat);

	void setMatrix3(int param, const glm::mat3 &mat);

	void setFloat(int param, float value);

	void setInt(int param, int value);

	void setVec3(int param, const glm::vec3 &vect);

	void setVec4(int param, const glm::vec4 &vect);

	// Accessing data:
	bool loadFromMemory(int kind, const char *data);
	bool loadFromFile(int kind, const char *filename);
	bool build(Shader *vertexShader, Shader *fragmentShader);

	void bind(int location, const char *attribName);

	// Rendering:				
	bool render(void *data = nullptr);

private:
	// Object kind:
	int type;

	// OGL id:
	unsigned int glId;
};