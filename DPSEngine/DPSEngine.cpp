//////////////
// #INCLUDE //
//////////////

// Glew (include it before GL.h):
#include <GL/glew.h>

// Engine
#include "DPSEngine.h"

// FreeGLUT:
#include <GL/freeglut.h>


/////////////
// #DEFINE //
/////////////

// Window size:
#define APP_WINDOWSIZEX   1024
#define APP_WINDOWSIZEY   512
#define APP_FBOSIZEX      APP_WINDOWSIZEX / 2
#define APP_FBOSIZEY      APP_WINDOWSIZEY


// Enums:
enum Eye
{
	EYE_LEFT = 0,
	EYE_RIGHT = 1,

	// Terminator:
	EYE_LAST,
};


////////////
// GLOBAL //
////////////

unsigned int globalVao = 0;
bool DPSEngine::initFlag = false;
std::vector<DPSMaterial*> DPSEngine::materials;
std::vector<DPSTexture*> DPSEngine::textures;
int DPSEngine::windowID = 0;
DPSList DPSEngine::renderList;
DPSList transparencyList;
DPSCamera * DPSEngine::activeCamera;

bool useCubemap = false;
DPSCubeMap * cubeMap;

// Matrices:
glm::mat4 perspective;
glm::mat4 fboPerspective;
glm::mat4 ortho;

// Vertex buffers:
unsigned int boxVertexVbo = 0;
unsigned int boxTexCoordVbo = 0;

// Textures:
unsigned int texId = 0;
unsigned int eyeTexId[OvVR::EYE_LAST] = { 0, 0 };

// FBO:   
Fbo *eyeFbo[OvVR::EYE_LAST] = { nullptr, nullptr };

// PPL shader:
Shader *pplVs = nullptr;
Shader *pplFs = nullptr;
Shader *pplShader = nullptr;
int projLoc = -1;
int mvLoc = -1;
int normalMatLoc = -1;
int matAmbientLoc = -1;
int calcLightLoc = -1;
int matDiffuseLoc = -1;
int matSpecularLoc = -1;
int matShininessLoc = -1;
int matTransparencyLoc = -1;
int lightPositionLoc = -1;
int lightAmbientLoc = -1;
int lightDiffuseLoc = -1;
int lightSpecularLoc = -1;

// Passthrough shader:
Shader *passthroughVs = nullptr;
Shader *passthroughFs = nullptr;
Shader *passthroughShader = nullptr;
int ptProjLoc = -1;
int ptMvLoc = -1;
int ptColorLoc = -1;

// OpenVR interface:
OvVR *ovr = nullptr;

/////////////
// SHADERS //
/////////////

// Blinn-Phong PPL with texture mapping:
const char *pplVertShader = R"(
   #version 440 core

   // Uniforms:
   uniform mat4 projection;
   uniform mat4 modelview;
   uniform mat3 normalMatrix;

   // Attributes:
   layout(location = 0) in vec3 in_Position;
   layout(location = 1) in vec3 in_Normal;
   layout(location = 2) in vec2 in_TexCoord;

   // Varying:
   out vec4 fragPosition;
   out vec3 normal;   
   out vec2 texCoord;

   void main(void)
   {
      fragPosition = modelview * vec4(in_Position, 1.0f);
      gl_Position = projection * fragPosition;      
      normal = normalMatrix * in_Normal;
      texCoord = in_TexCoord;
   }
)";

const char *pplFragShader = R"(
   #version 440 core

   in vec4 fragPosition;
   in vec3 normal;   
   in vec2 texCoord;
   
   out vec4 fragOutput;

	//CalcLight
	uniform float calcLight;


   // Material properties:
   uniform vec3 matAmbient;
   uniform vec3 matDiffuse;
   uniform vec3 matSpecular;
   uniform float matShininess;
   uniform float matTransparency;

   // Light properties:
   uniform vec3 lightPosition; 
   uniform vec3 lightAmbient; 
   uniform vec3 lightDiffuse; 
   uniform vec3 lightSpecular;

   // Texture mapping:
   //layout(binding = 0) uniform sampler2D texSampler;
	uniform sampler2D tex;
   void main(void)   
   {  

      // Texture element:
      vec4 texel = texture(tex, texCoord);
      

	if(calcLight > 0.0f)
	{
		
      // Ambient term:
      vec3 fragColor = matAmbient * lightAmbient;

      // Diffuse term:
      vec3 _normal = normalize(normal);
      vec3 lightDirection = normalize(lightPosition - fragPosition.xyz);      
      float nDotL = dot(lightDirection, _normal);   
      if (nDotL > 0.0f)
      {
         fragColor += matDiffuse * nDotL * lightDiffuse;
      
         // Specular term:
         vec3 halfVector = normalize(lightDirection + normalize(-fragPosition.xyz));                     
         float nDotHV = dot(_normal, halfVector);         
         fragColor += matSpecular * pow(nDotHV, matShininess) * lightSpecular;
      }       
	  fragOutput = texel * vec4(fragColor, matTransparency);      
	}
	else{
		 fragOutput =texel * vec4(1.0f , 1.0f , 1.0f, matTransparency);
	  }

	} 
	
)";

