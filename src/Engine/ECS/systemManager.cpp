#include "Labyrinth/Engine/ECS/systemManager.hpp"

namespace lp
{
    void SystemManager::destroyEntity(const Entity cv_entity)
    {
        for(auto& i: mSystems)
        {
            i.second->mEntities.erase(cv_entity);
            i.second->mDirty = true;
        }
    } 

    void SystemManager::entitySignatureChanged(const Entity cv_entity, const Signature cv_signature)
    {
        for(auto& i: this->mSystems)
        {
            const Signature& sign = this->mSystemSignatures[i.first];

            if((cv_signature & sign) == sign)
            {
                i.second->mEntities.insert(cv_entity);
            }else
            {
                i.second->mEntities.erase(cv_entity);
            }
            i.second->mDirty = true;
        }
    }
}

