#include "DPSEngine.h"

// Glew (include it before GL.h):
#include <GL/glew.h>


///////////////
// OVOREADER //
///////////////

class OvObject {
public:
	enum class Type : int {

		// Types:
		OBJECT = 0,
		NODE,
		OBJECT2D,
		OBJECT3D,
		LIST,

		// Derived classes:
		BUFFER,
		SHADER,
		TEXTURE,
		FILTER,
		MATERIAL,
		FBO,
		QUAD,
		BOX,
		SKYBOX,
		FONT,
		CAMERA,
		LIGHT,
		BONE,
		MESH,
		SKINNED,
		INSTANCED,
		PIPELINE,
		EMITTER,

		// Animation type
		ANIM,

		// Physics related:
		PHYSICS,

		// Terminator:
		LAST,
	};
};

// Stripped-down redefinition of OvMesh (just for the subtypes):
class LIB_API OvMesh {
public:
	enum class Subtype : int {
		// Foundation types:
		DEFAULT = 0,
		NORMALMAPPED,
		TESSELLATED,

		// Terminator:
		LAST,
	};
};

// Stripped-down redefinition of OvLight (just for the subtypes):
class LIB_API OvLight {
public:
	enum class Subtype : int {
		// Foundation types:
		OMNI = 0,
		DIRECTIONAL,
		SPOT,

		// Terminator:
		LAST,
	};
};

/**
*	Set src
*
*	@author		DPS
*	@param		const char *
*	@return		void
*
*/
LIB_API DPSOvoReader::DPSOvoReader(const char * src)
{
	this->src = src;
}


/**
*	Destructor of DPSOvoReader
*
*	@author		DPS
*	@param		void
*	@return		bool
*
*/
LIB_API DPSOvoReader::~DPSOvoReader(){}


/**
*	Load the .OVO file
*
*	@author		DPS
*	@param		const char * src
*	@return		DPSNode *
*
*/
DPSNode LIB_API * DPSOvoReader::load(std::vector<DPSTexture*> *textures, std::vector<DPSMaterial*> *materials)
{
	// Open file:
	FILE *dat = fopen(src, "rb");

	if (dat == nullptr) {
		std::cout << "ERROR: unable to open file '" << src << "'" << std::endl;
		return nullptr;
	}

	unsigned int position = 0;

	readChunk(dat, nullptr, textures, materials);

	fclose(dat);

	std::cout << "\nFile parsed" << std::endl;

	return rootNode;
}