// Passthrough shader with texture mapping:
const char *passthroughVertShader = R"(
   #version 440 core

   // Uniforms:
   uniform mat4 projection;
   uniform mat4 modelview;   

   // Attributes:
   layout(location = 0) in vec2 in_Position;   
   layout(location = 2) in vec2 in_TexCoord;

   // Varying:   
   out vec2 texCoord;

   void main(void)
   {      
      gl_Position = projection * modelview * vec4(in_Position, 0.0f, 1.0f);    
      texCoord = in_TexCoord;
   }
)";

const char *passthroughFragShader = R"(
   #version 440 core
   
   in vec2 texCoord;
   
   uniform vec4 color;

   out vec4 fragOutput;   

   // Texture mapping:
   layout(binding = 0) uniform sampler2D tex;
	//uniform sampler2D tex;

   void main(void)   
   {  
      // Texture element:
      vec4 texel = texture(tex, texCoord);      
      
      // Final color:
      fragOutput = color * texel;       
   }
)";

//////////////
// DLL MAIN //
//////////////

#ifdef _WINDOWS
#include <Windows.h>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * DLL entry point. Avoid to rely on it for easier code portability (Linux doesn't use this method).
 * @param instDLL handle
 * @param reason reason
 * @param _reserved reserved
 * @return true on success, false on failure
 */
int APIENTRY DllMain(HANDLE instDLL, DWORD reason, LPVOID _reserved) {

	// Check use:
	switch (reason)
	{
		///////////////////////////
	case DLL_PROCESS_ATTACH: //
		break;


		///////////////////////////
	case DLL_PROCESS_DETACH: //
		break;
	}

	// Done:
	return true;
}
#endif

///////////////
// FUNCTIONS //
///////////////

/**
*	Display Callback - void glutDisplayFunc(void (*func)(void))
*
*	@author		DPS
*	@param		void
*	@return		void
*
*	glutDisplayFunc sets the display callback for the current window. When GLUT determines that the normal plane for the window needs to
*	be redisplayed, the display callback for the window is called. Before the callback, the current window is set to the window needing
*	to be redisplayed and (if no overlay display callback is registered) the layer in use is set to the normal plane. The display callback
*	is called with no parameters. The entire normal plane region should be redisplayed in response to the callback (this includes ancillary
*	buffers if your program depends on their state).
*	GLUT determines when the display callback should be triggered based on the window's redisplay state. The redisplay state for a window
*	can be either set explicitly by calling glutPostRedisplay or implicitly as the result of window damage reported by the window system.
*	Multiple posted redisplays for a window are coalesced by GLUT to minimize the number of display callbacks called.
*	When an overlay is established for a window, but there is no overlay display callback registered, the display callback is used for
*	redisplaying both the overlay and normal plane (that is, it will be called if either the redisplay state or overlay redisplay state is set).
*	In this case, the layer in use is not implicitly changed on entry to the display callback.
*	See glutOverlayDisplayFunc to understand how distinct callbacks for the overlay and normal plane of a window may be established.
*	When a window is created, no display callback exists for the window. It is the responsibility of the programmer to install a display
*	callback for the window before the window is shown. A display callback must be registered for any window that is shown. If a window
*	becomes displayed without a display callback being registered, a fatal error occurs. Passing NULL to glutDisplayFunc is illegal as of
*	GLUT 3.0; there is no way to ``deregister'' a display callback (though another callback routine can always be registered).
*	Upon return from the display callback, the normal damaged state of the window (returned by calling glutLayerGet(GLUT_NORMAL_DAMAGED) is
*	cleared. If there is no overlay display callback registered the overlay damaged state of the window (returned by calling glutLayerGet(GLUT_OVERLAY_DAMAGED)
*	is also cleared.
*
*/
void LIB_API DPSEngine::displayFunc(void(*func)())
{
	glutDisplayFunc(func);
}

