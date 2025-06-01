#version 450 core

layout (location = 3) uniform vec3 u_position;

layout(location = 0) out vec2 oTexCoords;

#include "../includes/playerData.glsl"


vec3 positions[6] = {{-0.5,  0.5,  0.0}, {0.5,   0.5,  0.0}, {-0.5,  -0.5, 0.0}, {-0.5,  -0.5, 0.0}, {0.5,   0.5,  0.0}, {0.5,   -0.5, 0.0}};
vec2 TextureCoordinates[6] = {{0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}, {0.0, 1.0}, {1.0, 0.0}, {1.0, 1.0}};


mat4 translatee(mat4 m, vec3 v)
{
    mat4 Result = m;
    Result[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
	return Result;
}

void main()
{
    mat4 transform =  b_player.mView * translatee(mat4(1.0), u_position);
    oTexCoords = TextureCoordinates[gl_VertexID];

    // reset rotation/scale of the matrix, keeping the translation
    transform[0][0] = 1.0; transform[0][1] = 0.0; transform[0][2] = 0.0;
    transform[1][0] = 0.0; transform[1][1] = 1.0; transform[1][2] = 0.0; 
    transform[2][0] = 0.0; transform[2][1] = 0.0; transform[2][2] = 1.0;
    gl_Position = b_player.mProjection * transform * vec4(positions[gl_VertexID], 1.0);
}
