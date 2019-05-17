#pragma once

// Generic info:
#define LIB_NAME      "DPSEngine"  ///< Library credits
#define LIB_VERSION   10           ///< Library version (divide by 10)

#ifdef _WINDOWS
#include <windows.h>
// Export API:
// Specifies i/o linkage (VC++ spec):
#ifdef DPSENGINE_EXPORTS
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif
#else // Under Linux
#define LIB_API  // Dummy declaration
#endif

//C++/C
#include <string>
#include <list>
#include <iostream>
#include <vector>
#include <math.h>

// GLM:
#define GLM_FORCE_RADIANS
#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/packing.hpp>

// OpenVR:
#include <openvr.h>

//DPS TYPES
enum types {
	DPSTYPE_OBJECT,
	DPSTYPE_NODE,
	DPSTYPE_MESH,
	DPSTYPE_LIGHT,
	DPSTYPE_CAMERA,
};

//TEXTURE  ANISOTROPY
#define GL_TEXTURE_MAX_ANISOTROPY_EXT        0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT    0x84FF

//Classes
#include "DPSObject.h"
#include "DPSNode.h"
#include "DPSTexture.h"
#include "DPSMaterial.h"
#include "DPSList.h"
#include "DPSCamera.h"
#include "DPSLight.h"
#include "DPSMesh.h"
#include "DPSOvoReader.h"
#include "shader.h"
#include "fbo.h"
#include "ovr.h"
#include "DPSCubeMap.h"



class LIB_API DPSEngine
{
public:
	//load ovo object and create scene
	static DPSNode *load(const char * src);

	//configure
	static void setViewport(int width, int height);
	static void setTexturesPath(std::string path);
	static void swapBuffers();
	static void postWindowRedisplay();
	static void clearColor(float r, float g, float b, float a);
	static void clearScreen();
	static void enableLight();
	static void disableLight();
	static void writeValue(std::string label, float v, glm::mat4 ortho);
	static bool init(int argc, char *argv[]);
	static void setActiveCamera(DPSCamera * camera);


	static DPSCamera * getActiveCamera();
	static void start();
	static void renderScene();
	static void freeEngine();


	//Lists
	static void clearRenderList();
	static void passNodeToRenderList(DPSNode * node);


	//Function
	static void reshape(int width, int height);


	//Callbacks
	static void displayFunc(void(*func)());
	static void reshapeFunc(void(*func)(int, int));
	static void timerFunc(void(*func)(int), int time);
	static void keyboardFunc(void(*func)(unsigned char, int, int));
	static void mouseMotionFunc(void(*func)(int, int));
	static void specialFunc(void(*func)(int, int, int));


	//SHADER FUNCTIONS
	static  void setProjection(glm::mat4 projectionMatrix);
	static  void setModelView(glm::mat4 projectionMatrix);


	static  void buildCubeMap(std::string * filesName);

	static void loadLight(DPSLight * light, glm::mat4 inverseCameraMatrix);
	static void enableLight(bool state);
	static void loadMaterial(DPSMaterial * material);
	static void loadTexture(DPSTexture * texture);

private:
	static int windowID;
	static bool initFlag;

	static DPSEngine* engine;
	static DPSCamera* activeCamera;
	//static DPSCubeMap* cubeMap;

	//Lists
	static DPSList renderList;
	static std::vector<DPSMaterial*> materials;
	static std::vector<DPSTexture*> textures;
};