/**
*	Reshape Callback - void glutReshapeFunc(void (*func)(int width, int height));
*
*	@author		DPS
*	@param		int width, int height
*	@return		void
*
*	glutReshapeFunc sets the reshape callback for the current window. The reshape callback is triggered when a window is reshaped.
*	A reshape callback is also triggered immediately before a window's first display callback after a window is created or whenever
*	an overlay for the window is established. The width and height parameters of the callback specify the new window size in pixels.
*	Before the callback, the current window is set to the window that has been reshaped.
*	If a reshape callback is not registered for a window or NULL is passed to glutReshapeFunc (to deregister a previously registered
*	callback), the default reshape callback is used. This default callback will simply call glViewport(0,0,width,height) on the normal
*	plane (and on the overlay if one exists).
*	If an overlay is established for the window, a single reshape callback is generated. It is the callback's responsibility to update
*	both the normal plane and overlay for the window (changing the layer in use as necessary).
*	When a top-level window is reshaped, subwindows are not reshaped. It is up to the GLUT program to manage the size and positions of
*	subwindows within a top-level window. Still, reshape callbacks will be triggered for subwindows when their size is changed using glutReshapeWindow.
*
*/
void LIB_API DPSEngine::reshapeFunc(void(*func)(int, int))
{
	glutReshapeFunc(func);
}

/**
*	Timer Callback - void glutTimerFunc(void (*func)(int time));
*
*	@author		DPS
*	@param		int time
*	@return		void
*
*   glutTimerFunc registers the timer callback func to be triggered in at least msecs milliseconds. The value parameter to the timer callback will be
*   the value of the value parameter to glutTimerFunc. Multiple timer callbacks at same or differing times may be registered simultaneously.
*   The number of milliseconds is a lower bound on the time before the callback is generated. GLUT attempts to deliver the timer callback as soon as
*   possible after the expiration of the callback's time interval.
*   There is no support for canceling a registered callback. Instead, ignore a callback based on its value parameter when it is triggered.
*/
void LIB_API DPSEngine::timerFunc(void(*func)(int), int time)
{
	glutTimerFunc(time, func, 0);
}

/**
*	Keyboard Callback - void glutKeyboardFunc(void (*func)(unsigned char, int, int));
*
*	@author		DPS
*	@param		unsigned char key, int x, int y
*	@return		void
*
*   glutKeyboardFunc sets the keyboard callback for the current window. When a user types into the window, each key press generating an ASCII character
*   will generate a keyboard callback. The key callback parameter is the generated ASCII character. The state of modifier keys such as Shift cannot be
*   determined directly; their only effect will be on the returned ASCII data. The x and y callback parameters indicate the mouse location in window relative
*   coordinates when the key was pressed. When a new window is created, no keyboard callback is initially registered, and ASCII key strokes in the window are
*   ignored. Passing NULL to glutKeyboardFunc disables the generation of keyboard callbacks.
*   During a keyboard callback, glutGetModifiers may be called to determine the state of modifier keys when the keystroke generating the callback occurred.
*   Also, see glutSpecialFunc for a means to detect non-ASCII key strokes.
*
*/
void LIB_API DPSEngine::keyboardFunc(void(*func)(unsigned char, int, int))
{
	glutKeyboardFunc(func);
}

/**
*	Mouse Callback - void glutMotionFunc(void (*func)(int x, int y));
*
*	@author		DPS
*	@param		int width, int height
*	@return		void
*
*   glutMotionFunc set the motion and passive motion callback respectively for the current window. The motion callback for a window is
*   called when the mouse moves within the window while one or more mouse buttons are pressed. The passive motion callback for a window is called when the mouse moves
*   within the window while no mouse buttons are pressed.
*   The x and y callback parameters indicate the mouse location in window relative coordinates.
*   Passing NULL to glutMotionFunc or glutPassiveMotionFunc disables the generation of the mouse or passive motion callback respectively.
*
*/
void LIB_API DPSEngine::mouseMotionFunc(void(*func)(int, int))
{
	glutMotionFunc(func);
}


/**
 * Window close callback. Required to shutdown OpenVR before the context is released.
 */
void closeCallback()
{
	// Free OpenVR:   
	ovr->free();
	delete ovr;

	// Free OpenGL stuff:
	glDeleteBuffers(1, &boxVertexVbo);
	glDeleteBuffers(1, &boxTexCoordVbo);
	glDeleteVertexArrays(1, &globalVao);
	for (int c = 0; c < 2; c++)
	{
		delete eyeFbo[c];
		glDeleteTextures(1, &eyeTexId[c]);
	}
	delete passthroughShader;
	delete passthroughFs;
	delete passthroughVs;
}


