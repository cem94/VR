#pragma once

#include "Leap.h"
#include "DPSEngine.h"

class LeapMotionListener : public Leap::Listener
{
public:
	LeapMotionListener();

	~LeapMotionListener();

	virtual DPSNode * getGauntlet();

	virtual void setNode(DPSNode * gauntlet);

	virtual void onConnect(const Leap::Controller&);

	virtual void onInit(const Leap::Controller&);

	virtual void onFrame(const Leap::Controller&);

	virtual void onDisconnect(const Leap::Controller&);

	virtual void checkGesture(const Leap::Frame &);

	virtual glm::mat3 segFinalRotation(const Leap::Hand &hand, const Leap::FingerList &fingers, int index, int segment);

	virtual bool thumbTouchIndex(Leap::FingerList);
private:
	float hX, hY, hZ;

	// Hand settings:
	glm::mat4 gauntletMatrix;
	float scalingFactor = 2.0f;
	static const int nrOfCapsules = 15;      ///< The hand model is made of N capsules
	glm::mat3 basePose[nrOfCapsules];   ///< Hand initial, neutral pose
	DPSNode *gauntlet = nullptr;
};