#pragma once

#include <string>
#include "glew_glfw.h"

class Texture
{
	int width, height, nrChannels; // width, height and color channels
	GLuint texture; // single texture
	std::string filePath;

public:
	void load(const std::string& path);
	
	void loadEmpty(int width, int height);
	// To store 32bit data
	void loadEmptyR32f(int width, int height, GLenum filter, GLenum wrap);
	void loadR32f(int width, int height, void* data, GLenum filter, GLenum wrap);
	void loadEmptyRGBA32f(int width, int height, GLenum filter, GLenum wrap);
	void loadRGBA32f(int width, int height, void* data, GLenum filter, GLenum wrap);
	// TODO: add function for loading image with texture parameters

	void swap(Texture& t);
	inline GLuint GetTexture() { return texture; }

	Texture();
	Texture(const std::string& path);
	~Texture();

	// Activate and Bind Texture 
	void Bind(int ind = 0);
	bool isInitialized();
};