void DPSEngine::setProjection(glm::mat4 projectionMatrix)
{
	//Load Projection 
	pplShader->setMatrix(projLoc, projectionMatrix);
}

void DPSEngine::setModelView(glm::mat4 modelMatrix)
{
	// Set model matrix as current OpenGL matrix:
	pplShader->setMatrix(mvLoc, modelMatrix);
	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(modelMatrix));
	pplShader->setMatrix3(normalMatLoc, normalMatrix);
}

void DPSEngine::buildCubeMap(std::string * filesName)
{
	cubeMap = new DPSCubeMap(filesName);
	cubeMap->buildCubemap();
	useCubemap = true;
}

void DPSEngine::loadLight(DPSLight * light , glm::mat4 inverseCameraMatrix)
{
	pplShader->setVec3(lightAmbientLoc, light->getAmbient());
	pplShader->setVec3(lightDiffuseLoc, light->getDiffuse());
	pplShader->setVec3(lightSpecularLoc, light->getSpecular());

	glm::vec3 pos = (inverseCameraMatrix * light->getMatrix())[3];
	pplShader->setVec3(lightPositionLoc, glm::vec3(pos.x, pos.y, pos.z));
}

void DPSEngine::enableLight(bool state)
{
	if(state == true){
		pplShader->setFloat(calcLightLoc, 1.0f);
	}
	else {
		pplShader->setFloat(calcLightLoc, -1.0f);
	}
}

void DPSEngine::loadMaterial(DPSMaterial * material)
{
	pplShader->setVec3(matAmbientLoc, material->getAmbient());
	pplShader->setVec3(matDiffuseLoc, material->getDiffuse());
	pplShader->setVec3(matSpecularLoc, material->getSpecular());
	pplShader->setFloat(matShininessLoc, material->getShininess());
	pplShader->setFloat(matTransparencyLoc, material->getTrasparency());
	//pplShader->setFloat(matTransparencyLoc, material->getTrasparency());
}

void DPSEngine::loadTexture(DPSTexture * texture)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->textureId);
	glUniform1i(pplShader->getParamLocation("tex"), 0);
}


/**
*	Special Callback - void glutSpecialFunc(void (*func)(int, int, int));
*
*	@author		DPS
*	@param		int key, int y, int y
*	@return		void
*
*   glutSpecialFunc sets the special keyboard callback for the current window. The special keyboard callback is triggered when keyboard function or directional keys are pressed.
*   The key callback parameter is a GLUT_KEY_* constant for the special key pressed. The x and y callback parameters indicate the mouse in window relative coordinates when the key
*   was pressed. When a new window is created, no special callback is initially registered and special key strokes in the window are ignored. Passing NULL to glutSpecialFunc disables
*   the generation of special callbacks.
*/
void LIB_API DPSEngine::specialFunc(void(*func)(int, int, int))
{
	glutSpecialFunc(func);
}

#ifdef _DEBUG
//DEBUG CALLBACK
void __stdcall DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam) {
	printf("OpenGL says: %s\n", message);
}
#endif




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Initialize the shaders used by this program.
 * @return TF
 */
bool buildShaders()
{
	////////////////////
	// Build PPL shader:
	pplVs = new Shader();
	pplVs->loadFromMemory(Shader::TYPE_VERTEX, pplVertShader);

	pplFs = new Shader();
	pplFs->loadFromMemory(Shader::TYPE_FRAGMENT, pplFragShader);

	pplShader = new Shader();
	pplShader->build(pplVs, pplFs);
	pplShader->render();

	// Bind params:
	pplShader->bind(0, "in_Position");
	pplShader->bind(1, "in_Normal");
	pplShader->bind(2, "in_TexCoord");

	projLoc = pplShader->getParamLocation("projection");
	mvLoc = pplShader->getParamLocation("modelview");
	normalMatLoc = pplShader->getParamLocation("normalMatrix");

	matAmbientLoc = pplShader->getParamLocation("matAmbient");
	matDiffuseLoc = pplShader->getParamLocation("matDiffuse");
	matSpecularLoc = pplShader->getParamLocation("matSpecular");
	matShininessLoc = pplShader->getParamLocation("matShininess");
	matTransparencyLoc = pplShader->getParamLocation("matTransparency");

	calcLightLoc = pplShader->getParamLocation("calcLight");

	lightPositionLoc = pplShader->getParamLocation("lightPosition");
	lightAmbientLoc = pplShader->getParamLocation("lightAmbient");
	lightDiffuseLoc = pplShader->getParamLocation("lightDiffuse");
	lightSpecularLoc = pplShader->getParamLocation("lightSpecular");

	////////////////////////////
	// Build passthrough shader:
	passthroughVs = new Shader();
	passthroughVs->loadFromMemory(Shader::TYPE_VERTEX, passthroughVertShader);

	passthroughFs = new Shader();
	passthroughFs->loadFromMemory(Shader::TYPE_FRAGMENT, passthroughFragShader);

	passthroughShader = new Shader();
	passthroughShader->build(passthroughVs, passthroughFs);
	passthroughShader->render();

	// Bind params:
	passthroughShader->bind(0, "in_Position");
	passthroughShader->bind(2, "in_TexCoord");

	ptProjLoc = passthroughShader->getParamLocation("projection");
	ptMvLoc = passthroughShader->getParamLocation("modelview");
	ptColorLoc = passthroughShader->getParamLocation("color");

	// Done:
	return true;
}

