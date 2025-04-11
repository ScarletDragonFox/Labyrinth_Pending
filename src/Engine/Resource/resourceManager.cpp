#include "Labyrinth/Engine/Resource/resourceManager.hpp"
#include "Labyrinth/Engine/Resource/shaderManager.hpp"

namespace lp
{
    bool ResourceManager::initialize()
    {
        this->mShader = std::make_unique<lp::res::ShaderManager>();
        this->mShader->initialize();
    }

    void ResourceManager::destroy()
    {
        this->mShader->destroy();
    }
}
