#version 460 core

precision highp float;

#define PI 3.14159265359f
#define g 9.81f

// minimum phase speed at the wavenumber CM
// KM is wavenumber for continuity between low and high wavenumber regimes (Omega function)
#define KM 370.0f
#define CM 0.23f
#define OMEGA 0.84f

uniform vec2 wind; // 
uniform float size;
uniform float resolution;

out vec4 FragColor;
	
float omega(float k);
float square(float f);

in vec2 TexCoord;

// A unified directional spectrum for long and short wind-driven waves

void main()
{
	vec2 coord = gl_FragCoord.xy - 0.5;
    float n = (coord.x < resolution * 0.5) ? coord.x : coord.x - resolution;
    float m = (coord.y < resolution * 0.5) ? coord.y : coord.y - resolution;
    vec2 waveV = (2.0 * PI * vec2(n, m)) / size;

    float k = length(waveV);
	float lenWind = length(wind); // U10, U 10 is the wind speed at a height of 10 m above the water surface

	float kp = g * square(OMEGA / lenWind); // wave mumber of the spectral peak

	float c = omega(k) / k; // coef
	float cp = omega(kp) / kp; // coef, dominant long wave

	//
	//  omnidirectional spectrum expressed as a sum of two spectra regimes
	//  Bl, Bh

	// Low-Frequency Spectrum

	float Lpm = exp(-1.25 * square(kp / k)); // Pierson-Moskowitz shape spectrum
	float gamma = 1.7;
	float sigma = 0.08 * (1.0 + 4.0 * pow(OMEGA, -3.0));
	float Gamma = exp(-square(sqrt(k / kp) - 1.0) / 2.0 * square(sigma));
	float Jp = pow(gamma, Gamma); // peak enhancèment or "overshoot" factor
	float Fp = Lpm * Jp * exp(-OMEGA / sqrt(10.0) * (sqrt(k / kp) - 1.0)); // long-wave side effect function Fp
	float alphap = 0.006 * sqrt(OMEGA);
	float Bl = 0.5 * alphap * cp / c * Fp; // Long-wave curvature spectrum

	float z0 = 0.000037 * square(lenWind) / g * pow(lenWind / cp, 0.9);
	float uStar = 0.41 * lenWind / log(10.0 / z0);
	float alpham = 0.01 * ((uStar < CM) ? (1.0 + log(uStar / CM)) : (1.0 + 3.0 * log(uStar / CM)));
	float Fm = exp(-0.25 * square(k / KM - 1.0)); // short-wave side effect function 
	float Bh = 0.5 * alpham * CM / c * Fm * Lpm; // short wave curvature spectrum

	// Unified Spreading Function

	float a0 = log(2.0) / 4.0;
	float am = 0.13 * uStar / CM;
	float Delta = tanh(a0 + 4.0 * pow(c / cp, 2.5) + am * pow(CM / c, 2.5));

	float cosPhi = dot(normalize(wind), normalize(waveV));
	
	float S = (1.0 / (2 * PI)) * pow(k, -4.0) * (Bl + Bh) * (1.0 + Delta * (2.0 * cosPhi * cosPhi - 1.0));

	float dk = 2.0 * PI / size;
	float h = sqrt(S / 2.0) * dk;

	if (waveV.x == 0. && waveV.y == 0.0) h = 0.0;

	FragColor = vec4(h, 0.0, 0.0, 0.0);
}

float omega(float k)
{
	return sqrt(g * k * (1.0 + square(k / KM)));
}

float square(float f)
{
	return f * f;
}