/**
*	Initialize the freeglut Contest
*
*	@author		DPS
*	@param		int argc, char * argv[]
*	@return		bool
*
*/
bool LIB_API DPSEngine::init(int argc, char * argv[])
{
	// Init context:
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(4, 4); //FOR opnegl 4.4
	glutInitContextFlags(GLUT_CORE_PROFILE | GLUT_DEPTH);//FOR opnegl 4.4

	glutInitWindowPosition(600, 600);
	glutInitWindowSize(1240, 720);

	// FreeGLUT can parse command-line params, in case:
	glutInit(&argc, argv);

	// Set some optional flags:
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	// Create the window with a specific title:
	windowID = glutCreateWindow("VR Engine");

	// Init Glew (*after* the context creation):
	glewExperimental = GL_TRUE;
	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		std::cout << "Error: " << glewGetErrorString(error) << std::endl;
		return -1;
	}
	else {
		if (GLEW_VERSION_4_4)
			std::cout << "Driver supports OpenGL 4.4\n" << std::endl;
		else
		{
			std::cout << "Error: OpenGL 4.4 not supported\n" << std::endl;
			return -1;
		}

		// Init OpenVR:   
		ovr = new OvVR();
		if (ovr->init() == false)
		{
			std::cout << "[ERROR] Unable to init OpenVR" << std::endl;
			delete ovr;
			return -2;
		}

		// Report some info:
		std::cout << "   Manufacturer . . :  " << ovr->getManufacturerName() << std::endl;
		std::cout << "   Tracking system  :  " << ovr->getTrackingSysName() << std::endl;
		std::cout << "   Model number . . :  " << ovr->getModelNumber() << std::endl;

		// Set Close callback functions:
		glutCloseFunc(closeCallback);

		// Init VAO:   
		glGenVertexArrays(1, &globalVao);
		glBindVertexArray(globalVao);

		// Tell OpenGL that you want to use vertex arrays for the given attributes:
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		// Enable vertex position and colour + Texture attribute arrays
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		// Compile shaders:
		buildShaders();
		pplShader->render();

		// Set initial material and light params:
		pplShader->setVec3(matAmbientLoc, glm::vec3(0.2f, 0.2f, 0.2f));
		pplShader->setVec3(matDiffuseLoc, glm::vec3(0.7f, 0.7f, 0.7f));
		pplShader->setVec3(matSpecularLoc, glm::vec3(0.6f, 0.6f, 0.6f));
		pplShader->setFloat(matShininessLoc, 128.0f);

		pplShader->setVec3(lightAmbientLoc, glm::vec3(1.0f, 1.0f, 1.0f));
		pplShader->setVec3(lightDiffuseLoc, glm::vec3(1.0f, 1.0f, 1.0f));
		pplShader->setVec3(lightSpecularLoc, glm::vec3(1.0f, 1.0f, 1.0f));

		// Create a 2D box for screen rendering:
		glm::vec2 *boxPlane = new glm::vec2[4];
		boxPlane[0] = glm::vec2(0.0f, 0.0f);
		boxPlane[1] = glm::vec2(APP_FBOSIZEX, 0.0f);
		boxPlane[2] = glm::vec2(0.0f, APP_FBOSIZEY);
		boxPlane[3] = glm::vec2(APP_FBOSIZEX, APP_FBOSIZEY);

		// Copy data into VBOs:
		glGenBuffers(1, &boxVertexVbo);
		glBindBuffer(GL_ARRAY_BUFFER, boxVertexVbo);
		glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), boxPlane, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		delete[] boxPlane;


		// ...same for tex coords:
		glm::vec2 *texCoord = new glm::vec2[4];
		texCoord[0] = glm::vec2(0.0f, 0.0f);
		texCoord[1] = glm::vec2(1.0f, 0.0f);
		texCoord[2] = glm::vec2(0.0f, 1.0f);
		texCoord[3] = glm::vec2(1.0f, 1.0f);
		glGenBuffers(1, &boxTexCoordVbo);
		glBindBuffer(GL_ARRAY_BUFFER, boxTexCoordVbo);
		glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), texCoord, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(2);
		delete[] texCoord;

		// Load FBO and its texture:
		GLint prevViewport[4];
		glGetIntegerv(GL_VIEWPORT, prevViewport);

		int fboSizeX = ovr->getHmdIdealHorizRes();
		int fboSizeY = ovr->getHmdIdealVertRes();
		std::cout << "   Ideal resolution :  " << fboSizeX << "x" << fboSizeY << std::endl;

		for (int c = 0; c < OvVR::EYE_LAST; c++)
		{
			// Fill texture with static color:
			unsigned char *data = new unsigned char[fboSizeX * fboSizeY * 4];
			for (int d = 0; d < fboSizeX * fboSizeY; d++)
			{
				if (c == 0)
				{
					data[d * 4] = 255;
					data[d * 4 + 1] = 0;
					data[d * 4 + 2] = 0;
				}
				else
				{
					data[d * 4] = 0;
					data[d * 4 + 1] = 255;
					data[d * 4 + 2] = 255;
				}
				data[d * 4 + 3] = 255;
			}

			// Init texture:
			glGenTextures(1, &eyeTexId[c]);
			glBindTexture(GL_TEXTURE_2D, eyeTexId[c]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, fboSizeX, fboSizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); // GL_RGBA8 IS IMPORTANT!!
			delete[] data;

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// Init FBO:
			eyeFbo[c] = new Fbo();
			eyeFbo[c]->bindTexture(0, Fbo::BIND_COLORTEXTURE, eyeTexId[c]);
			eyeFbo[c]->bindRenderBuffer(1, Fbo::BIND_DEPTHBUFFER, fboSizeX, fboSizeY);
			if (!eyeFbo[c]->isOk())
				std::cout << "[ERROR] Invalid FBO" << std::endl;
		}


		Fbo::disable();
		glViewport(0, 0, prevViewport[2], prevViewport[3]);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);


		//default camera
		DPSEngine::activeCamera = new DPSCamera();
		DPSEngine::activeCamera->setName("MainCamera");
		DPSEngine::activeCamera->setMatrix(glm::mat4(1.0f));




		//ZBuffer
		glEnable(GL_DEPTH_TEST);
		glClearDepth(1.0f);
		glDepthFunc(GL_LESS);

		//glEnable(GL_LIGHTING);
		//glShadeModel(GL_SMOOTH); //Activate Gouraud shading, better than GL_FLAT
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


		//glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0f);//More accurate computation of specular highlights
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		glEnable(GL_NORMALIZE);
		glEnable(GL_CULL_FACE);
		DPSTexture::initializeFreeImage();

		//Transparency --> enable and set blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

