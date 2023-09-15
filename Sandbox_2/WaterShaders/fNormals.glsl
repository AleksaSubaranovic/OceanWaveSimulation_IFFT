#version 460 core
precision highp float;

uniform float resolution;
uniform float size;

in vec2 TexCoord;
layout (binding = 0) uniform sampler2D displacementTex;

out vec4 FragColor;

void main()
{
	float texel = 1.0 / resolution;
    float texelSize = size / resolution;

	vec3 center = texture2D(displacementTex, TexCoord).rgb;
    vec3 right = vec3(texelSize, 0.0, 0.0) + texture2D(displacementTex, TexCoord + vec2(texel, 0.0)).rgb - center;
    vec3 left = vec3(-texelSize, 0.0, 0.0) + texture2D(displacementTex, TexCoord + vec2(-texel, 0.0)).rgb - center;
    vec3 top = vec3(0.0, 0.0, -texelSize) + texture2D(displacementTex, TexCoord + vec2(0.0, -texel)).rgb - center;
    vec3 bottom = vec3(0.0, 0.0, texelSize) + texture2D(displacementTex, TexCoord + vec2(0.0, texel)).rgb - center;

	vec3 topRight = cross(right, top);
    vec3 topLeft = cross(top, left);
    vec3 bottomLeft = cross(left, bottom);
    vec3 bottomRight = cross(bottom, right);

	FragColor =  vec4(normalize(topRight + topLeft + bottomLeft + bottomRight), 1.0);
}