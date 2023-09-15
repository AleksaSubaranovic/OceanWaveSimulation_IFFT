#version 460 core

precision highp float;

// Directional Light only
struct DirectionalLight {
    vec3 direction;
	
    vec3 ambient; // not used
    vec3 diffuse;
    vec3 specular; // not used
};
uniform DirectionalLight dirLight;

uniform vec3 cameraLocation;
//vec3 skyColor = vec3(3.2, 9.6, 12.8); // can be texture, cubemap skybox, --- easy to implement as color
vec3 waterColor = vec3(0.004, 0.016, 0.047); // needs to be small value vector

layout(binding = 1) uniform sampler2D normalTex;

// in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

out vec4 FragColor;

vec3 getNorm();
vec3 calcReflectColor(vec3 norm, vec3 viewDir, vec3 skyColor);

void main()
{
    vec3 norm = getNorm();
    vec3 lightDir = normalize(-dirLight.direction);// from Light.cpp for DirectionalLight
    vec3 viewDir = normalize(cameraLocation - FragPos);

    vec3 skyColor = dirLight.diffuse;

    // if (dot(norm, viewDir) < 0) norm = -norm;

    // fresnel
    float fresnel = 0.02f + 0.98f * pow(1.f - dot(norm, viewDir), 5.0);
    float exposure = 0.35f;
    vec3 sky = fresnel * skyColor;

    float diff = clamp(dot(norm, lightDir), 0.0, 1.0); // just diffuse (skyColor * diffuse)
    vec3 water = (1.0 - fresnel) * waterColor * skyColor * diff;

    vec3 result = sky + water; // + calcReflectColor(norm, viewDir, sky); (fresnel handles reflection factor)
    //result = ambient + diffuse + specular + heightColor + refColor;

    // hdr ( range of light and dark tones )
    result = 1.0 - exp(-result * exposure);

    FragColor = vec4(result, 1.0f);
}

vec3 getNorm() // no need for function right now
{
    return texture2D(normalTex, TexCoord).rgb;
    // return normalize(Normal);
}

/*
vec3 calcAmbient()
{
    return dirLight.ambient * ambientF;
}

vec3 calcHeightColor()
{
    // consts 
    shallowColor = vec3(0.0, 0.64, 0.68);
	deepColor = vec3(0.02, 0.05, 0.10);
    // height calc
    height = (FragPos.y - hMin) / (hMax - hMin); // HMAX AND HMIN CAN'T BE EQUAL!!!

    return height * shallowColor + (1 - height) * deepColor;
}

vec3 calcDiffuse()
{
    diff = max(dot(norm, lightDir), 0.0);
    return diffuseF * dirLight.diffuse * diff;
}

vec3 calcSpecular()
{
    spec =  pow(max(dot(viewDir, reflect(-lightDir, norm)), 0.0), 64) * 3; // over expousere
    return dirLight.specular * spec;
}
*/


vec3 calcReflectColor(vec3 norm, vec3 viewDir, vec3 skyColor)
{
    // Psudo reflect
	float refCoeff = pow(max(dot(norm, viewDir), 0.0), 0.3); // Smaller power will have more concentrated reflect.
	return (1 - refCoeff) * skyColor;
}