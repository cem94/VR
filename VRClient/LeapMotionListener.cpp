#include "LeapMotionListener.h"


LeapMotionListener::LeapMotionListener()
{
	std::cout << "LeapMotionListener init" << std::endl;
}

LeapMotionListener::~LeapMotionListener()
{
	std::cout << "LeatpMotionListnere destroyed" << std::endl;
}

DPSNode * LeapMotionListener::getGauntlet()
{
	return this->gauntlet;
}

void LeapMotionListener::setNode(DPSNode * gauntlet)
{
	this->gauntlet = gauntlet;

	this->gauntletMatrix = gauntlet->getMatrix();

	/* TEST glm conversions from mat 4 to mat3
	glm::mat4 tmp4;
	for (int s = 0; s < 4; s++)
		for (int t = 0; t < 4; t++)
			tmp4[s][t] = s * 4 + t;

	glm::mat3 tmp3(tmp4);
	*/

	// Thumb
	basePose[13] = gauntlet->findChildByName("PolliceFalangeBase")->getMatrix();
	basePose[14] = gauntlet->findChildByName("PolliceFalangePunta")->getMatrix();

	// Pinky:
	basePose[1] = gauntlet->findChildByName("MignoloFalangeBase")->getMatrix();
	basePose[5] = gauntlet->findChildByName("MignoloFalangeMedia")->getMatrix();
	basePose[9] = gauntlet->findChildByName("MignoloFalangePunta")->getMatrix();

	// Ring:
	basePose[2] = gauntlet->findChildByName("AnulareFalangeBase")->getMatrix();
	basePose[6] = gauntlet->findChildByName("AnulareFalangeMedia")->getMatrix();
	basePose[10] = gauntlet->findChildByName("AnulareFalangePunta")->getMatrix();

	// Middle:
	basePose[3] = gauntlet->findChildByName("MedioFalangeBase")->getMatrix();
	basePose[7] = gauntlet->findChildByName("MedioFalangeMedia")->getMatrix();
	basePose[11] = gauntlet->findChildByName("MedioFalangePunta")->getMatrix();

	// Index:
	basePose[4] = gauntlet->findChildByName("IndiceFalangeBase")->getMatrix();
	basePose[8] = gauntlet->findChildByName("IndiceFalangeMedia")->getMatrix();
	basePose[12] = gauntlet->findChildByName("IndiceFalangePunta")->getMatrix();
}

void LeapMotionListener::onConnect(const Leap::Controller& c)
{
	std::cout << "Leap Motion connected (enable Circle, Key tap, Screen tap and Swipe gestures)" << std::endl;
	// Gesture explanation
	// https://developer-archive.leapmotion.com/documentation/v2/csharp/devguide/Leap_Gestures.html
	c.enableGesture(Leap::Gesture::TYPE_CIRCLE);
	c.enableGesture(Leap::Gesture::TYPE_KEY_TAP);
	c.enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);
	c.enableGesture(Leap::Gesture::TYPE_SWIPE);

	// if you want to change some gesture settings
	// https://developer-archive.leapmotion.com/documentation/java/api/Leap.Config.html
	// c.config().setFloat("Gesture.Swipe.MinLength", 200.0f);
	// c.config().save();
}

void LeapMotionListener::onInit(const Leap::Controller &)
{
	std::cout << "Leap Motion init" << std::endl;
}

