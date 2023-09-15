#version 460 core

precision highp float;

#define PI 3.14159265359

uniform float tSize; // transform size
uniform float stSize; // subtransform size
uniform bool isVertical;

uniform sampler2D inputTex;

vec2 c_mult(vec2 a, vec2 b);
in vec2 TexCoord;

out vec4 FragColor;

// IFFT algorithm (storing x,y,z offsets)
void main()
{
	float index;
	if (isVertical)
	{
		index = TexCoord.y * tSize - 0.5;
	}
	else 
	{
		index = TexCoord.x * tSize - 0.5;
	}

	float evenIndex = floor(index / stSize) * (stSize * 0.5) + mod(index, stSize * 0.5);

	vec4 even, odd;
	if (isVertical)
	{
		even = texture2D(inputTex, vec2(gl_FragCoord.x, evenIndex + 0.5) / tSize).rgba;
		odd = texture2D(inputTex, vec2(gl_FragCoord.x, evenIndex + tSize * 0.5 + 0.5) / tSize).rgba;
	}
	else
	{
		even = texture2D(inputTex, vec2(evenIndex + 0.5, gl_FragCoord.y) / tSize).rgba;
		odd = texture2D(inputTex, vec2(evenIndex + tSize * 0.5 + 0.5, gl_FragCoord.y) / tSize).rgba;
	}

	float angle = -2.0 * PI * (index / stSize);
	vec2 twiddle = vec2(cos(angle), sin(angle));

	vec2 output1 = even.xy + c_mult(twiddle, odd.xy);
	vec2 output2 = even.zw + c_mult(twiddle, odd.zw);

	FragColor = vec4(output1, output2);
}

vec2 c_mult(vec2 a, vec2 b)
{
	return vec2(a[0] * b[0] - a[1] * b[1], a[1] * b[0] + a[0] * b[1]);
}
