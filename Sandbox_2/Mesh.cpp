#include "Mesh.h"
#include "Shader.h"
#include <fstream>
#include "Generic.h"
#include <string>
#include <sstream>
void Mesh::initObjects()
{
	Vertex::CalculateNormals(&indices[0], indices.size(), &vertices[0], vertices.size());
	//Vertex::CalculateTangents(&indices[0], indices.size(), &vertices[0], vertices.size()); // if normal texture is set

	vao = new VAO();

	vbo = new VBO(&vertices[0], vertices.size() * sizeof(Vertex), false);

	ebo = new EBO(&indices[0], indices.size() * sizeof(uint32_t), false);
	
	vao->AttribPointerAndEnable_Vertex();

	glBindVertexArray(0);
}

Mesh::Mesh()
{
	// nothing yet,
}

Mesh::~Mesh()
{
	delete vao;
	delete vbo;
	delete ebo;
}

void Mesh::setup(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices)
{
	this->vertices = std::move(vertices); // or swap

	this->indices = std::move(indices);

	initObjects();
}

void Mesh::SetDiffuseTex(const std::string& diffuseTexPath)
{
	material.diffuseTex.load(diffuseTexPath);
}

void Mesh::SetSpecularTex(const std::string& specularTexPath)
{
	material.specularTex.load(specularTexPath);
}

void Mesh::SetEmissionTex(const std::string& emissionTexPath)
{
	material.emissionTex.load(emissionTexPath);
}

void Mesh::SetNormalTex(const std::string& normalTexPath)
{
	normalTexture.load(normalTexPath);
}

// set model, view, perspective first, light if any and number of lights

void Mesh::DrawMesh(Shader& s)
{
	material.SendToShader(s);
	if (normalTexture.isInitialized())
	{
		int loc = glGetUniformLocation(s.getShaderId(), "normalTex");
		glUniform1i(loc, NORMAL_TEX_UNIT);

		normalTexture.Bind(NORMAL_TEX_UNIT);
		s.setBool("shouldUseNormalTex", true);
	}
	else s.setBool("shouldUseNormalTex", false);
	
	vao->Bind();
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::ReadFromOBJ(std::vector<Vertex>& vertices, std::vector<unsigned int> indices, const std::string& filePath)
{
	PRINT_ERROR("NOT IMPLEMENTED, READ obj file");
	return;

	using namespace glm;
	using arr_vec3 = std::vector<glm::vec3>;
	using arr_vec2 = std::vector<glm::vec2>;
	using arr = std::vector<GLint>;

	arr_vec2 texCoords;
	arr_vec3 positions, normals;
	arr positionInd, texCoordInd, normalInd;

	std::vector<Vertex> v;

	std::stringstream ss;
	std::ifstream file(filePath);
	std::string line = "";
	std::string prefix = "";
	vec3 tempVec3;
	vec2 tempVec2;
	GLint tempInt;

	if (!file.is_open())
	{
		PRINT_ERROR("FILE OPEN");
		PRINT_ERROR(filePath);
	}

	while (std::getline(file, line))
	{
		ss.clear();
		ss.str(line);
		ss >> prefix;

		if (prefix == "v")
		{
			ss >> tempVec3.x >> tempVec3.y >> tempVec3.z;
			positions.push_back(tempVec3);
		}
		else if (prefix == "vt")
		{
			ss >> tempVec2.x >> tempVec2.y;
			texCoords.push_back(tempVec2);
		}
		else if (prefix == "vn")
		{
			ss >> tempVec3.x >> tempVec3.y >> tempVec3.z;
			normals.push_back(tempVec3);
		}
		else if (prefix == "f")
		{
			int counter = 0;
			while (ss >> tempInt)
			{
				if (counter == 0)
				{
					positionInd.push_back(tempInt);
				}
				else if (counter == 1)
				{
					texCoordInd.push_back(tempInt);
				}
				else if (counter == 2)
				{
					normalInd.push_back(tempInt);
				}

				if (ss.peek() == '/')
				{
					counter++;
					ss.ignore(1, '/');
				}
				else if (ss.peek() == ' ')
				{
					counter++;
					ss.ignore(1, ' ');
				}

				if (counter > 2) counter = 0;
			}
		}
	}//

	v.resize(positions.size(), Vertex());

	for (int i = 0; i < positionInd.size(); i++)
	{
		
	}
}