/**
*	Method for create the tree structure and the render list
*
*	@author		DPS
*	@param		FILE* dat , DPSNode* parent
*	@return		void
*
*/
void LIB_API DPSOvoReader::readChunk(FILE* dat, DPSNode* parent, std::vector<DPSTexture*> *textures, std::vector<DPSMaterial*> *materials)
{
	unsigned int position = 0;
	unsigned int chunkId, chunkSize;

	//Check if EOF
	fread(&chunkId, sizeof(unsigned int), 1, dat);

	if (feof(dat))  //Break Condition
		return;

	fread(&chunkSize, sizeof(unsigned int), 1, dat);

	//cout << "\n[chunk id : " << chunkId << ", chunk size : " << chunkSize << ", chunk type : ";

	// Load whole chunk into memory:
	char *data = new char[chunkSize];

	if (fread(data, sizeof(char), chunkSize, dat) != chunkSize) { //Break Condition
		fclose(dat);
		delete[] data;
		return;
	}

	switch ((OvObject::Type) chunkId) {
		//CASE OBJECT
		case OvObject::Type::OBJECT: {
			//cout << "version]" << endl;

			// OVO revision number:
			unsigned int versionId;
			memcpy(&versionId, data + position, sizeof(unsigned int));
			//cout << "   Version . . . :  " << versionId << endl;
			position += sizeof(unsigned int);

			readChunk(dat, nullptr, textures, materials);

			delete[] data;
			return;
		}

		//CASE NODE
		case OvObject::Type::NODE: {
			//cout << "node]" << endl;

			DPSNode* node = new DPSNode();

			// Node name:
			char nodeName[FILENAME_MAX];
			strcpy(nodeName, data + position);
			node->setName(nodeName);
			position += (unsigned int)strlen(nodeName) + 1;

			// Node matrix:
			glm::mat4 matrix;
			memcpy(&matrix, data + position, sizeof(glm::mat4));
			node->setMatrix(matrix);
			position += sizeof(glm::mat4);

			if (parent == nullptr) 
				rootNode = node;
			else {
				parent->link(node);
				node->setParent(parent);
				//cout << "   Parent Name . :  " << parent->getName() << endl;
			}

			//cout << "   Name  . . . . :  " << node->getName() << endl;

			// Nr. of children nodes:
			unsigned int children;
			memcpy(&children, data + position, sizeof(unsigned int));
			//cout << "   Nr. children  :  " << children << endl;

			position += sizeof(unsigned int);

			// Optional target node, [none] if not used:
			char targetName[FILENAME_MAX];
			strcpy(targetName, data + position);
			//cout << "   Target node . :  " << targetName << endl;
			position += (unsigned int)strlen(targetName) + 1;

			// read all children data
			while ((children) != 0) {
				readChunk(dat, node, textures, materials);
				children--;
			}

			delete[] data;
			return;
		}

		//CASE MATERIAL
		case OvObject::Type::MATERIAL: {

			DPSMaterial* material = new DPSMaterial();

			// Material name:
			char materialName[FILENAME_MAX];
			strcpy(materialName, data + position);
			//cout << "   Name  . . . . :  " << materialName << endl;
			position += (unsigned int)strlen(materialName) + 1;
			material->setName(materialName);

			// Material term colors, starting with emissive:
			glm::vec3 emission, albedo;
			memcpy(&emission, data + position, sizeof(glm::vec3));
			//cout << "   Emission  . . :  " << emission.r << ", " << emission.g << ", " << emission.b << endl;
			position += sizeof(glm::vec3);

			// Albedo:
			memcpy(&albedo, data + position, sizeof(glm::vec3));
			//cout << "   Albedo  . . . :  " << albedo.r << ", " << albedo.g << ", " << albedo.b << endl;
			position += sizeof(glm::vec3);

			// Roughness factor:
			float roughness;
			memcpy(&roughness, data + position, sizeof(float));
			//cout << "   Roughness . . :  " << roughness << endl;
			position += sizeof(float);

			// Metalness factor:
			float metalness;
			memcpy(&metalness, data + position, sizeof(float));
			//cout << "   Metalness . . :  " << metalness << endl;
			position += sizeof(float);

			//Transparency
			float alpha;
			memcpy(&alpha, data + position, sizeof(float));
			//cout << "   Transparency  :  " << alpha << endl;
			position += sizeof(float);

			material->setShininess(roughness);
			material->setProprierties(albedo, alpha);

			// Albedo texture filename, or [none] if not used:
			char textureName[FILENAME_MAX];
			strcpy(textureName, data + position);
			//cout << "   Albedo tex. . :  " << textureName << endl;
			position += (unsigned int)strlen(textureName) + 1;

			bool textureFound = false;

			// Check if Texture exists
			for (DPSTexture* tex : *textures) {
				if (tex->getName() == textureName) {
					material->setTexture(tex);
					textureFound = true;
					break;
				}
			}

			if (!textureFound) {
				DPSTexture* texture = new DPSTexture();
				texture->setName(textureName);
				texture->loadTexture(textureName);
				//cout << "   File Name tex.:  " << textureName << endl;
				textures->push_back(texture);
				material->setTexture(texture);
			}

			// Normal map filename, or [none] if not used:
			char normalMapName[FILENAME_MAX];
			strcpy(normalMapName, data + position);
			//cout << "   Normalmap tex.:  " << normalMapName << endl;
			position += (unsigned int)strlen(normalMapName) + 1;

			// Height map filename, or [none] if not used:
			char heightMapName[FILENAME_MAX];
			strcpy(heightMapName, data + position);
			//cout << "   Heightmap tex.:  " << heightMapName << endl;
			position += (unsigned int)strlen(heightMapName) + 1;

			// Roughness map filename, or [none] if not used:
			char roughnessMapName[FILENAME_MAX];
			strcpy(roughnessMapName, data + position);
			//cout << "   Roughness tex.:  " << roughnessMapName << endl;
			position += (unsigned int)strlen(roughnessMapName) + 1;

			// Metalness map filename, or [none] if not used:
			char metalnessMapName[FILENAME_MAX];
			strcpy(metalnessMapName, data + position);
			//cout << "   Metalness tex.:  " << metalnessMapName << endl;
			position += (unsigned int)strlen(metalnessMapName) + 1;

			materials->push_back(material);

			readChunk(dat, parent, textures, materials);

			delete[] data;
			return;
		}
	
		// CASE MESH/SKINNED
		case OvObject::Type::MESH:
		case OvObject::Type::SKINNED: {
			// Both standard and skinned meshes are handled through this case:
			bool isSkinned = false;
			if ((OvObject::Type) chunkId == OvObject::Type::SKINNED) {
				isSkinned = true;
				//cout << "skinned mesh]" << endl;
			} else {
			//cout << "mesh]" << endl;
			}

			DPSMesh* mesh = new DPSMesh();

			// Mesh name:
			char meshName[FILENAME_MAX];
			strcpy(meshName, data + position);
			position += (unsigned int)strlen(meshName) + 1;
			//cout << "   Parent Name . :  " << parent->getName() << endl;
			//cout << "   Name  . . . . :  " << meshName << endl;
			mesh->setName(meshName);
			mesh->setParent(parent);

			// Mesh matrix:
			glm::mat4 matrix;
			memcpy(&matrix, data + position, sizeof(glm::mat4));
			position += sizeof(glm::mat4);
			mesh->setMatrix(matrix);

			// Mesh nr. of children nodes:
			unsigned int children;
			memcpy(&children, data + position, sizeof(unsigned int));
			//cout << "   Nr. children  :  " << children << endl;
			position += sizeof(unsigned int);

			// Optional target node, or [none] if not used:
			char targetName[FILENAME_MAX];
			strcpy(targetName, data + position);
			//cout << "   Target node . :  " << targetName << endl;
			position += (unsigned int)strlen(targetName) + 1;

			// Mesh subtype (see OvMesh SUBTYPE enum):
			unsigned char subtype;
			memcpy(&subtype, data + position, sizeof(unsigned char));
			char subtypeName[FILENAME_MAX];

			switch ((OvMesh::Subtype) subtype)
			{
				case OvMesh::Subtype::DEFAULT: strcpy(subtypeName, "standard"); break;
				case OvMesh::Subtype::NORMALMAPPED: strcpy(subtypeName, "normal - mapped"); break;
				case OvMesh::Subtype::TESSELLATED: strcpy(subtypeName, "tessellated"); break;
				default: strcpy(subtypeName, "UNDEFINED");
			}

			//cout << "   Subtype . . . :  " << (int)subtype << " (" << subtypeName << ")" << endl;
			position += sizeof(unsigned char);

			// Nr. of vertices:
			unsigned int vertices, faces;
			memcpy(&vertices, data + position, sizeof(unsigned int));
			//cout << "   Nr. vertices  :  " << vertices << endl;
			position += sizeof(unsigned int);

			// ...and faces:
			memcpy(&faces, data + position, sizeof(unsigned int));
			//cout << "   Nr. faces . . :  " << faces << endl;
			position += sizeof(unsigned int);

			// Material name, or [none] if not used:
			char materialName[FILENAME_MAX];
			strcpy(materialName, data + position);
			//cout << "   Material  . . :  " << materialName << endl;
			position += (unsigned int)strlen(materialName) + 1;

			for (int i = 0; i < materials->size(); i++) {
				if (materials->at(i)->getName() == materialName) {
					mesh->setMaterial(materials->at(i));
					break;
				}
			}

			// Mesh bounding sphere radius:
			float radius;
			memcpy(&radius, data + position, sizeof(float));
			//cout << "   Radius  . . . :  " << radius << endl;
			position += sizeof(float);

			// Mesh bounding box minimum corner:
			glm::vec3 bBoxMin;
			memcpy(&bBoxMin, data + position, sizeof(glm::vec3));
			//cout << "   BBox minimum  :  " << bBoxMin.x << ", " << bBoxMin.y << ", " << bBoxMin.z << endl;
			position += sizeof(glm::vec3);

			// Mesh bounding box maximum corner:
			glm::vec3 bBoxMax;
			memcpy(&bBoxMax, data + position, sizeof(glm::vec3));
			//cout << "   BBox maximum  :  " << bBoxMax.x << ", " << bBoxMax.y << ", " << bBoxMax.z << endl;
			position += sizeof(glm::vec3);

			// Optional physics properties:
			unsigned char hasPhysics;
			memcpy(&hasPhysics, data + position, sizeof(unsigned char));
			//cout << "   Physics . . . :  " << (int)hasPhysics << endl;
			position += sizeof(unsigned char);

			if (hasPhysics) {
				struct PhysProps {
					unsigned char type;
					unsigned char contCollisionDetection;
					unsigned char collideWithRBodies;
					unsigned char hullType;

					// Vector data:
					glm::vec3 massCenter;

					// Mesh properties:
					float mass;
					float staticFriction;
					float dynamicFriction;
					float bounciness;
					float linearDamping;
					float angularDamping;
					void *physObj;
				};

				PhysProps mp;
				memcpy(&mp, data + position, sizeof(PhysProps));
				position += sizeof(PhysProps);
			}

			//Cretae temp arraya
			float *vertexArray = new float[vertices * 3];
			unsigned int *facesArray = new unsigned int[faces * 3];
			float *normalArray = new float[vertices * 3];
			float *textureArray = new float[vertices * 2];

			mesh->numberOfVertices = vertices;
			mesh->numberOfFaces = faces;


			// Interleaved and compressed vertex/normal/UV/tangent data:
			for (unsigned int c = 0; c < vertices; c++) {

				// Vertex coords:
				glm::vec3 v;
				memcpy(&v, data + position, sizeof(glm::vec3));
				position += sizeof(glm::vec3);
				vertexArray[c * 3] = v.x;
				vertexArray[c * 3 + 1] = v.y;
				vertexArray[c * 3 + 2] = v.z;

				// Vertex normal:
				unsigned int normalData;
				memcpy(&normalData, data + position, sizeof(unsigned int));
				position += sizeof(unsigned int);
				glm::vec4 n = glm::unpackSnorm3x10_1x2(normalData);
				normalArray[c * 3] = n.x;
				normalArray[c * 3 + 1] = n.y;
				normalArray[c * 3 + 2] = n.z;


				// Texture coordinates:
				unsigned short textureData[2];
				memcpy(textureData, data + position, sizeof(unsigned short) * 2);
				position += sizeof(unsigned short) * 2;

				glm::vec2 uv;
				uv.x = glm::unpackHalf1x16(textureData[0]);
				uv.y = glm::unpackHalf1x16(textureData[1]);
			

				textureArray[c * 2] = uv.x;
				textureArray[c * 2 +1] = uv.y;

				// Tangent vector:
				unsigned int tangentData;
				memcpy(&tangentData, data + position, sizeof(unsigned int));
				position += sizeof(unsigned int);
			}

			// Faces:
			for (unsigned int c = 0; c < faces; c++) {
				// Face indexes:
				unsigned int f[3];
				memcpy(f, data + position, sizeof(unsigned int) * 3);
				position += sizeof(unsigned int) * 3;
				facesArray[c * 3] = f[0];
				facesArray[c * 3 + 1] = f[1];
				facesArray[c * 3 + 2] = f[2];
			}

			// BIND VBO
			// Generate a vertex buffer and bind it:
			glGenBuffers(1, &mesh->vertexVbo);
			glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexVbo);
			glBufferData(GL_ARRAY_BUFFER, mesh->numberOfVertices * 3 * sizeof(float), vertexArray, GL_STATIC_DRAW);

			// Generate a normal buffer and bind it:
			glGenBuffers(1, &mesh->normalVbo);
			glBindBuffer(GL_ARRAY_BUFFER, mesh->normalVbo);
			glBufferData(GL_ARRAY_BUFFER, mesh->numberOfVertices * 3 * sizeof(float), normalArray, GL_STATIC_DRAW);

			// Generate a texture buffer and bind it:
			glGenBuffers(1, &mesh->textureVbo);
			glBindBuffer(GL_ARRAY_BUFFER, mesh->textureVbo);
			glBufferData(GL_ARRAY_BUFFER, mesh->numberOfVertices * 2 * sizeof(float), textureArray, GL_STATIC_DRAW);

			//TODO::: ADDDDD
			// Generate a face buffer and bind it: 
			glGenBuffers(1, &mesh->facesVbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->facesVbo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->numberOfFaces * 3 * sizeof(unsigned int), facesArray, GL_STATIC_DRAW);

			// Extra information for skinned meshes:
			if (isSkinned) {

				// Initial mesh pose matrix:
				glm::mat4 poseMatrix;
				memcpy(&poseMatrix, data + position, sizeof(glm::mat4));
				position += sizeof(glm::vec4);

				// Bone list:
				unsigned int nrOfBones;
				memcpy(&nrOfBones, data + position, sizeof(unsigned int));
				//cout << "   Nr. bones . . :  " << nrOfBones << endl;
				position += sizeof(unsigned int);

				for (unsigned int c = 0; c < nrOfBones; c++) {

					// Bone name:
					char boneName[FILENAME_MAX];
					strcpy(boneName, data + position);
					//cout << "      Bone name  :  " << boneName << " (" << c << ")" << endl;
					position += (unsigned int)strlen(boneName) + 1;

					// Initial bone pose matrix (already inverted):
					glm::mat4 boneMatrix;
					memcpy(&boneMatrix, data + position, sizeof(glm::mat4));
					position += sizeof(glm::mat4);
				}

				// Per vertex bone weights and indexes:
				for (unsigned int c = 0; c < vertices; c++) {

					// Bone indexes:
					unsigned int boneIndex[4];
					memcpy(boneIndex, data + position, sizeof(unsigned int) * 4);
					position += sizeof(unsigned int) * 4;

					// Bone weights:
					unsigned short boneWeightData[4];
					memcpy(boneWeightData, data + position, sizeof(unsigned short) * 4);
					position += sizeof(unsigned short) * 4;
				}
			}

			//add mesh into tree structure
			parent->link(mesh);

			while ((children) != 0) {
				readChunk(dat, mesh, textures, materials);
				children--;
			}

			delete[] data;
			return;
		}

		// CASE LIGHT
		case OvObject::Type::LIGHT: {
			DPSLight* light = new DPSLight();

			// Light name:
			char lightName[FILENAME_MAX];
			strcpy(lightName, data + position);
			position += (unsigned int)strlen(lightName) + 1;
			light->setName(lightName);
			light->setParent(parent);

			//cout << "   Parent Name . :  " << parent->getName() << endl;
			//cout << "   Name  . . . . :  " << lightName << endl;

			// Light matrix:
			glm::mat4 matrix;
			memcpy(&matrix, data + position, sizeof(glm::mat4));
			position += sizeof(glm::mat4);

			light->setMatrix(matrix);

			// Nr. of children nodes:
			unsigned int children;
			memcpy(&children, data + position, sizeof(unsigned int));
			//cout << "   Nr. children  :  " << children << endl;
			position += sizeof(unsigned int);

			// Optional target node name, or [none] if not used:
			char targetName[FILENAME_MAX];
			strcpy(targetName, data + position);
			//cout << "   Target node . :  " << targetName << endl;
			position += (unsigned int)strlen(targetName) + 1;

			// Light subtype (see OvLight SUBTYPE enum):
			unsigned char subtype;
			memcpy(&subtype, data + position, sizeof(unsigned char));
			char subtypeName[FILENAME_MAX];

			switch ((OvLight::Subtype) subtype)
			{
				case OvLight::Subtype::DIRECTIONAL:
					strcpy(subtypeName, "directional");
					light->setLightType(DIRECTIONAL);
					break;
				case OvLight::Subtype::OMNI:
					strcpy(subtypeName, "omni");
					light->setLightType(OMNI);
					break;
				case OvLight::Subtype::SPOT:
					strcpy(subtypeName, "spot");
					light->setLightType(SPOT);
					break;
				default: strcpy(subtypeName, "UNDEFINED");
			}

			//cout << "   Subtype . . . :  " << (int)subtype << " (" << subtypeName << ")" << endl;
			position += sizeof(unsigned char);

			// Light color:
			glm::vec3 color;
			memcpy(&color, data + position, sizeof(glm::vec3));
			//cout << "   Color . . . . :  " << color.r << ", " << color.g << ", " << color.b << endl;
			position += sizeof(glm::vec3);
			light->setColor(color);

			// Influence radius:
			float radius;
			memcpy(&radius, data + position, sizeof(float));
			//cout << "   Radius  . . . :  " << radius << endl;
			position += sizeof(float);
			light->setRadius(radius);

			// Direction:
			glm::vec3 direction;
			memcpy(&direction, data + position, sizeof(glm::vec3));
			//cout << "   Direction . . :  " << direction.r << ", " << direction.g << ", " << direction.b << endl;
			position += sizeof(glm::vec3);
			light->setDirection(direction);

			// Cutoff:
			float cutoff;
			memcpy(&cutoff, data + position, sizeof(float));
			//cout << "   Cutoff  . . . :  " << cutoff << endl;
			position += sizeof(float);
			light->setCutoff(cutoff);

			light->setPosition(matrix[3]);

			// Exponent:
			float spotExponent;
			memcpy(&spotExponent, data + position, sizeof(float));
			//cout << "   Spot exponent :  " << spotExponent << endl;
			position += sizeof(float);
			//TODO -> Chiedere se implementare EXPONENT!

			// Cast shadow flag:
			unsigned char castShadows;
			memcpy(&castShadows, data + position, sizeof(unsigned char));
			//cout << "   Cast shadows  :  " << (int)castShadows << endl;
			position += sizeof(unsigned char);

			// Volumetric lighting flag:
			unsigned char isVolumetric;
			memcpy(&isVolumetric, data + position, sizeof(unsigned char));
			//cout << "   Volumetric  . :  " << (int)isVolumetric << endl;
			position += sizeof(unsigned char);

			//Turn light on
			light->turnOn();

			//add light into tree structure
			parent->link(light);

			while ((children) != 0) {
				readChunk(dat, light, textures, materials);
				children--;
			}

			delete[] data;
			return;	
		}
	
		// CASE BONE
		case OvObject::Type::BONE: {
			// Bone name:
			char boneName[FILENAME_MAX];
			strcpy(boneName, data + position);
			//cout << "   Name  . . . . :  " << boneName << endl;
			position += (unsigned int)strlen(boneName) + 1;

			// Bone matrix:
			glm::mat4 matrix;
			memcpy(&matrix, data + position, sizeof(glm::mat4));
			position += sizeof(glm::mat4);

			// Nr. of children nodes:
			unsigned int children;
			memcpy(&children, data + position, sizeof(unsigned int));
			//cout << "   Nr. children  :  " << children << endl;
			position += sizeof(unsigned int);

			// Optional target node, or [none] if not used:
			char targetName[FILENAME_MAX];
			strcpy(targetName, data + position);
			//cout << "   Target node . :  " << targetName << endl;
			position += (unsigned int)strlen(targetName) + 1;

			// Mesh bounding box minimum corner:
			glm::vec3 bBoxMin;
			memcpy(&bBoxMin, data + position, sizeof(glm::vec3));
			//cout << "   BBox minimum  :  " << bBoxMin.x << ", " << bBoxMin.y << ", " << bBoxMin.z << endl;
			position += sizeof(glm::vec3);

			// Mesh bounding box maximum corner:
			glm::vec3 bBoxMax;
			memcpy(&bBoxMax, data + position, sizeof(glm::vec3));
			//cout << "   BBox maximum  :  " << bBoxMax.x << ", " << bBoxMax.y << ", " << bBoxMax.z << endl;
			position += sizeof(glm::vec3);

			delete[] data;

			return;
		}

		default:
			std::cout << "UNKNOWN]" << std::endl;
			std::cout << "ERROR: corrupted or bad data in file " << std::endl;
			fclose(dat);
			delete[] data;

			return;
	}
}