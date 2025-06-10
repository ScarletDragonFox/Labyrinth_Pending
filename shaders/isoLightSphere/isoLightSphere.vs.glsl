#version 450 core

layout (location = 0) in vec3 aPos;

layout(location = 0) out vec3 oColor;

#include "../includes/playerData.glsl"

#include "../includes/lightBuffer.glsl"

mat4 my_translate(mat4 m, vec3 v)
{
    mat4 Result = m;
    Result[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
	return Result;
}


mat4 my_scale(mat4 m, float v_scale)
{
	mat4 Result;
	Result[0] = m[0] * v_scale;
	Result[1] = m[1] * v_scale;
	Result[2] = m[2] * v_scale;
	Result[3] = m[3];
	return Result;
}

void main()
{
    const SingularPointLightStructure light = b_PointLights[b_PointLightsAlive[gl_InstanceID]];

    //mat4 model = mat4(1.0);
    //model = my_translate(model, light.position);
    //model = my_scale(model, light.radius * 1.95);
    //vec4 temp = model * vec4(aPos, 1.0);
    vec4 temp = my_scale(my_translate(mat4(1.0), light.position), light.radius * 1.95) * vec4(aPos, 1.0);

    oColor = light.color;
    gl_Position = b_player.mProjection * b_player.mView * temp;
}