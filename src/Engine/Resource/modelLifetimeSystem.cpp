#include "Labyrinth/Engine/Resource/modelLifetimeSystem.hpp"

#include "Labyrinth/engine.hpp"
#include "Labyrinth/Engine/ECS/coreECS.hpp"

namespace lp::res
{
    void SystemModelLifetime::modelLoaded(const lp::res::ModelID_t cv_id, std::shared_ptr<lp::res::LoadedModel> v_ptr)
    {
        auto& ecs = lp::g_engine.getECS();
        
        for(const auto& mdl:this->mEntities)
        {
            auto& comp = ecs.getComponent<lp::ecs::ComponentModel>(mdl);
            if(comp.mID == cv_id)
            {
                comp.mModel = v_ptr;
            }
        }
        //this->mDirty = false; no need to reset dirty flag
    }

    void SystemModelLifetime::modelUnloaded(const lp::res::ModelID_t cv_id)
    {
        auto& ecs = lp::g_engine.getECS();
        for(const auto& mdl:this->mEntities)
        {
            auto& comp = ecs.getComponent<lp::ecs::ComponentModel>(mdl);
            if(comp.mID == cv_id)
            {
                comp.mModel = nullptr;
            }
        }
    }
}