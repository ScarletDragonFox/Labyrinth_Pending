#include "Labyrinth/Engine/Resource/resourceManager.hpp"
#include "Labyrinth/Engine/Resource/shaderManager.hpp"

#include <iostream>
#include <algorithm> //std::find_if

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

    lp::res::ModelID_t ResourceManager::getModel(const std::string_view cv_path, const bool cv_DoNotLoad)
    {
        auto it = std::find_if(mModelIDtoModelMap.begin(), mModelIDtoModelMap.end(), [&cv_path](auto&& p){
            return cv_path == p.second;
        });
        if(it == mModelIDtoModelMap.end())
        {
            if(cv_DoNotLoad){
                return lp::res::const_id_model_invalid;
            } else{
                ++mLastModelID;
                mModelIDtoModelMap[mLastModelID] = cv_path;
                this->mModel.scheduleLoad(cv_path);
                return mLastModelID;
            }
        } else
        {
            return it->first;
        }
        return lp::res::const_id_model_invalid;
    }

    lp::res::LoadedModel const* ResourceManager::getLoadedModel(const lp::res::ModelID_t cv_id)const
    {
        if(!isValidModel(cv_id)) return nullptr;
        return mModel.getLoadedModel(this->mModelIDtoModelMap.at(cv_id));
    }

    bool ResourceManager::isValidModel(const lp::res::ModelID_t cv_id) const
    {
        if(cv_id == lp::res::const_id_model_invalid) return false;
        if(this->mModelIDtoModelMap.contains(cv_id)) return true;
        return false;
    }
}
