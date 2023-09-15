#version 460

layout (location = 0) in vec2 position;
// layout (location = 1) in vec3 normal; normal is in texture

out vec2 TexCoord;

// simple input for quad
void main()
{
	TexCoord = position * 0.5 + 0.5;


    gl_Position = vec4(position, 0.0, 1.0);
}