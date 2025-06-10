#version 450 core

layout(early_fragment_tests) in;

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 in_TexCoords;
layout (location = 1) in vec3 in_Normals;
layout (location = 2) in vec3 in_FragPos;

layout (binding = 0) uniform sampler2D  tex_diffuse;
layout (binding = 1) uniform sampler2D  tex_specular;

layout (location = 4) uniform float u_model;

#include "../includes/lightBuffer.glsl"
#include "../includes/playerData.glsl"

// calculates the color when using a point light.
vec3 CalcPointLight(vec3 lightColour, vec3 lightPosition, float lightRadius, float  matShininess, vec3 matColor, vec3 matSpecular, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(lightPosition - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matShininess);
    // attenuation

    float attenuation = max(0.95 - length(fragPos - lightPosition.xyz) / lightRadius, 0.0);

    // combine results
    //vec3 ambient = lightColour * matColor * 0.0001;
    vec3 diffuse = lightColour * diff * matColor;
    vec3 specular = lightColour * spec * matSpecular;
    //ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (diffuse + specular);
}


void main()
{
    vec3 colDiffuse = texture(tex_diffuse, in_TexCoords).rgb;
    vec3 colSpecular = texture(tex_specular, in_TexCoords).rgb;
    vec3 res = colDiffuse * 0.01;

    const vec3 viewDir = normalize(b_player.mPosition - in_FragPos);

    for(uint i = 0; i < b_PointLightsAlive.length(); ++i) //without the '()' here the glsl compiler in the DRIVER crashes our program.
    {
        const SingularPointLightStructure light = b_PointLights[b_PointLightsAlive[i]];
        res += CalcPointLight(light.color, light.position, light.radius, 16.0, colDiffuse, colSpecular, in_Normals, in_FragPos, viewDir);
    }
    FragColor = vec4(res, 1.0);
}