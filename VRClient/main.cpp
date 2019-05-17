/**
 * @file		main.cpp
 * @brief		TheGuantlet
 *
 * @author		Brian Pulfer [brian.pulfer@student.supsi.ch]
 *				Kevin Dominguez [kevin.dominguez@student.supsi.ch]
 *				Cem Koca [cem.koca@student.supsi.ch]
 *
 *				SUPSI DTI STUDENT COMPUTER GRAPHICS PROJECT
 **/

#define _USE_MATH_DEFINES
#include "DPSEngine.h"
#include <math.h>

#include <glm/gtx/string_cast.hpp>

#include "Leap.h"
#include "LeapMotionListener.h"


 ///////////////
 // GLOBAL    //
 ///////////////

glm::mat4 perspective;
glm::mat4 ortho;
glm::mat4 digitRot;
glm::mat4 tumbRot;
glm::mat4 negDigitRot;
glm::mat4 negTumbRot;
glm::mat4 viewMat;
glm::mat4 translation;
glm::mat4 scale;
glm::mat4 rotation;
glm::mat4 f;

glm::vec3 eye;
glm::vec3 center;
glm::vec3 up;


// Leap motion
Leap::Controller controller;
LeapMotionListener listener;

DPSNode* root{ new DPSNode() };
DPSMesh* palm{ new DPSMesh() };
DPSMesh * boxPedestal{ new DPSMesh() };
DPSCamera *mainCamera;
DPSLight * lightSource{ new DPSLight() };
DPSLight * omniMovable{ new DPSLight() };
DPSMesh * clouds{ new DPSMesh() };

bool cameraControl = false;
bool closing = true;
bool movable = true;
bool debug = false;
bool printText = false;

int frames = 0;


DPSNode *gauntlet = nullptr;

float fingerAngle = M_PI_2;
float fingerAngleStep = 0.3f;
float fps = 0.0f;
float r = 0.0f, g = 0.0f, b = 0.0f, a = 1.0f;

double mouseXprev = -1;
double mouseYprev = -1;
double angleZrads = 0; // angle in rad/s  around the z-axis --> 0 == right // PI == left ---> 360° of liberty
double angleXrads = 0;  // angle in rad/s  around the x-axis --> 0 == up // PI == down ---> 180° of liberty
double viewAngleStepRad = 0.007; // rad step per mouse move
double moveSpeed = 0.5;


///////////////
// CALLBACKS //
///////////////
void reshapeCallback(int width, int height) {
	DPSEngine::reshape(width, height);

	DPSEngine::setViewport(width, height);
	perspective = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 1.0f, 100.0f);
	DPSEngine::getActiveCamera()->setProjection(perspective);

	ortho = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);
}

void keyboardCallback(unsigned char key, int x, int y) {

	/*
	switch (key) {
		//flags
		case 'e': cameraControl = !cameraControl; break;
		case 'c': closing = !closing; break;
		case 'i': printText = !printText; break;

		//Camera/Light movement control
		case 'w':
			if (cameraControl && movable) {
				glm::vec3 move((eye.x - center.x) * moveSpeed, (eye.y - center.y) * moveSpeed, (eye.z - center.z) * moveSpeed);
				eye = eye - move;
				center = center - move;
			} else {
				if (omniMovable != nullptr) {
					omniMovable->setMatrix(omniMovable->getMatrix() *  glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 2.0f)));
				}
			}
			break;
		case 'a':
			if (cameraControl && movable) {
				glm::vec3 rotatedcenter = glm::vec3(sin(M_PI_2) * sin(angleZrads - M_PI_2), cos(M_PI_2), cos(angleZrads - M_PI_2)*sin(M_PI_2)) + eye; //calc point to look at
				glm::vec3 move((eye.x - rotatedcenter.x)  * moveSpeed, (eye.y - rotatedcenter.y)  * moveSpeed, (eye.z - rotatedcenter.z) * moveSpeed);
				eye = eye - move;
				center = center - move;
			} else {
				if (omniMovable != nullptr) {
					omniMovable->setMatrix(omniMovable->getMatrix() * glm::translate(glm::mat4(), glm::vec3(-2.0f, 0.0f, 0.0f)));
				}
			}
			break;
		case 's':
			if (cameraControl && movable) {
				glm::vec3 move((eye.x - center.x) * moveSpeed, (eye.y - center.y) * moveSpeed, (eye.z - center.z) * moveSpeed);
				eye = eye + move;
				center = center + move;
			} else {
				if (omniMovable != nullptr) {
					omniMovable->setMatrix(omniMovable->getMatrix() * glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -2.0f)));
				}
			}
			break;
		case 'd':
			if (cameraControl && movable) {
				glm::vec3 rotatedcenter = glm::vec3(sin(M_PI_2) * sin(angleZrads + M_PI_2), cos(M_PI_2), cos(angleZrads + M_PI_2)*sin(M_PI_2)) + eye; //calc point to lock at
				glm::vec3 move((eye.x - rotatedcenter.x) * moveSpeed, (eye.y - rotatedcenter.y) * moveSpeed, (eye.z - rotatedcenter.z) * moveSpeed);
				eye = eye - move;
				center = center - move;
			} else {
				if (omniMovable != nullptr) {
					omniMovable->setMatrix(omniMovable->getMatrix() * glm::translate(glm::mat4(), glm::vec3(2.0f, 0.0f, 0.0f)));
				}
			}
			break;
		case 'r':
			if (!cameraControl) {
				if (omniMovable != nullptr) {
					omniMovable->setMatrix(omniMovable->getMatrix() * glm::translate(glm::mat4(), glm::vec3(0.0f, 2.0f, 0.0f)));
				}
			}
			break;
		case 'f':
			if (!cameraControl) {
				if (omniMovable != nullptr) {
					omniMovable->setMatrix(omniMovable->getMatrix() *  glm::translate(glm::mat4(), glm::vec3(0.0f, -2.0f, 0.0f)));
				}
			}
			break;

		//Movable
		case '8':
			movable = true;
			DPSEngine::setActiveCamera(cameraMovable);
			break;
	}

	*/

	DPSEngine::postWindowRedisplay();
}

