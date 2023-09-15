#pragma once
#include <glm/glm.hpp>
#include "Shader.h"
#include "Material.h"

struct BaseLight
{
	glm::vec3 ambient, diffuse, specular;

	virtual void SendToShader(Shader* shader, int ind, const std::string& type, std::string& s);

	BaseLight();

	// Multiply by intensity
	void Intensify(float intensity);
};

struct DirectionalLight : public BaseLight
{
	glm::vec3 direction;

	void SendToShader(Shader* shader, int ind, const std::string& type, std::string& s) override;

	DirectionalLight();
};

struct PointLight : public BaseLight
{
	glm::vec3 location;

	float constant;
	float linear;
	float quadratic;

	void SendToShader(Shader* shader, int ind, const std::string& type, std::string& s) override;

	PointLight();
};

struct SpotLight : public BaseLight
{
	float cutOff; // angle
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;

	glm::vec3 location;
	glm::vec3 direction;

	void SendToShader(Shader* shader, int ind, const std::string& type, std::string& s) override;

	SpotLight();
};

class LightShader : public Shader
{
	//void CompileShader(const char* vertexShader, const char* fragmentShader) override;

	static const char* fShaderSource;
	static const char* vShaderSource;


public:
	// Constructor
	LightShader();

	void SetLightsArrayLength(int numOfPointLights, int numOfSpotLights);
};
