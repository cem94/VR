// Glew (include it before GL.h):
#include <GL/glew.h>

// Engine
#include "DPSEngine.h"
#include <glm/gtx/string_cast.hpp>

// FreeGLUT:
#include <GL/freeglut.h>


bool OvVR::init()
{
	vr::EVRInitError error = vr::VRInitError_None;

	// Init VR system:
	vrSys = vr::VR_Init(&error, vr::VRApplication_Scene);
	if (error != vr::VRInitError_None)
	{
		vrSys = nullptr;
		std::cout << "[ERROR] Unable to init VR runtime: " << vr::VR_GetVRInitErrorAsEnglishDescription(error) << std::endl;
		return false;
	}

	// Init render models:
	vrModels = (vr::IVRRenderModels *) vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &error);
	if (vrModels == nullptr)
	{
		vrSys = nullptr;
		vr::VR_Shutdown();
		std::cout << "[ERROR] Unable to get render model interface: " << vr::VR_GetVRInitErrorAsEnglishDescription(error) << std::endl;
		return false;
	}

	// Initialize the compositor:
	vrComp = vr::VRCompositor();
	if (!vrComp)
	{
		vrModels = nullptr;
		vrSys = nullptr;
		vr::VR_Shutdown();
		std::cout << "[ERROR] Unable to get VR compositor" << std::endl;
		return false;
	}

	// Tweaks:        
	//vrComp->ShowMirrorWindow();        

	// Init controller tracking:
	controllers.clear();
	for (unsigned int c = 0; c < vr::k_unMaxTrackedDeviceCount; c++)
	{
		if (vrSys->GetTrackedDeviceClass(c) == vr::TrackedDeviceClass_Controller)
		{
			std::cout << "   Found controller at " << c << std::endl;
			Controller *cont = new Controller();
			cont->id = c;

			unsigned int bufferLen = vrSys->GetStringTrackedDeviceProperty(c, vr::Prop_RenderModelName_String, nullptr, 0, nullptr);
			if (bufferLen == 0)
			{
				std::cout << "[ERROR] Unable to get controller render model" << std::endl;
				delete cont;
				continue;
			}

			std::string result;
			result.resize(bufferLen);
			vrSys->GetStringTrackedDeviceProperty(c, vr::Prop_RenderModelName_String, &result[0], bufferLen, nullptr);
			std::cout << "   Controller render model: '" << result << "'" << std::endl;
			controllers.push_back(cont);
		}
	}

	// Done:
	return true;
}

