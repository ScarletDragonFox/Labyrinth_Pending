#include "Labyrinth/Engine/Resource/resourceManager.hpp"
#include "Labyrinth/Engine/Resource/shaderManager.hpp"

#include <iostream>

namespace lp
{
    bool ResourceManager::initialize()
    {
        this->mShader = std::make_unique<lp::res::ShaderManager>();
        if(this->mShader->initialize())
        {
            std::cerr << "Shader Manager failed to initialize!\n";
            return true;
        }
        return false;
    }

    void ResourceManager::destroy()
    {
        this->mShader->destroy();
    }
}
