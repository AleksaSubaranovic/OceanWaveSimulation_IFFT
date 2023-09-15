#include "Material.h"
#include "Shader.h"

Material::Material()
{

}

Material::Material(const std::string& diffuseTexPath, const std::string& specularTexPath)
{
	diffuseTex.load(diffuseTexPath);
	specularTex.load(specularTexPath);
}

//Material::Material(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const std::string& diffuseTexPath, const std::string& specularTexPath)
//	: ambient(ambient), diffuse(diffuse), specular(specular)
//{
//	diffuseTex.load(diffuseTexPath);
//	specularTex.load(specularTexPath);
//}

void Material::SendToShader(Shader& shader)
{
	shader.setFloat("material.shininess", shininess);
	
	GLint dLoc = glGetUniformLocation(shader.getShaderId(), "material.diffuseTex");
	GLint sLoc = glGetUniformLocation(shader.getShaderId(), "material.specularTex");
	GLint eLoc = glGetUniformLocation(shader.getShaderId(), "material.emissionTex");

	glUniform1i(dLoc, DIFFUSE_TEX_UNIT);
	glUniform1i(sLoc, SPECULAR_TEX_UNIT);
	glUniform1i(eLoc, EMISSION_TEX_UNIT);

	if (diffuseTex.isInitialized())
	{
		diffuseTex.Bind(DIFFUSE_TEX_UNIT);
	}

	if (specularTex.isInitialized())
	{
		specularTex.Bind(SPECULAR_TEX_UNIT);
	}

	if (emissionTex.isInitialized())
	{
		emissionTex.Bind(EMISSION_TEX_UNIT);
		shader.setBool("shouldUseEmission", true);
	}
	else shader.setBool("shouldUseEmission", false);
}
