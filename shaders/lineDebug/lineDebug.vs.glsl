#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;

layout (location = 1) uniform mat4 u_camView;
layout (location = 2) uniform mat4 u_camProjection;

layout (location = 3) uniform mat4 u_model;

layout(location = 0) out vec3 oColor;

void main()
{
    vec4 temp = u_model * vec4(aPos, 1.0);
    oColor = aCol;
    gl_Position = u_camProjection * u_camView * temp;
}