void displayCallback()
{
	DPSEngine::clearScreen();

	//Camera
	/*
	mainCamera->setCenter(center);
	mainCamera->setEyes(eye);
	viewMat = glm::lookAt(eye, center, up);
	mainCamera->setMatrix(viewMat);
	*/



	DPSEngine::clearRenderList();
	DPSEngine::passNodeToRenderList(root);


	//position guantlet in front of camera
	glm::mat4 position = gauntlet->getMatrix();
	glm::mat4 camera = DPSEngine::getActiveCamera()->getMatrix();
	position[3] = camera[3];
	float translationDistance = 3.0f;
	translation = glm::translate(glm::mat4(), translationDistance*glm::vec3(camera[0].x, camera[0].y, camera[0].z));
	gauntlet->setMatrix(translation*position);


	DPSEngine::renderScene();

	//if(printText){
		//DPSEngine::writeValue("FPS: ", fps, ortho);
	//}

	DPSEngine::swapBuffers();
	DPSEngine::postWindowRedisplay();
	frames++;
}

void timerCallback(int value)
{
	if (lightSource != nullptr) {
		lightSource->setMatrix(glm::rotate(glm::mat4(1.0f), glm::radians(0.1f), glm::vec3(0.0f, 1.0f, 0.0f)) * lightSource->getMatrix());
	}

	if (clouds != nullptr) {
		clouds->setMatrix(glm::rotate(glm::mat4(1.0f), glm::radians(-0.07f), glm::vec3(0.0f, 1.0f, 0.0f)) * clouds->getMatrix());
	}

	DPSEngine::timerFunc(timerCallback, 10);
}

void fpsCallback(int value)
{
	fps = frames / 1.0f;
	frames = 0;
	DPSEngine::timerFunc(fpsCallback, 1000);
}

void mouseCallback(int x, int y)
{
	if (cameraControl) {
		if (mouseXprev == -1 || mouseYprev == -1) {
			mouseXprev = x;
			mouseYprev = y;
		}
		else {
			double xDif = abs(x - mouseXprev);
			double yDif = abs(y - mouseYprev);
			if (xDif > 25 || yDif > 25) {
				xDif = 25;
				yDif = 25;
			}
			//cerr << "mouse x : " << x << "mouse y : "  << y <<endl;

			if (x <= mouseXprev) {
				angleZrads = (angleZrads + xDif * viewAngleStepRad);
			}
			else {
				angleZrads = (angleZrads - xDif * viewAngleStepRad);
			}
			if (y <= mouseYprev) {
				angleXrads = (angleXrads + yDif * viewAngleStepRad);
			}
			else {
				angleXrads = (angleXrads - yDif * viewAngleStepRad);
			}

			//Check boundaries
			if (angleXrads >= 0.01f) {  // rad == 0 --->
				angleXrads = 0.01f;
			}
			else if (angleXrads <= -M_PI + 0.01f) {
				angleXrads = -M_PI + 0.01f;
			}
			if (angleZrads >= 2 * M_PI) {
				angleZrads = 0;
			}
			else if (angleZrads <= 0) {
				angleZrads = 2 * M_PI;
			}

			center = glm::vec3(sin(angleXrads) * sin(angleZrads), cos(angleXrads), cos(angleZrads)*sin(angleXrads)) + eye; //calc point to lock at

			mouseXprev = x;
			mouseYprev = y;
			DPSEngine::postWindowRedisplay();
		}
	}
}

