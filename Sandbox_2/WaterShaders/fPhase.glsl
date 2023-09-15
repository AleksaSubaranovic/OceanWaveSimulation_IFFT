#version 460 core
precision highp float;

#define PI 3.14159265359
#define g 9.81f
#define KM 370.0f

in vec2 TexCoord;
layout (binding = 0) uniform sampler2D phaseTex;

uniform float delta; // delta time
uniform float size;
uniform float resolution; // 512

out vec4 FragColor;

float omega(float k);
// Ping Pong Texture, Use Ping and delta time calculate y parametar around circle
void main()
{
	vec2 coord = gl_FragCoord.xy - 0.5;

	float n = (coord.x < resolution * 0.5) ? coord.x : coord.x - resolution;
    float m = (coord.y < resolution * 0.5) ? coord.y : coord.y - resolution;

	vec2 waveV =  (2.0 * PI * vec2(n, m)) / size;
	
	float phase = texture2D(phaseTex, TexCoord).r;
	float deltaPhase = omega(length(waveV)) * delta;
	phase = mod(phase + deltaPhase, 2.0 * PI);

	FragColor = vec4(phase, 0.0,0.0,0.0);
}

float omega(float k)
{
	return sqrt(g * k * (1.0 + k * k / KM * KM));
}