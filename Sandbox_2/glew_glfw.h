#pragma once

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Vertex.h"

// Vertex Buffer Object
struct VBO
{
	GLuint vbo;

	VBO(float* data, size_t size, bool isDynamic);
	VBO(Vertex* data, size_t size, bool isDynamic);
	VBO(void* data, size_t size, bool isDynamic);

	void Bind();
};

// Verter Array Object
struct VAO
{
	GLuint vao;

	VAO();

	void Bind();

	void AttribPointerAndEnable(GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);

	void AttribPointerAndEnable_Vertex();
};

// Element Buffer Object
struct EBO
{
	GLuint ebo;

	EBO(const void* data, size_t size, bool isDynamic);

	void Bind();
};

struct FrameBuffer
{
	GLuint fbo;

	FrameBuffer();

	void Bind();

	void UnBind();

	void AttachTexture(class Texture* texture);
};

struct RenderBuffer
{
	GLuint rbo;

	RenderBuffer();
};