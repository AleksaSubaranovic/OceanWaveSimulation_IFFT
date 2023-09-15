#pragma once

#include <glm/glm.hpp>

struct Vertex
{
	glm::vec3 location;
	glm::vec3 normal;
	glm::vec2 tex;
	glm::vec3 tangent; // for normal maps

	Vertex();
	Vertex(glm::vec3 location, glm::vec2 tex);
	Vertex(glm::vec3 location, glm::vec3 normal, glm::vec2 tex);
	static void CalculateNormals(const uint32_t* indexes, uint32_t indexCount, Vertex* vertexes, uint32_t vertexCount);

	static void CalculateTangents(const uint32_t* indexes, uint32_t indexCount, Vertex* vertexes, uint32_t vertexCount);
};