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
        
        this->mModel.initialize();

        return false;
    }

    void ResourceManager::destroy()
    {
        this->mShader->destroy();
    }

    lp::res::ModelID_t ResourceManager::loadModel(const std::string_view cv_path)
    {
        return this->mModel.scheduleLoad(cv_path);
    }

    lp::res::LoadedModel const* ResourceManager::getLoadedModel(const lp::res::ModelID_t cv_id)const
    {
        if(!isValidModel(cv_id)) return nullptr;
        return mModel.getLoadedModel(cv_id);
    }

    bool ResourceManager::isValidModel(const lp::res::ModelID_t cv_id) const
    {
       return mModel.isValid(cv_id);
    }
}
