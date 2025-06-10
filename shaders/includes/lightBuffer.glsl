struct SingularPointLightStructure
{
    vec3 position;
    float radius;
    vec3 color;
    /// @brief shadow id -1. 0 means no shadow
    int shadowID;
};


layout (std140,binding = 0) buffer SSBO_PointLights
{
    SingularPointLightStructure b_PointLights[];
};

layout (std430,binding = 1) buffer SSBO_PointLightsAlive
{
    uint b_PointLightsAlive[];
};

// layout (location = 25) uniform float u_AO;
// layout (binding = 5) uniform samplerCube irradianceMap;//5
// layout (binding = 6) uniform samplerCube prefilterMap;//6
// layout (binding = 7) uniform sampler2D brdfLUT;//7