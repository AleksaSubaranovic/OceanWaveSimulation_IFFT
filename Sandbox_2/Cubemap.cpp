#include "Cubemap.h"
#include "stb/stb_image.h"
#include "Generic.h"
#include "Shader.h"

Cubemap::Cubemap()
{
}

void Cubemap::Bind(int ind)
{
	glActiveTexture(GL_TEXTURE0 + ind);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
}

void Cubemap::load(std::string faces[6])
{
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	unsigned char* data;
	for (int i = 0; i < 6; i++)
	{
		data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 4);

		if (data == NULL)
		{
			PRINT_ERROR("Image Loading failled (cubemap)");
			return;
		}

		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

Cubemap::~Cubemap()
{
	glDeleteTextures(1, &textureID);
}

void Cubemap::drawSkybox(Shader& s, const glm::mat4& view, const glm::mat4& projection)
{
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	s.activate();
	auto view2 = glm::mat4(glm::mat3(view)); // remove translation from the view matrix
	s.setMatrix4f("view", view2);
	s.setMatrix4f("projection", projection);

	// positions          
	static float skyboxVert[] = { -1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f };

	static VAO vao;
	static VBO vbo(skyboxVert, sizeof(skyboxVert), false);
	vao.Bind();
	vao.AttribPointerAndEnable(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	Bind(); // bind cubemap

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0); // maybe not needed
	glDepthFunc(GL_LESS);
}

const char* Cubemap::vSkyboxShader = R"glsl(
#version 330 core
layout (location = 0) in vec3 position;

out vec3 TexCoord;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoord = position;
    vec4 pos = projection * view * vec4(position, 1.0);
    gl_Position = pos.xyww;
}  
)glsl";

const char* Cubemap::fSkyboxShader = R"glsl(
#version 330 core
out vec4 FragColor;

in vec3 TexCoord;

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, TexCoord);
}
)glsl";