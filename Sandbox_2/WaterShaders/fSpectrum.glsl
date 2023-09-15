#version 460 core
precision highp float;

#define PI 3.14159265359f
#define g 9.81f
#define KM 370.0f

uniform float size;
uniform float resolution; // 512
uniform float waveFactor; // wawe chopiness

in vec2 TexCoord;

layout(binding = 0) uniform sampler2D phaseTex;
layout(binding = 1) uniform sampler2D initSpecTex;

out vec4 FragColor;

float omega(float k);

// POWER OF 2
#define POW2(number) (number * number)

vec2 c_mult(vec2 a, vec2 b); // complex multiplication
vec2 c_mult_I(vec2 a); // complex multiplication with I

// prep for fft, store complex values, 
void main()
{
	vec2 coord = gl_FragCoord.xy - 0.5;

	float n = (coord.x < resolution * 0.5) ? coord.x : coord.x - resolution;
	float m = (coord.y < resolution * 0.5) ? coord.y : coord.y - resolution;

	vec2 waveV = (2.0 * PI * vec2(n, m)) / size;
	
	float phase = texture2D(phaseTex, TexCoord).r;
    vec2 phaseV = vec2(cos(phase), sin(phase));

	// no need for h0(k,t), h0 is updated with phase(time) (ping pong idea)
	vec2 h0 = vec2(texture2D(initSpecTex, TexCoord).r, 0.0f); // h0(k)
    vec2 h0Star = vec2(texture2D(initSpecTex, vec2(1.0 - TexCoord + 1.0 / resolution)).r, 0.0f); //h(-k)
    //h0Star.y *= -1.0;   this implementation sets a + ib, to a + i * 0

	vec2 h = c_mult(h0, phaseV) + c_mult(h0Star, vec2(phaseV.x, -phaseV.y));
	vec2 hX = -c_mult_I(h * (waveV.x / length(waveV))) * waveFactor;
    vec2 hZ = -c_mult_I(h * (waveV.y / length(waveV))) * waveFactor;

	if (waveV.x == 0.0 && waveV.y == 0.0)
    {
        h = vec2(0.f);
        hX = vec2(0.f);
        hZ = vec2(0.f);
    }

	FragColor = vec4(hX + c_mult_I(h), hZ);
}

float omega(float k)
{
	return sqrt(g * k * (1.0 + k * k / KM * KM));
}

vec2 c_mult(vec2 a, vec2 b)
{
	return vec2(a[0] * b[0] - a[1] * b[1], a[1] * b[0] + a[0] * b[1]);
}

vec2 c_mult_I(vec2 a)
{
	return vec2(-a[1], a[0]);
}