#ifdef _DEBUG
// Register OpenGL debug callback:  --> disable on release
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		// Register OpenGL debug callback:   --> disable on release
		glDebugMessageCallback((GLDEBUGPROC)DebugCallback, nullptr);

#endif
		return true;
	}
}


void LIB_API DPSEngine::setActiveCamera(DPSCamera * camera)
{
	activeCamera = camera;
}



LIB_API DPSCamera* DPSEngine::getActiveCamera()
{
	return activeCamera;
}

/**
*	Set viewport
*
*	@author		DPS
*	@param		int width, int height
*	@return		void
*
*/


void LIB_API DPSEngine::reshape(int width, int height) {
	// ... ignore the params, we want a fixed-size window

	// Update matrices:
	perspective = glm::perspective(glm::radians(45.0f), (float)APP_WINDOWSIZEX / (float)APP_WINDOWSIZEY, 1.0f, 1024.0f);
	fboPerspective = glm::perspective(glm::radians(45.0f), (float)APP_FBOSIZEX / (float)APP_FBOSIZEY, 1.0f, 1024.0f);
	ortho = glm::ortho(0.0f, (float)APP_WINDOWSIZEX, 0.0f, (float)APP_WINDOWSIZEY, -1.0f, 1.0f);
	// (bad) trick to avoid window resizing:
	if (width != APP_WINDOWSIZEX || height != APP_WINDOWSIZEY)
		glutReshapeWindow(APP_WINDOWSIZEX, APP_WINDOWSIZEY);
}


