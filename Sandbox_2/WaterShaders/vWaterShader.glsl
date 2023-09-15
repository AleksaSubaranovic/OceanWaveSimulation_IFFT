#version 460 core

precision highp float;

layout (location = 0) in vec3 position;
// layout (location = 1) in vec3 normal; normal is in texture
layout (location = 1) in vec2 texCoord;

out vec2 TexCoord;
out vec3 FragPos;

// uniform mat4 model; no need for model, always mat4(1.f)
uniform mat4 view;
uniform mat4 projection;

uniform float size; // 250
uniform float geomSize; // 2000

layout(binding = 0) uniform sampler2D displacementTex;

void main()
{
	FragPos = position + texture2D(displacementTex, texCoord).rgb *  (geomSize / size);
	TexCoord = texCoord;
	gl_Position = projection * view * vec4(FragPos, 1.0f);

	// Normal = mat3(transpose(inverse(model))) * normal; 
}