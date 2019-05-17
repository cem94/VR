#include "DPSEngine.h"

// Glew (include it before GL.h):
#include <GL/glew.h>

// FreeGLUT:
#include <GL/freeglut.h>


 /**
 *	Constructor
 *
 *	@author		DPS
 *	@param		void
 *	@return		void
 *
 */
Shader::Shader() : type(TYPE_UNDEFINED), glId(0) {}


/**
*	Destructor
*
*	@author		DPS
*	@param		void
*	@return		void
*
*/
Shader::~Shader()
{
	if (glId)
	{
		switch (type)
		{
		case TYPE_VERTEX:
		case TYPE_FRAGMENT:
			glDeleteShader(glId);
			break;

		case TYPE_PROGRAM:
			glDeleteProgram(glId);
			break;
		}
	}
}

/**
 *	Returns the param location given its variable name
 *
 *	@author		DPS
 *	@param		const char * name
 *	@return		int
 *
 */
int Shader::getParamLocation(const char *name)
{
	// Safety net:
	if (name == nullptr)
	{
		std::cout << "[ERROR] Invalid params" << std::endl;
		return 0;
	}

	// Return location:
	int r = glGetUniformLocation(glId, name);
	if (r == -1)
		std::cout << "[ERROR] Param not found" << std::endl;
	return r;
}


/**
  *	Set shader matrix4 throught the given parameter
  *
  *	@author		DPS
  *	@param		int
  *	@param		mat4x4
  *	@return		int
  *
  */
void Shader::setMatrix(int param, const glm::mat4 & mat)
{
	glUniformMatrix4fv(param, 1, GL_FALSE, glm::value_ptr(mat));
}


/**
  *	Set shader matrix3 throught the given parameter
  *
  *	@author		DPS
  *	@param		int
  *	@param		mat4x4
  *	@return		int
  *
  */
void Shader::setMatrix3(int param, const glm::mat3 & mat)
{
	glUniformMatrix3fv(param, 1, GL_FALSE, glm::value_ptr(mat));
}


/**
  *	Set shader float parameter throught the given parameter
  *
  *	@author		DPS
  *	@param		int
  *	@param		mat4x4
  *	@return		int
  *
  */
void Shader::setFloat(int param, float value)
{
	glUniform1f(param, value);
}


/**
  *	Set shader int paramteter throught the given parameter
  *
  *	@author		DPS
  *	@param		int
  *	@param		int
  *	@return		int
  *
  */
void Shader::setInt(int param, int value)
{
	glUniform1i(param, value);
}


/**
  *	Set shader vec3 throught the given parameter
  *
  *	@author		DPS
  *	@param		int
  *	@param		vec3
  *	@return		int
  *
  */
void Shader::setVec3(int param, const glm::vec3 & vect)
{
	glUniform3fv(param, 1, glm::value_ptr(vect));
}


/**
  *	Set shader vec4 throught the given parameter
  *
  *	@author		DPS
  *	@param		int
  *	@param		vec4
  *	@return		void
  *
  */
void Shader::setVec4(int param, const glm::vec4 & vect)
{
	glUniform4fv(param, 1, glm::value_ptr(vect));
}


/**
  *	Loads and compiles a vertex, pixel or geometry shader from source code stored in memory.
  *
  *	@author		DPS
  *	@param		int
  *	@param		const char *
  *	@return		bool
  *
  */