void LIB_API  DPSEngine::setViewport(int width, int height)
{
	glViewport(0, 0, width, height);
}

/**
*	Set textures path
*
*	@author		DPS
*	@param		string path
*	@return		void
*
*/
void LIB_API DPSEngine::setTexturesPath(std::string path)
{
	DPSTexture::setPath(path);
}

/**
*	Swap buffers
*
*	@author		DPS
*	@param      void
*	@return		void
*
*   Performs a buffer swap on the layer in use for the current window. Specifically, glutSwapBuffers promotes the contents of the back buffer of the layer in use of the
*   current window to become the contents of the front buffer. The contents of the back buffer then become undefined. The update typically takes place during the vertical
*   retrace of the monitor, rather than immediately after glutSwapBuffers is called.
*   An implicit   glFlush is done by glutSwapBuffers before it returns. Subsequent OpenGL commands can be issued immediately after calling glutSwapBuffers, but are not executed
*   until the buffer exchange is completed.
*   If the layer in use is not double buffered, glutSwapBuffers has no effect.
*
*/
void LIB_API DPSEngine::swapBuffers()
{
	glutSwapBuffers();
}

/**
*	Post window redisplay
*
*	@author		DPS
*	@param      int windowID
*	@return		void
*
*   Similar to glutPostRedisplay(), except that instead of affecting the current window , this function affects an arbitrary window, indicated by the windowID parameter.
*/
void LIB_API DPSEngine::postWindowRedisplay()
{
	glutPostWindowRedisplay(windowID);
}

/**
*	Enable light
*
*	@author		DPS
*	@param      void
*	@return		void
*
*/
void LIB_API DPSEngine::enableLight()
{
	glEnable(GL_LIGHTING);
}

/**
*	Disable light
*
*	@author		DPS
*	@param      void
*	@return		void
*
*/
void LIB_API DPSEngine::disableLight()
{
	glDisable(GL_LIGHTING);
}

/**
*	Clear color
*
*	@author		DPS
*	@param      float r, float g, float b, float a
*	@return		void
*
*/
void LIB_API DPSEngine::clearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/**
*	Clear color
*
*	@author		DPS
*	@param      void
*	@return		void
*
*/
void LIB_API DPSEngine::clearScreen()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/**
*	Print value on screen
*
*	@author		DPS
*	@param      string label, float v, glm::mat4 ortho
*	@return		void
*
*/
///TODO: posizione testo dinamica (variabile globale)
void LIB_API DPSEngine::writeValue(std::string label, float v, glm::mat4 ortho)

{
	// Set orthographic projection:


   // Disable lighting before rendering 2D text:
	glDisable(GL_LIGHTING);
	char text[64];

	strcpy(text, label.c_str());
	sprintf(text, "FPS: %.1f", v);
	glRasterPos2f(1.0f, 36.0f);
	glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char *)text);

	glEnable(GL_LIGHTING);

}


/**
*	Start of the application loop
*
*	@author		DPS
*	@param		void
*	@return		void
*
*   glutMainLoop enters the GLUT event processing loop. This routine should be called at most once in a GLUT program. Once called, this routine will never return.
*   It will call as necessary any callbacks that have been registered.
*/
void LIB_API DPSEngine::start()
{
	glutMainLoop();
}