void LeapMotionListener::onFrame(const Leap::Controller &c)
{
	// Get the frame 
	Leap::Frame frame = c.frame();

	// Get both hands (left, right)
	Leap::HandList hL = frame.hands();

	for (Leap::HandList::const_iterator hI = hL.begin(); hI != hL.end(); hI++) {
		// get hand
		auto leapHand = (*hI);

		// check wich hand is
		if (leapHand.isLeft()) {
			// std::cout << "Left hand" << std::endl;
			continue;
		}
		else if (leapHand.isRight()) {
			// std::cout << "Right hand" << std::endl;
		}

		// Hand position:
		hX = (leapHand.palmPosition().x / scalingFactor);
		hY = (leapHand.palmPosition().y / scalingFactor) - 40.0f;
		hZ = (leapHand.palmPosition().z / scalingFactor);

		Leap::Matrix m = leapHand.basis();
		memcpy(&gauntletMatrix, &m.toArray4x4(), sizeof(glm::mat4));
		Leap::Vector wvec = leapHand.palmPosition();
		gauntletMatrix[3] = glm::vec4(wvec.x / scalingFactor, wvec.y / scalingFactor - 70.0f, wvec.z / scalingFactor, 1.0f); // Some harcoded offsets

		gauntlet->setMatrix(gauntletMatrix);

		// Index:         
		// OvMatrix4 inv = handMatrix;
		// inv[3] = OvVector4(0.0f, 0.0f, 0.0f, 1.0f);
		// inv = glm::transpose(inv);
		Leap::FingerList fingers = leapHand.fingers();

		// Thumb:
		gauntlet->findChildByName("PolliceFalangeBase")->setMatrix3x3(basePose[13] * segFinalRotation(leapHand, fingers, 0, Leap::Bone::TYPE_INTERMEDIATE));
		gauntlet->findChildByName("PolliceFalangePunta")->setMatrix3x3(basePose[14] * segFinalRotation(leapHand, fingers, 0, Leap::Bone::TYPE_DISTAL));

		//auto fingMatrix = hand->findChildByName("PolliceFalangePunta")->getMatrix();
		//std::cout << "Thumb: " << std::endl << "\t [X] " << fingMatrix[0][0] << ",  " << fingMatrix[0][1] << ",  " << fingMatrix[0][2] << "\t [Y] " << fingMatrix[1][0] << ",  " << fingMatrix[1][1] << ",  " << fingMatrix[1][2] << "\t [Z] " << fingMatrix[2][0] << ",  " << fingMatrix[2][1] << ",  " << fingMatrix[2][2] << std::endl;

		// Pinky:
		gauntlet->findChildByName("MignoloFalangeBase")->setMatrix3x3(basePose[1] * segFinalRotation(leapHand, fingers, 4, Leap::Bone::TYPE_PROXIMAL));
		gauntlet->findChildByName("MignoloFalangeMedia")->setMatrix3x3(basePose[5] * segFinalRotation(leapHand, fingers, 4, Leap::Bone::TYPE_INTERMEDIATE));
		gauntlet->findChildByName("MignoloFalangePunta")->setMatrix3x3(basePose[9] * segFinalRotation(leapHand, fingers, 4, Leap::Bone::TYPE_DISTAL));

		//fingMatrix = hand->findChildByName("MignoloFalangePunta")->getMatrix();
		//std::cout << "Pinky: " << std::endl << "\t [X] " << fingMatrix[0][0] << ",  " << fingMatrix[0][1] << fingMatrix[0][2] << "\t [Y] " << fingMatrix[1][0] << ",  " << fingMatrix[1][1] << fingMatrix[1][2] << "\t [Z] " << fingMatrix[2][0] << ",  " << fingMatrix[2][1] << ",  " << fingMatrix[2][2] << std::endl;

		// Ring:
		gauntlet->findChildByName("AnulareFalangeBase")->setMatrix3x3(basePose[2] * segFinalRotation(leapHand, fingers, 3, Leap::Bone::TYPE_PROXIMAL));
		gauntlet->findChildByName("AnulareFalangeMedia")->setMatrix3x3(basePose[6] * segFinalRotation(leapHand, fingers, 3, Leap::Bone::TYPE_INTERMEDIATE));
		gauntlet->findChildByName("AnulareFalangePunta")->setMatrix3x3(basePose[10] * segFinalRotation(leapHand, fingers, 3, Leap::Bone::TYPE_DISTAL));

		//fingMatrix = hand->findChildByName("AnulareFalangePunta")->getMatrix();
		//std::cout << "Ring: " << std::endl << "\t [X] " << fingMatrix[0][0] << ",  " << fingMatrix[0][1] << fingMatrix[0][2] << "\t [Y] " << fingMatrix[1][0] << ",  " << fingMatrix[1][1] << fingMatrix[1][2] << "\t [Z] " << fingMatrix[2][0] << ",  " << fingMatrix[2][1] << ",  " << fingMatrix[2][2] << std::endl;

		// Middle:
		gauntlet->findChildByName("MedioFalangeBase")->setMatrix3x3(basePose[3] * segFinalRotation(leapHand, fingers, 2, Leap::Bone::TYPE_PROXIMAL));
		gauntlet->findChildByName("MedioFalangeMedia")->setMatrix3x3(basePose[7] * segFinalRotation(leapHand, fingers, 2, Leap::Bone::TYPE_INTERMEDIATE));
		gauntlet->findChildByName("MedioFalangePunta")->setMatrix3x3(basePose[11] * segFinalRotation(leapHand, fingers, 2, Leap::Bone::TYPE_DISTAL));

		//fingMatrix = hand->findChildByName("MedioFalangePunta")->getMatrix();
		//std::cout << "Middle: " << std::endl << "\t [X] " << fingMatrix[0][0] << ",  " << fingMatrix[0][1] << fingMatrix[0][2] << "\t [Y] " << fingMatrix[1][0] << ",  " << fingMatrix[1][1] << fingMatrix[1][2] << "\t [Z] " << fingMatrix[2][0] << ",  " << fingMatrix[2][1] << ",  " << fingMatrix[2][2] << std::endl;

		// Index:
		gauntlet->findChildByName("IndiceFalangeBase")->setMatrix3x3(basePose[4] * segFinalRotation(leapHand, fingers, 1, Leap::Bone::TYPE_PROXIMAL));
		gauntlet->findChildByName("IndiceFalangeMedia")->setMatrix3x3(basePose[8] * segFinalRotation(leapHand, fingers, 1, Leap::Bone::TYPE_INTERMEDIATE));
		gauntlet->findChildByName("IndiceFalangePunta")->setMatrix3x3(basePose[12] * segFinalRotation(leapHand, fingers, 1, Leap::Bone::TYPE_DISTAL));

		//fingMatrix = hand->findChildByName("IndiceFalangePunta")->getMatrix();
		//std::cout << "Index: " << std::endl << "\t [X] " << fingMatrix[0][0] << ",  " << fingMatrix[0][1] << fingMatrix[0][2] << "\t [Y] " << fingMatrix[1][0] << ",  " << fingMatrix[1][1] << fingMatrix[1][2] << "\t [Z] " << fingMatrix[2][0] << ",  " << fingMatrix[2][1] << ",  " << fingMatrix[2][2] << std::endl;

		// Check if Thumb and Index touch each other
		thumbTouchIndex(fingers);
	}

	// TODO:: se dovesse funzionare avvisare CEM
	Leap::GestureList gestures = frame.gestures();
	if (frame.gestures().count() || gestures.count())
		std::cout << "recognized gesture" << std::endl;


	// check if there are any valids gestures -> not work
	// checkGesture(frame);
}

