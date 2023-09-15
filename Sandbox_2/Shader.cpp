#include "Shader.h"
#include <fstream>
#include <sstream>
#include "Generic.h"

void Shader::CompileShader(const char* vertexShader, const char* fragmentShader)
{
	GLuint vShader, fShader;
	int success;
	char infoLog[512];

	vShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vShader, 1, &vertexShader, NULL);
	glCompileShader(vShader);

	// check for errors
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vShader, 512, NULL, infoLog);
		PRINT_ERROR("ERROR::SHADER::VERTEX::COMPILATION_FAILED");
		PRINT_ERROR(infoLog);
	}

	fShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fShader, 1, &fragmentShader, NULL);
	glCompileShader(fShader);

	glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fShader, 512, NULL, infoLog);
		PRINT_ERROR("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED");
		PRINT_ERROR(infoLog);
	}

	// shader program
	ID = glCreateProgram();
	glAttachShader(ID, vShader);
	glAttachShader(ID, fShader);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		PRINT_ERROR("ERROR::SHADER::PROGRAM::COMPILATION_FAILED");
		PRINT_ERROR(infoLog);
	}

	// no need for vertex and fragment shaders, they will be deleted when detached
	// now only marked for delete
	glDeleteShader(vShader);
	glDeleteShader(fShader);
}

void Shader::CompileComputeShader(const char* _shader)
{
	GLuint shader;
	int success;
	char infoLog[512];

	shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(shader, 1, &_shader, NULL);
	glCompileShader(shader);

	// check for errors
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		PRINT_ERROR("ERROR::SHADER::COMPUTE::COMPILATION_FAILED");
		PRINT_ERROR(infoLog);
	}

	// shader program
	ID = glCreateProgram();
	glAttachShader(ID, shader);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		PRINT_ERROR("ERROR::SHADER::PROGRAM::COMPILATION_FAILED");
		PRINT_ERROR(infoLog);
	}
}

Shader::Shader()
{
	ID = -1;
}

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	std::string vertexShaderSource, fragmentShaderSource;
	vertexShaderSource = GetSingleShaderFromFile(vertexPath);
	fragmentShaderSource = GetSingleShaderFromFile(fragmentPath);

	CompileShader(vertexShaderSource.c_str(), fragmentShaderSource.c_str());
}

void Shader::activate()
{
	glUseProgram(ID);
}

void Shader::recompile(const char* vertexShader, const char* fragmentShader)
{
	CompileShader(vertexShader, fragmentShader);
}

void Shader::setBool(const std::string& name, bool value)
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value)
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setColor(const std::string& name, float r, float g, float b)
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), r, g, b, 1.0);
}

void Shader::setMatrix4f(const std::string& name, const glm::mat4& mat)
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setVec3(const std::string& name, glm::vec3 v)
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(v));
}

void Shader::setVec2(const std::string& name, glm::vec2 v)
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(v));
}

std::string Shader::GetSingleShaderFromFile(const char* path)
{
	std::string shaderFromFile;
	std::ifstream shaderFile;

	// ensure ifstream objects can throw exceptions:
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		shaderFile.open(path);
		std::stringstream shaderStream;

		shaderStream << shaderFile.rdbuf();
		shaderFile.close();

		shaderFromFile = shaderStream.str();
	}
	catch (std::exception e)
	{
		PRINT_ERROR("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ");
		PRINT_ERROR(e.what());
	}

	return shaderFromFile;
}

Shader::~Shader()
{
	if (shouldDeleteProgram) glDeleteProgram(ID);
}

const char* Shader::vBasicShaderSource = R"glsl(
#version 330 core

in vec3 position;

void main()
{
    gl_Position = vec4(position, 1.0);
}
)glsl";

const char* Shader::fColorShaderSource = R"glsl(
#version 330 core
out vec4 FragColor;

uniform vec4 color;

void main()
{
	FragColor = color;
}
)glsl";

const char* Shader::vBasicTextureShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec2 position;
// layout (location = 1) in vec2 texCoord;

out vec2 TexCoord;

void main()
{
    gl_Position = vec4(position, 0.0, 1.0);
	TexCoord = position;
}
)glsl";

const char* Shader::fBasicTextureShaderSource = R"glsl(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D Texture;

void main()
{
    FragColor = texture(Texture, TexCoord);
}
)glsl";

const char* Shader::vStandardTextureShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0);
	TexCoord = texCoord;
} 
)glsl";

const char* Shader::vColorShaderSourceMVP = R"glsl(
#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	mat4 mvp = projection * view * model;
	gl_Position = mvp * vec4(position, 1.0);
}
)glsl";

const char* Shader::fColorShaderSourceMVP = R"glsl(
#version 330 core
out vec4 FragColor;

uniform vec4 color;

void main()
{
	FragColor = color;
}

)glsl";

// implement with function in Framework on init
Shader Shader::colorShader = Shader();
Shader Shader::textureShader = Shader();
Shader Shader::textureShaderMVP = Shader();
Shader Shader::colorShaderMVP = Shader();

void Shader::initBasicShaders()
{
	// color shader
	colorShader.CompileShader(Shader::vBasicShaderSource, Shader::fColorShaderSource);
	// texture shader
	textureShader.CompileShader(Shader::vBasicTextureShaderSource, Shader::fBasicTextureShaderSource);
	// texture shader with model view projection unfirom
	textureShaderMVP.CompileShader(Shader::vStandardTextureShaderSource, Shader::fBasicTextureShaderSource);

	colorShaderMVP.CompileShader(Shader::vColorShaderSourceMVP, Shader::fColorShaderSourceMVP);
}