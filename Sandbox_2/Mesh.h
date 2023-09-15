#pragma once

#include "glew_glfw.h"

#include <vector>
#include "Vertex.h"
#include "Texture.h"
#include "Material.h"

class Mesh
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	
	// standard will be 1 diffuse, spec, emissive and normal
	Material material; // must have diffuse (regular)
	Texture normalTexture; // optional

	VAO* vao;
	VBO* vbo;
	EBO* ebo;

	void initObjects();

public:
	Mesh();
	~Mesh();

	// Returns refrence!
	inline std::vector<Vertex> & getVertices() { return vertices; }
	// Returns refrence!
	inline std::vector<unsigned int>& getIndices() { return indices; }
	// Returns refrence!
	inline Material& getMaterial() { return material; }
	// Returns refrence!
	inline Texture& getNormalTexture() { return normalTexture; }

	inline VAO* getVAO() { return vao; }

	void setup(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices);

	void SetDiffuseTex(const std::string& diffuseTexPath);

	void SetSpecularTex(const std::string& specularTexPath);

	void SetEmissionTex(const std::string& emissionTexPath);

	void SetNormalTex(const std::string& normalTexPath);

	// PASS ALL UNIFORMS OUTSIDE THIS CLASS FIRST
	// PASS CAMERA LOCATION, MODEL, VIEW AND PERSPECTIVE BEFORE THIS !!!
	// ARG: ACTIVE SHADER
	// DRAWS MESH WITH MATERIAL
	void DrawMesh(class Shader& s);

	static void ReadFromOBJ(std::vector<Vertex>& vertices, std::vector<unsigned int> indices, const std::string& filePath);
};

