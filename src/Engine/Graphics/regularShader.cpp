#include "Labyrinth/Engine/Graphics/regularShader.hpp"
#include "Labyrinth/engine.hpp"
#include "Labyrinth/Engine/Resource/shaderManager.hpp"

namespace lp::gl
{
    void RegularShader::LoadShader(const ShaderType ST)
    {
        this->mProgramID = g_engine.getResurceManager().getShaderManager().getShaderID(ST);
    }
}
