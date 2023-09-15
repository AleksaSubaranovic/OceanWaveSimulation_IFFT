#include "Texture.h"
#include "stb/stb_image.h"
#include "Generic.h"

void Texture::load(const std::string& path)
{
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

	if (data == NULL)
	{
		PRINT_ERROR("Image Loading failled");
		PRINT_ERROR(path.c_str());
		return;
	}
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	// GL_RGB NIJE GL_RGBA               RGBAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	filePath = path;
}

void Texture::loadEmpty(int width, int height)
{
	this->height = height;
	this->width = width;
	this->filePath = "__FILE_PATH_NONE__";

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::loadEmptyR32f(int width, int height, GLenum filter, GLenum wrap)
{
	this->height = height;
	this->width = width;
	this->filePath = "__FILE_PATH_NONE__";

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

}

void Texture::loadR32f(int width, int height, void* data, GLenum filter, GLenum wrap)
{
	this->height = height;
	this->width = width;
	this->filePath = "__FILE_PATH_NONE__";

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
}

void Texture::loadEmptyRGBA32f(int width, int height, GLenum filter, GLenum wrap)
{
	this->height = height;
	this->width = width;
	this->filePath = "__FILE_PATH_NONE__";

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
}

void Texture::loadRGBA32f(int width, int height, void* data, GLenum filter, GLenum wrap)
{
	this->height = height;
	this->width = width;
	this->filePath = "__FILE_PATH_NONE__";

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

	printf("loadRGBA32F is not correctly implemented");
}

void Texture::swap(Texture& t)
{
	GLuint temp = texture;
	texture = t.texture;
	t.texture = temp;

	std::swap(width, t.width);
	std::swap(height, t.height);
	std::swap(filePath, t.filePath);
}

Texture::Texture()
{
	height = width = nrChannels = -1;
	filePath = "";
}

Texture::Texture(const std::string& path)
{
	load(path);
	filePath = path;
}
Texture::~Texture()
{
	glDeleteTextures(1, &texture);
	texture = 0;
}

void Texture::Bind(int ind)
{
	glActiveTexture(GL_TEXTURE0 + ind);
	glBindTexture(GL_TEXTURE_2D, texture);
}

bool Texture::isInitialized()
{
	return filePath != "";
}