/**
*	Render the pre-loaded scene
*
*	@author		DPS
*	@param		void
*	@return		void
*
*/
void LIB_API DPSEngine::renderScene()
{
	// Store the current viewport size:
	GLint prevViewport[4];
	glGetIntegerv(GL_VIEWPORT, prevViewport);

	// Update user position:
	ovr->update();
	glm::mat4 headPos = ovr->getModelviewMatrix();

	DPSEngine::activeCamera->setMatrix(headPos);

	//Get Lights --> if present they are always in front of the render list
	std::vector<DPSNode*> lights;

	//TODO:: ADD
	//for (int i = 0; i < renderList.getNumberOfLights(); i++) {
		//lights.push_back(renderList.get(i));
	//}




	int i = 0;
	for (i; i < renderList.getSize(); i++) {
		if (renderList.get(i)->getType() == DPSTYPE_LIGHT) {
			lights.push_back(renderList.get(i));
		}else {
			break;
		}
	}

	// Render to each eye:   
	for (int c = 0; c < EYE_LAST; c++)
	{
		OvVR::OvEye curEye = (OvVR::OvEye) c;
		glm::mat4 projMat = ovr->getProjMatrix(curEye, 1.0f, 1024.0f);
		glm::mat4 eye2Head = ovr->getEye2HeadMatrix(curEye);

		// Set your camera projection matrix to this one:
		glm::mat4 myCameraProjMat = projMat * glm::inverse(eye2Head);

#ifdef APP_VERBOSE   
		std::cout << "Eye " << c << " proj matrix: " << glm::to_string(myCameraProjMat) << std::endl;
#endif

		// Set your camera modelview matrix to this one:
		glm::mat4 myCameraModelViewMat = headPos;

#ifdef APP_VERBOSE   
		std::cout << "Eye " << c << " modelview matrix: " << glm::to_string(myCameraModelViewMat) << std::endl;
#endif

		// Put your 3D rendering to FBO here...      
		eyeFbo[c]->render();

		// Clear the FBO content:
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




		if (useCubemap) {

			cubeMap->enableShader();
			cubeMap->setProjection(myCameraProjMat);
			cubeMap->setModelView(glm::scale(glm::inverse(myCameraModelViewMat), glm::vec3(20.0f, 20.0f, 20.0f)));

			cubeMap->render();
		}


		// Setup params for the PPL shader:
		pplShader->render();
		//DPSEngine::setProjection(activeCamera->getProjection());
		//DPSEngine::setModelView(activeCamera->getMatrix());

		DPSEngine::setProjection(myCameraProjMat);
		DPSEngine::setModelView(myCameraModelViewMat);

		//Multi Pass
		for (int k = 0; k < lights.size(); k++)
		{
			//lights.at(k)->setMatrix(lights.at(k)->getMatrix() * glm::inverse(myCameraModelViewMat));
			DPSEngine::loadLight(static_cast<DPSLight*>(lights.at(k)) , glm::inverse(myCameraModelViewMat));

			//Render scene
			//for (int j = renderList.getNumberOfLights(); j < renderList.getSize(); j++) {
			//TODO:: ADDDD
			for (int j = i; j < renderList.getSize(); j++) {
				//renderList.get(j)->render(activeCamera->getMatrix());
				renderList.get(j)->render(glm::inverse(myCameraModelViewMat));
			}
		}

		// Send rendered image to the proper OpenVR eye:      
		ovr->pass(curEye, eyeTexId[c]);
	}

	// Update internal OpenVR settings:
	ovr->render();

	// Done with the FBO, go back to rendering into the default context buffers:
	Fbo::disable();
	glViewport(0, 0, prevViewport[2], prevViewport[3]);

	////////////////
	// 2D rendering:

	// Set a matrix for the left "eye":    
	glm::mat4 f = glm::mat4(1.0f);

	// Setup the passthrough shader:
	passthroughShader->render();
	passthroughShader->setMatrix(ptProjLoc, ortho);
	passthroughShader->setMatrix(ptMvLoc, f);
	//passthroughShader->setVec4(ptColorLoc, glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));

	// Bind the FBO buffer as texture and render:
	glBindTexture(GL_TEXTURE_2D, eyeTexId[0]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// Set matrix for the other eye and render another quad:
	f = glm::translate(glm::mat4(1.0f), glm::vec3(APP_WINDOWSIZEX / 2.0f, 0.0f, 0.0f));
	passthroughShader->setMatrix(ptMvLoc, f);
	glBindTexture(GL_TEXTURE_2D, eyeTexId[1]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

/**
*	Load the .OVO file
*
*	@author		DPS
*	@param		const char * src
*	@return		DPSNode *
*
*/
DPSNode LIB_API  *DPSEngine::load(const char * src)
{
	DPSOvoReader reader{ src };
	return reader.load(&textures, &materials);
}

/**
*	Free the pre-allocated resources
*
*	@author		DPS
*	@param
*	@return
*
*/
void LIB_API DPSEngine::freeEngine()
{
	DPSTexture::deinitializeFreeImage();

	for (int i = 0; i < textures.size(); i++) {
		textures.at(i)->~DPSTexture();
		free(textures.at(i));
	}

	for (int i = 0; i < materials.size(); i++) {
		free(materials.at(i));
	}

	//DPSList will be automatically free, by the distructor
}


void LIB_API DPSEngine::clearRenderList()
{
	renderList.clear();
}

void LIB_API DPSEngine::passNodeToRenderList(DPSNode * node)
{
	node->computeWorldCoordinates();
	renderList.pass(node);
}