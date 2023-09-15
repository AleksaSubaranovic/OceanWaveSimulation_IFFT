#pragma once

#include "glew_glfw.h"
#include <string>
#include <glm/glm.hpp>

class Cubemap
{
	GLuint textureID;

public:
	Cubemap();
	void Bind(int ind = 0);
	void load(std::string faces[6]);

	~Cubemap();

	static const char* vSkyboxShader;
	static const char* fSkyboxShader;

	void drawSkybox(class Shader& s, const glm::mat4& view, const glm::mat4& projection);
};

