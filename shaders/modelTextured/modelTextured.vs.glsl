#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTextureCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;

layout (location = 1) uniform mat4 u_camView;
layout (location = 2) uniform mat4 u_camProjection;
layout (location = 3) uniform mat4 u_model;

layout (location = 0) out vec2 in_TexCoords;

void main()
{
    vec4 temp = u_model * vec4(aPos, 1.0);
    gl_Position = u_camProjection * u_camView * temp;

    in_TexCoords = aTextureCoords;
}

