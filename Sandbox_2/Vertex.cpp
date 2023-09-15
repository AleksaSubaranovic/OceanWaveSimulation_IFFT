#include "Vertex.h"
#include <glm/vec3.hpp>

using namespace glm;
Vertex::Vertex()
{
	tangent = vec3(0.0f,0.0f,0.0f);
}

Vertex::Vertex(glm::vec3 location, glm::vec2 tex) : location(location), tex(tex)
{
	normal = vec3(0.f, 0.f, 0.f);
	tangent = vec3(0.0f, 0.0f, 0.0f);
}

Vertex::Vertex(glm::vec3 location, glm::vec3 normal, glm::vec2 tex) : location(location), tex(tex), normal(normal)
{
	tangent = vec3(0.0f, 0.0f, 0.0f);
}

void Vertex::CalculateNormals(const uint32_t* indexes, uint32_t indexCount, Vertex* vertexes, uint32_t vertexCount)
{
	for (uint32_t i = 0; i < indexCount; i += 3)
	{
		uint32_t i0 = indexes[i], i1 = indexes[i + 1], i2 = indexes[i + 2];
		vec3 v1 = vertexes[i1].location - vertexes[i0].location;
		vec3 v2 = vertexes[i2].location - vertexes[i0].location;

		vec3 normal = cross(v1, v2);
		vertexes[i0].location += normal;
		vertexes[i1].location += normal;
		vertexes[i2].location += normal;
	}
	for (uint32_t i = 0; i < vertexCount; i++)
	{
		vertexes[i].location = normalize(vertexes[i].location);
	}
}

void Vertex::CalculateTangents(const uint32_t* indexes, uint32_t indexCount, Vertex* vertexes, uint32_t vertexCount)
{
	for (uint32_t i = 0; i < indexCount; i += 3)
	{
		Vertex& v0 = vertexes[indexes[i]];
		Vertex& v1 = vertexes[indexes[i + 1]];
		Vertex& v2 = vertexes[indexes[i + 2]];

		vec3 edge1 = v1.location - v0.location;
		vec3 edge2 = v2.location - v0.location;

		float deltaU1 = v1.tex.x - v0.tex.x;
		float deltaV1 = v1.tex.y - v0.tex.y;
		float deltaU2 = v2.tex.x - v0.tex.x;
		float deltaV2 = v2.tex.y - v0.tex.y;

		float f = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);

		vec3 tangent; //  , bitangent; // not needed
		tangent.x = f * (deltaV2 * edge1.x - deltaV1 * edge2.x);
		tangent.y = f * (deltaV2 * edge1.y - deltaV1 * edge2.y);
		tangent.z = f * (deltaV2 * edge1.z - deltaV1 * edge2.z);

		v0.tangent += tangent;
		v1.tangent += tangent;
		v2.tangent += tangent;
	}

	for (uint32_t i = 0; i < vertexCount; i++)
	{
		vertexes[i].tangent = normalize(vertexes[i].tangent);
	}
}
