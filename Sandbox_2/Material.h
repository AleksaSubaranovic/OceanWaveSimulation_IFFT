#pragma once
#include <glm/glm.hpp>
#include "Texture.h"

#define DIFFUSE_TEX_UNIT 0
#define SPECULAR_TEX_UNIT 1
#define EMISSION_TEX_UNIT 2
#define NORMAL_TEX_UNIT 3

struct Material
{
	//glm::vec3 ambient;
	//glm::vec3 diffuse;
	//glm::vec3 specular;
	float shininess = 32;

	Texture diffuseTex, specularTex, emissionTex;;

	Material();
	Material(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const std::string& diffuseTexPath, const std::string& specularTexPath) = delete;
	Material(const std::string& diffuseTexPath, const std::string& specularTexPath);


	void SendToShader(class Shader& shader);
};