void LeapMotionListener::onDisconnect(const Leap::Controller &)
{
	std::cout << "Leap Motion disconnected" << std::endl;
}

// Check if there are any valids gestures
void LeapMotionListener::checkGesture(const Leap::Frame &f)
{
	Leap::GestureList gestures = f.gestures();
	for (Leap::GestureList::const_iterator gl = gestures.begin(); gl != f.gestures().end(); gl++)
	{
		const Leap::Gesture gesture = (*gl);
		// Gesture is terminate
		if (gesture.state() == Leap::Gesture::STATE_STOP) {
			switch (gesture.type()) {
			case Leap::Gesture::TYPE_CIRCLE:
				//Handle circle gestures
				break;
			case Leap::Gesture::TYPE_KEY_TAP:
				//Handle key tap gestures
				break;
			case Leap::Gesture::TYPE_SCREEN_TAP:
				//Handle screen tap gestures
				break;
			case Leap::Gesture::TYPE_SWIPE:
				//Leap::Vector direction = Leap::SwipeGesture(*gl).direction();
				//float speed = Leap::SwipeGesture(*gl).speed();

				//std::cout << "Gesture Swipe:" << std::endl << "\tspeed = " << speed << std::endl << "\tdirection = " << direction << std::endl;
				// Call function
				break;
			default:
				//Handle unrecognized gestures
				break;
			}
		}
	}
}

glm::mat3 LeapMotionListener::segFinalRotation(const Leap::Hand & hand, const Leap::FingerList & fingers, int index, int segment)
{
	// Node matrix:
	Leap::Matrix lpFingerMatrix;
	glm::mat3 fingerMatrix;
	lpFingerMatrix = fingers[index].bone((Leap::Bone::Type) segment).basis();
	memcpy(&fingerMatrix, &lpFingerMatrix.toArray3x3(), sizeof(glm::mat3));

	// Get parent matrix:
	Leap::Matrix lpParentMatrix;
	glm::mat3 parentMatrix;
	if (segment > 0)
		lpParentMatrix = fingers[index].bone((Leap::Bone::Type) (segment - 1)).basis();
	else
		lpParentMatrix = hand.basis();
	memcpy(&parentMatrix, &lpParentMatrix.toArray3x3(), sizeof(glm::mat3));
	parentMatrix = glm::inverse(parentMatrix);

	// Return matrix:
	return parentMatrix * fingerMatrix;
}

bool LeapMotionListener::thumbTouchIndex(Leap::FingerList fingers)
{
	auto thumb = fingers[Leap::Finger::TYPE_THUMB].bone(Leap::Bone::TYPE_DISTAL).nextJoint();

	auto index = fingers[Leap::Finger::TYPE_INDEX].bone(Leap::Bone::TYPE_DISTAL).nextJoint();

	auto distance = index - thumb;

	// std::cout << distance << std::endl;

	bool gesture = abs(distance[0]) + abs(distance[1]) + abs(distance[0]) < 22;

	//std::cout << "Gesture: " << gesture << std::endl;
	return gesture;
}