#pragma once

#include <string>
#include "glew_glfw.h"
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>

// Shader Program
class Shader
{
protected:
	GLuint ID;

    // Compile Shader with shader sources, not shader file path
    void CompileShader(const char* vertexShader, const char* fragmentShader);
    
public:
    void CompileComputeShader(const char* _shader);
    Shader();
    Shader(const char* vertexPath, const char* fragmentPath);
    // activate the shader
    void activate();

    // use at own risk
    void recompile(const char* vertexShader, const char* fragmentShader);

    inline GLuint getShaderId() { return ID; }

    // utility uniform functions
    void setBool(const std::string& name, bool value);
    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setColor(const std::string& name, float r, float g, float b);
    void setMatrix4f(const std::string& name, const glm::mat4& mat);
    void setVec3(const std::string& name, glm::vec3 v);
    void setVec2(const std::string& name, glm::vec2 v);
    static std::string GetSingleShaderFromFile(const char* path);

    ~Shader();

protected:
    bool shouldDeleteProgram = true;

    static const char* vBasicShaderSource;
    static const char* fColorShaderSource;
    static const char* vBasicTextureShaderSource;
    static const char* fBasicTextureShaderSource;
    static const char* vStandardTextureShaderSource;

    static const char* vColorShaderSourceMVP;
    static const char* fColorShaderSourceMVP;

public:
    static Shader colorShader;
    static Shader textureShader;
    static Shader textureShaderMVP; // model view projection
    static Shader colorShaderMVP;
    static void initBasicShaders();
};

