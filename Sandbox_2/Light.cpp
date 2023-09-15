#include "Light.h"
#include "Material.h"
#include <string>
#include "Generic.h"
#include <iostream>
#include <glm/glm.hpp>

const char* LightShader::vShaderSource = R"glsl(
#version 460 core
layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoord;
layout (location = 1) in vec3 normal;
layout (location = 3) in vec3 tangent;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out vec3 Tangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    mat4 mvp = projection * view * model;
    FragPos = vec3(model * vec4(position, 1.0));
    Normal = mat3(transpose(inverse(model))) * normal;
    
    gl_Position = mvp * vec4(position, 1.0);
    TexCoord = texCoord;
    Tangent = (model * vec4(tangent, 0.0)).xyz;
}
)glsl";


const char* LightShader::fShaderSource = R"glsl(
#version 460 core
struct Material
{
    float shininess;

    sampler2D diffuseTex;
    sampler2D specularTex;
    sampler2D emissionTex;
};
uniform Material material;

struct DirectionalLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct PointLight {
    vec3 location;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct SpotLight {
    vec3 location;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 4

uniform DirectionalLight dirLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform int nPointLights;
uniform int nSpotLights;
uniform bool shouldUseEmission;

in vec3 FragPos;  
in vec3 Normal;
in vec2 TexCoord;
in vec3 Tangent;

out vec4 FragColor;

uniform vec3 cameraLocation;
uniform sampler2D normalTex;
uniform bool shouldUseNormalTex;

vec3 norm;
vec3 viewDir;

vec3 CalcDirLight(DirectionalLight light);
vec3 CalcPointLight(PointLight light);
vec3 CalcSpotLight(SpotLight light);
vec3 CalcBumpedNormal();

void main()
{
    norm = CalcBumpedNormal();
    vec3 viewDir = normalize(cameraLocation - FragPos);
    
    vec3 res = CalcDirLight(dirLight);
    
    for (int i = 0; i < nPointLights; i++)
        res += CalcPointLight(pointLights[i]);

    for (int i = 0; i < nSpotLights; i++)
        res += CalcSpotLight(spotLights[i]);

    if (shouldUseEmission) res += texture(material.emissionTex, TexCoord).rgb;
    FragColor = vec4(res, 1.0);
}

vec3 ambient,diffuse,specular, emission;
float diff,spec,dist,att, theta, epsilon, intensity; // att = attenuation
vec3 lightDir;

vec3 CalcDirLight(DirectionalLight light)
{
    lightDir = normalize(-light.direction);
    diff = max(dot(norm, lightDir), 0.0);
    // reflectDir = reflect(-lightDir, norm);
    spec = pow(max(dot(viewDir, reflect(-lightDir, norm)), 0.0), material.shininess);
        
    ambient = light.ambient * vec3(texture(material.diffuseTex, TexCoord));
    diffuse = light.diffuse * diff * vec3(texture(material.diffuseTex, TexCoord));
    specular = light.specular * spec * vec3(texture(material.specularTex, TexCoord));
    //emission = texture(material.emissionTex, TexCoord).rgb;

    return ambient + diffuse + specular;// + emission;
}

vec3 CalcPointLight(PointLight light)
{
    lightDir = normalize(light.location - FragPos);
    diff = max(dot(norm, lightDir), 0.0);
    spec = pow(max(dot(viewDir, reflect(-lightDir, norm)), 0.0), material.shininess);
    dist = length(light.location - FragPos);
    att = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));  

    ambient = light.ambient * vec3(texture(material.diffuseTex, TexCoord));
    diffuse = light.diffuse * diff * vec3(texture(material.diffuseTex, TexCoord));
    specular = light.specular * spec * vec3(texture(material.specularTex, TexCoord));
    //emission = texture(material.emissionTex, TexCoord).rgb;

    ambient *= att;
    diffuse *= att;
    specular *= att;
    //emission *= att; // || TODO:
    return ambient + diffuse + specular;// + emission;
}

