
struct RendererForwardPlus_PlayerData
    {
        mat4 mView;
        mat4 mProjection; //infinite projection
        vec3 mPosition;
        float padding;
    };

layout (std140,binding = 0) uniform UBO_Matricies{
    RendererForwardPlus_PlayerData b_player;
};