/**
* Release VR components.
* @return TF
*/

 bool OvVR::free()
{
	for (unsigned int c = 0; c < controllers.size(); c++)
		delete controllers[c];
	controllers.clear();

	vr::VR_Shutdown();
	vrComp = nullptr;
	vrModels = nullptr;
	vrSys = nullptr;

	// Done:      
	return true;
}

 /**
 * Get tracking system name.
 * @return tracking system name
 */

  std::string OvVR::getTrackingSysName()
 {
	 unsigned int bufferLen = vrSys->GetStringTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String, nullptr, 0, nullptr);
	 if (bufferLen == 0)
		 return std::string();

	 std::string result;
	 result.resize(bufferLen);
	 vrSys->GetStringTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String, &result[0], bufferLen, nullptr);
	 return result;
 }

  /**
  * Print render models to the screen.
  */

   bool OvVR::printRenderModels()
  {
	  for (unsigned int c = 0; c < vrModels->GetRenderModelCount(); c++)
	  {
		  char buffer[256];
		  vrModels->GetRenderModelName(c, buffer, 256);
		  std::cout << "   " << c << ") " << buffer << " model" << std::endl;

		  for (unsigned int d = 0; d < vrModels->GetComponentCount(buffer); d++)
		  {
			  char cbuffer[256];
			  vrModels->GetComponentName(buffer, d, cbuffer, 256);
			  std::cout << "     " << d << ") " << cbuffer << std::endl;
		  }
	  }

	  // Done:
	  return true;
  }

   /**
   * Get manufacturer name.
   * @return manufacturer system name
   */

    std::string OvVR::getManufacturerName()
   {
	   unsigned int bufferLen = vrSys->GetStringTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_ManufacturerName_String, nullptr, 0, nullptr);
	   if (bufferLen == 0)
		   return std::string();

	   std::string result;
	   result.resize(bufferLen);
	   vrSys->GetStringTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_ManufacturerName_String, &result[0], bufferLen, nullptr);
	   return result;
   }

	/**
	* Get model number (name).
	* @return model number (name)
	*/

	 std::string OvVR::getModelNumber()
	{
		unsigned int bufferLen = vrSys->GetStringTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_ModelNumber_String, nullptr, 0, nullptr);
		if (bufferLen == 0)
			return std::string();

		std::string result;
		result.resize(bufferLen);
		vrSys->GetStringTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_ModelNumber_String, &result[0], bufferLen, nullptr);
		return result;
	}

	 /**
	 * Get HMD proper horizontal resolution in pixels.
	 * @return HMD horizontal resolution in pixels
	 */

	  unsigned int OvVR::getHmdIdealHorizRes()
	 {
		 //get the proper resolution of the hmd
		 unsigned int result, dummy;
		 vrSys->GetRecommendedRenderTargetSize(&result, &dummy);
		 return result;
	 }

	  /**
	  * Get HMD proper vertical resolution in pixels.
	  * @return HMD vertical resolution in pixels
	  */

	   unsigned int OvVR::getHmdIdealVertRes()
	  {
		  //get the proper resolution of the hmd
		  unsigned int result, dummy;
		  vrSys->GetRecommendedRenderTargetSize(&dummy, &result);
		  return result;
	  }

	   /**
	   * Converts an OpenVR 4x3 matrix into an OpenGL one.
	   * @param matrix OpenVR 34 matrix
	   * @return OpenGL-friendly matrix
	   */

	    glm::mat4 OvVR::ovr2ogl(const vr::HmdMatrix34_t & matrix)
	   {
		   return glm::mat4(matrix.m[0][0], matrix.m[1][0], matrix.m[2][0], 0.0f,
			   matrix.m[0][1], matrix.m[1][1], matrix.m[2][1], 0.0f,
			   matrix.m[0][2], matrix.m[1][2], matrix.m[2][2], 0.0f,
			   matrix.m[0][3], matrix.m[1][3], matrix.m[2][3], 1.0f);
	   }

		/**
		* Converts an OpenVR 4x4 matrix into an OpenGL one.
		* @param matrix OpenVR 44 matrix
		* @return Overvision-friendly matrix
		*/

		 glm::mat4 OvVR::ovr2ogl(const vr::HmdMatrix44_t & matrix)
		{
			return glm::mat4(matrix.m[0][0], matrix.m[1][0], matrix.m[2][0], matrix.m[3][0],
				matrix.m[0][1], matrix.m[1][1], matrix.m[2][1], matrix.m[3][1],
				matrix.m[0][2], matrix.m[1][2], matrix.m[2][2], matrix.m[3][2],
				matrix.m[0][3], matrix.m[1][3], matrix.m[2][3], matrix.m[3][3]);
		}

		 /**
		 * Update poses and internal params. Invoke that once per frame.
		 * @return TF
		 */

		  bool OvVR::update()
		 {
			 // Main update method:
			 vrComp->WaitGetPoses(vrPoses, vr::k_unMaxTrackedDeviceCount, nullptr, 0);

			 // If used, update controllers:
			 for (auto c : controllers)
			 {
				 if (vrPoses[c->id].bPoseIsValid)
					 c->matrix = ovr2ogl(vrPoses[c->id].mDeviceToAbsoluteTracking);
			 }

			 // Done:
			 return true;
		 }

		  /**
		  * Get the projection matrix for the given eye and plane params.
		  * @param eye left or right eye (use enum)
		  * @param nearPlane user camera near plane distance
		  * @param farPlane user camera far plane distance
		  * @return projection matrix ready for OpenGL
		  */

		   glm::mat4 OvVR::getProjMatrix(OvEye eye, float nearPlane, float farPlane)
		  {
			  switch (eye)
			  {
			  case EYE_LEFT: return ovr2ogl(vrSys->GetProjectionMatrix(vr::Eye_Left, nearPlane, farPlane)); break;
			  case EYE_RIGHT: return ovr2ogl(vrSys->GetProjectionMatrix(vr::Eye_Right, nearPlane, farPlane)); break;
			  default: return glm::mat4(1.0f);
			  }
		  }

		   /**
		   * Get the eye-to-head modelview matrix for the given eye.
		   * @param eye left or right eye (use enum)
		   * @return eye-to-head modelview matrix ready for OpenGL
		   */

		    glm::mat4 OvVR::getEye2HeadMatrix(OvEye eye)
		   {
			   switch (eye)
			   {
			   case EYE_LEFT: return ovr2ogl(vrSys->GetEyeToHeadTransform(vr::Eye_Left)); break;
			   case EYE_RIGHT: return ovr2ogl(vrSys->GetEyeToHeadTransform(vr::Eye_Right)); break;
			   default: return glm::mat4(1.0f);
			   }
		   }

			/**
			* Get the user's head modelview position.
			* @return modelview matrix ready for OpenGL
			*/

			 glm::mat4 OvVR::getModelviewMatrix()
			{
				if (vrPoses[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid == false)
					return glm::mat4(1.0f);

				glm::mat4 headPos = ovr2ogl(vrPoses[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking);
				return headPos;
			}

			 /**
			 * Get the number of identified and initialized controllers.
			 * @return number of controllers
			 */

			  unsigned int OvVR::getNrOfControllers()
			 {
				 return (unsigned int)controllers.size();
			 }

			  /**
			  * Enable/disable reprojection
			  * @param flag true or false
			  */

			   void OvVR::setReprojection(bool flag)
			  {
				  vrComp->ForceInterleavedReprojectionOn(flag);
			  }

			   /**
			   * Pass the left and right textures to the HMD.
			   * @param eye left or right eye (use enum)
			   * @param eyeTexture OpenGL texture handle
			   */

			    void OvVR::pass(OvEye eye, unsigned int eyeTexture)
			   {
				   const vr::Texture_t t = { reinterpret_cast<void *>(uintptr_t(eyeTexture)), vr::TextureType_OpenGL, vr::ColorSpace_Linear };
				   switch (eye)
				   {
				   case EYE_LEFT:  vrComp->Submit(vr::Eye_Left, &t); break;
				   case EYE_RIGHT: vrComp->Submit(vr::Eye_Right, &t); break;
				   }
			   }

				/**
				* Once passed the left and right textures, invoke this method to terminate rendering.
				*/

				 void OvVR::render()
				{
					vrComp->PostPresentHandoff();
				}
