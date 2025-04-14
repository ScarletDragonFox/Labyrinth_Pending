#version 450 core

layout(early_fragment_tests) in;

layout(location = 0) out vec4 FragColor;

layout (location = 4) uniform vec3 u_colour;


void main()
{
    FragColor = vec4(u_colour, 1.0);
}