bool Shader::loadFromMemory(int type, const char *data)
{
	// Safety net:
	if (data == nullptr)
	{
		std::cout << "[ERROR] Invalid params" << std::endl;
		return false;
	}

	// Check kind:
	int glKind = 0;
	switch (type)
	{
		//////////////////////
	case TYPE_VERTEX:		//
		glKind = GL_VERTEX_SHADER;
		break;

		//////////////////////
	case TYPE_FRAGMENT:		//
		glKind = GL_FRAGMENT_SHADER;
		break;

		//////////////////////
	default:				//
		std::cout << "[ERROR] Invalid kind" << std::endl;
		return false;
	}

	// Destroy if already loaded:
	if (glId)
		switch (type)
		{
		case TYPE_VERTEX:
		case TYPE_FRAGMENT:
			glDeleteShader(glId);
			break;
		default:
			std::cout << "[ERROR] Cannot reload a program as a shader" << std::endl;
			return false;
		}

	// Load program:
	glId = glCreateShader(glKind);
	if (glId == 0)
	{
		std::cout << "[ERROR] Unable to create shader object" << std::endl;
		return false;
	}

	glShaderSource(glId, 1, (const char **)&data, NULL);
	glCompileShader(glId);

	// Verify shader:
	int status;
	char buffer[MAX_LOGSIZE];
	int length = 0;
	memset(buffer, 0, MAX_LOGSIZE);

	glGetShaderiv(glId, GL_COMPILE_STATUS, &status);
	glGetShaderInfoLog(glId, MAX_LOGSIZE, &length, buffer);
	if (status == false)
	{
		std::cout << "[ERROR] Shader not compiled: " << buffer << std::endl;
		return false;
	}

	// Update values:
	this->type = type;

	// Done:
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Loads and compiles a vertex, pixel or geometry shader from a specified source text file.
 * @param subtype subtype of shader (pixel, vertex, geometry or program shader)
 * @param filename text file containing the vertex shader code
 * @return true/false on success/fail
 * @warning when a program is reloaded as shader (or viceversa), big trouble...
 */
 //TODO:: Possiamo eliminarla, visto che non lo utilizziamo
bool Shader::loadFromFile(int type, const char *filename)
{
	// Safety net:
	if (filename == nullptr)
	{
		std::cout << "[ERROR] Invalid params" << std::endl;
		return false;
	}

	// Open file:
	FILE *dat = fopen(filename, "rt");
	if (dat == nullptr)
	{
		std::cout << "[ERROR] Unable to open file" << std::endl;
		return false;
	}

	// Get file length:
	unsigned int length;
	fseek(dat, 0L, SEEK_END);
	length = ftell(dat);
	fseek(dat, 0L, SEEK_SET);

	// Read code:
	char *content = nullptr;
	if (length > 0)
	{
		content = new char[sizeof(char) * (length + 1)];
		length = (unsigned int)fread(content, sizeof(char), length, dat);
		content[length] = '\0';
	}
	fclose(dat);

	// Load from memory then:
	bool result = loadFromMemory(type, content);

	// Clean:
	delete[] content;

	// Done:
	return result;
}


/**
*	Loads and compiles a full program.
*
*	@author		DPS
*	@param		Shader *
*	@param		Shader *
*	@return		bool
*
*/
bool Shader::build(Shader *vertexShader, Shader *fragmentShader)
{
	// Safety net:
	if (vertexShader && vertexShader->type != TYPE_VERTEX)
	{
		std::cout << "[ERROR] Invalid vertex shader passed" << std::endl;
		return false;
	}
	if (fragmentShader && fragmentShader->type != TYPE_FRAGMENT)
	{
		std::cout << " [ERROR] Invalid fragment shader passed" << std::endl;
		return false;
	}

	// Delete if already used:
	if (glId)
	{
		// On reload, make sure it was a program before:
		if (this->type != TYPE_PROGRAM)
		{
			std::cout << "[ERROR] Cannot reload a shader as a program" << std::endl;
			return false;
		}
		glDeleteProgram(glId);
	}

	// Create program:
	glId = glCreateProgram();
	if (glId == 0)
	{
		std::cout << "[ERROR] Unable to create program" << std::endl;
		return false;
	}

	// Bind vertex shader:
	if (vertexShader)
		glAttachShader(glId, vertexShader->glId);

	// Bind fragment shader:
	if (fragmentShader)
		glAttachShader(glId, fragmentShader->glId);

	// Link program:
	glLinkProgram(glId);
	this->type = TYPE_PROGRAM;

	// Verify program:
	int status;
	char buffer[MAX_LOGSIZE];
	int length = 0;
	memset(buffer, 0, MAX_LOGSIZE);

	glGetProgramiv(glId, GL_LINK_STATUS, &status);
	glGetProgramInfoLog(glId, MAX_LOGSIZE, &length, buffer);
	if (status == false)
	{
		std::cout << "[ERROR] Program link error: " << buffer << std::endl;
		return false;
	}

	glValidateProgram(glId);
	glGetProgramiv(glId, GL_VALIDATE_STATUS, &status);
	if (status == GL_FALSE)
	{
		std::cout << "[ERROR] Unable to validate program" << std::endl;
		return false;
	}

	return true;
}


/**
*	Bind attribute to the shader attribute via location
*
*	@author		DPS
*	@param		int location
*	@param		const char *
*	@return		void
*
*/
void Shader::bind(int location, const char * attribName)
{
	glBindAttribLocation(glId, location, attribName);
}


/**
*	Render the shader
*
*	@author		DPS
*	@param		void *
*	@return		bool
*
*/
bool Shader::render(void *data)
{
	// Activate shader:
	if (glId)
		glUseProgram(glId);
	else
	{
		std::cout << "[ERROR] Invalid shader rendered" << std::endl;
		return false;
	}

	// Done:
	return true;
}