#version 450 core

layout(early_fragment_tests) in;

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 in_TexCoords;


layout (binding = 0) uniform sampler2D  tex_diffuse;


void main()
{
    vec3 col = texture(tex_diffuse, in_TexCoords).rgb;
    FragColor = vec4(col, 1.0);
}