int main(int argc, char *argv[]) {
	//init contest

	DPSEngine::init(argc, argv);
	DPSEngine::setTexturesPath("../../resources/");
	DPSEngine::clearColor(r, g, b, a);

	//load custom 3d scene model
	root = DPSEngine::load("../../resources/sceneBis.OVO");


	gauntlet = root->findChildByName("Guanto");

	listener.setNode(gauntlet);

	if (controller.isConnected())
		controller.addListener(listener);

	if (root == nullptr) {
		std::cout << "Failed to read file" << std::endl;
		return 0;
	}

	//callbacks
	DPSEngine::displayFunc(displayCallback);
	DPSEngine::reshapeFunc(reshapeCallback);
	DPSEngine::timerFunc(timerCallback, 10);
	DPSEngine::timerFunc(fpsCallback, 1000);
	DPSEngine::keyboardFunc(keyboardCallback);
	DPSEngine::mouseMotionFunc(mouseCallback);

	//lightSource = static_cast<DPSLight*>((root->findChildByName("MoonLightDirectional")));

	//disable light for skybox
	//dynamic_cast<DPSMesh*>(root->findChildByName("SkyrimStars:0"))->useLight(false);
	dynamic_cast<DPSMesh*>(root->findChildByName("SkyrimGalaxy"))->useLight(false);
	dynamic_cast<DPSMesh*>(root->findChildByName("Bloodmoon"))->useLight(false);

	//load palm and fingers
	palm = dynamic_cast<DPSMesh*> (root->findChildByName("Palmo"));

	clouds = static_cast<DPSMesh*>((root->findChildByName("CloudOuter")));
	lightSource = static_cast<DPSLight*>((root->findChildByName("MoonLightDirectional")));
	//omniMovable = static_cast<DPSLight*>((root->findChildByName("Omni")));

	//DPSMesh * relief = dynamic_cast<DPSMesh*> (root->findChildByName("Relief"));
	//DPSMesh * brazier = dynamic_cast<DPSMesh*> (root->findChildByName("Brazier"));
	//DPSMesh * sea = dynamic_cast<DPSMesh*> (root->findChildByName("Sea"));

	
	//build Cube Map
	std::string cubemapNames[6] =
	{
	   "../../resources/posx.jpg",
	   "../../resources/negx.jpg",
	   "../../resources/posy.jpg",
	   "../../resources/negy.jpg",
	   "../../resources/posz.jpg",
	   "../../resources/negz.jpg",
	};
	DPSEngine::buildCubeMap(cubemapNames);


	//(sea)->getMaterial()->setSpecular(glm::vec4(1.0f, 1.0f , 1.0f, 5.0f));
	
	(palm)->getMaterial()->setAmbient(glm::vec4(palm->getMaterial()->getAlbedo()*2.0f, 1.0f));
	(palm)->getMaterial()->setDiffuse(glm::vec4(palm->getMaterial()->getAlbedo()*2.0f, 1.0f));
	(palm)->getMaterial()->setSpecular(glm::vec4(palm->getMaterial()->getAlbedo()*2.0f, 1.0f));

	/*
	(relief)->getMaterial()->setAmbient(glm::vec4(palm->getMaterial()->getAlbedo()*0.05f, 1.0f));
	(relief)->getMaterial()->setDiffuse(glm::vec4(palm->getMaterial()->getAlbedo(), 1.0f));
	(relief)->getMaterial()->setSpecular(glm::vec4(palm->getMaterial()->getAlbedo(), 1.0f));
	*/
	//set initial position
	//translation = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f));
	//scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
	//rotation = glm::rotate(glm::mat4(), glm::radians(0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	//f = translation * rotation * scale;
	//up = glm::vec3(0.0f, 1.0f, 0.0f); // do not touch

	//root->setMatrix(root->getMatrix() * scale);
	//Init Movable


	//eye = glm::vec3(10.0f, 10.0f, 10.0f);
	//center = (palm->getWorldMatrix())[3] * f;
	//viewMat = glm::lookAt(eye, center, up) * f;

	/*
	mainCamera = new DPSCamera();
	mainCamera->setName("MainCamera");
	mainCamera->setParent(root);
	mainCamera->setMatrix(viewMat);
	*/
	//default camera
	//DPSEngine::setActiveCamera(mainCamera);

	//start the application loop
	DPSEngine::start();

	//free allocated resources
	DPSEngine::freeEngine();

	return 0;
}