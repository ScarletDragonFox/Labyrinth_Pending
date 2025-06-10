#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTextureCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;

layout (location = 3) uniform mat4 u_model;

layout (location = 0) out vec2 out_TexCoords;
layout (location = 1) out vec3 out_Normals;
layout (location = 2) out vec3 out_FragPos;

#include "../includes/playerData.glsl"

void main()
{
    vec4 temp = u_model * vec4(aPos, 1.0);
    gl_Position = b_player.mProjection * b_player.mView * temp;

    out_TexCoords = aTextureCoords;
    out_Normals = aNormal;
    out_FragPos = temp.xyz;
}