vec3 CalcSpotLight(SpotLight light)
{
    lightDir = normalize(light.location - FragPos);
    diff = max(dot(norm, lightDir), 0.0);
    spec = pow(max(dot(viewDir, reflect(-lightDir, norm)), 0.0), material.shininess);
    dist = length(light.location - FragPos);
    
    att = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
    theta = dot(lightDir, normalize(-light.direction)); 
    epsilon = light.cutOff - light.outerCutOff;
    intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    ambient = light.ambient * vec3(texture(material.diffuseTex, TexCoord));
    diffuse = light.diffuse * diff * vec3(texture(material.diffuseTex, TexCoord));
    specular = light.specular * spec * vec3(texture(material.specularTex, TexCoord));
    //emission = texture(material.emissionTex, TexCoord).rgb;

    ambient *= att * intensity;
    diffuse *= att * intensity;
    specular *= att * intensity;
    //emission *= att * intensity; //|| TODO:
    return ambient + diffuse + specular;// + emission;
}

vec3 CalcBumpedNormal()
{
    if (shouldUseNormalTex)
    {
        vec3 norm0 = normalize(Normal);
        vec3 Tangent0 = normalize(Tangent);
        Tangent0 = normalize(Tangent0 - dot(Tangent0, norm0) * norm0);
        vec3 Bitangent = cross(Tangent0, norm0);
        vec3 BumpMapNormal = texture(normalTex, TexCoord).xyz;
        BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
        
        mat3 TBN = mat3(Tangent0, Bitangent, norm0);
        vec3 NewNormal = TBN * BumpMapNormal;
        NewNormal = normalize(NewNormal);
        return NewNormal;
    }
    else return normalize(Normal);
}
)glsl";

LightShader::LightShader()
{
    CompileShader(vShaderSource, fShaderSource);
}

void LightShader::SetLightsArrayLength(int numOfPointLights, int numOfSpotLights)
{
    setInt("nPointLights", numOfPointLights);
    setInt("nSpotLights", numOfSpotLights);
}


// macro for easy stringing to shader
#define L_(value) "[" + value
// macro for easy stringing to shader
#define _L(value) "]." #value

void BaseLight::SendToShader(Shader* shader, int ind, const std::string& type, std::string& s)
{
    s = std::to_string(ind);

    shader->setVec3(type + L_(s) + _L(ambient), ambient);
    shader->setVec3(type + L_(s) + _L(diffuse), diffuse);
    shader->setVec3(type + L_(s) + _L(specular), specular);
}

BaseLight::BaseLight()
{
    ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    diffuse = glm::vec3(1.f, 1.f, 1.f);
    specular = glm::vec3(0.5f, 0.5f, 0.5f);
}

void BaseLight::Intensify(float intensity)
{
    ambient *= intensity;
    diffuse *= intensity;
    specular *= intensity;
}

void DirectionalLight::SendToShader(Shader* shader, int ind, const std::string& type, std::string& s)
{
    // BaseLight::SendToShader(shader, ind, type, s);

    shader->setVec3(type + ".ambient", ambient);
    shader->setVec3(type + ".diffuse", diffuse);
    shader->setVec3(type + ".specular", specular);
    shader->setVec3(type + ".direction", direction);
}

DirectionalLight::DirectionalLight() : BaseLight()
{
    //direction = glm::vec3(-0.2f, -1.0f, -0.3f);
}

void PointLight::SendToShader(Shader* shader, int ind, const std::string& type, std::string& s)
{
    BaseLight::SendToShader(shader, ind, type, s);

    shader->setVec3(type + L_(s) + _L(location), location);

    shader->setFloat(type + L_(s) + _L(constant), constant);
    shader->setFloat(type + L_(s) + _L(linear), linear);
    shader->setFloat(type + L_(s) + _L(quadratic), quadratic);
}

PointLight::PointLight() : BaseLight()
{
    constant = 1.f;
    linear = 0.09f;
    quadratic = 0.032f;
}

void SpotLight::SendToShader(Shader* shader, int ind, const std::string& type, std::string& s)
{
    BaseLight::SendToShader(shader, ind, type, s);

    shader->setVec3(type + L_(s) + _L(location), location);
    shader->setVec3(type + L_(s) + _L(direction), direction);

    shader->setFloat(type + L_(s) + _L(cutOff), cutOff);
    shader->setFloat(type + L_(s) + _L(outerCutOff), outerCutOff);

    shader->setFloat(type + L_(s) + _L(constant), constant);
    shader->setFloat(type + L_(s) + _L(linear), linear);
    shader->setFloat(type + L_(s) + _L(quadratic), quadratic);
}

SpotLight::SpotLight() : BaseLight()
{
    constant = 1.f;
    linear = 0.09f;
    quadratic = 0.032f;

    cutOff = glm::cos(glm::radians(12.5f));
    outerCutOff = glm::cos(glm::radians(17.5f));
}
