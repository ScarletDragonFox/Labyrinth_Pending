#include "Labyrinth/Engine/Graphics/graphicsPrepareSystem.hpp"

#include "Labyrinth/engine.hpp"
#include <Labyrinth/Engine/ECS/coreECS.hpp>

#include <Labyrinth/Engine/ComponentPhysics.hpp>
#include <Labyrinth/Engine/ComponentPosition.hpp>

#include <Labyrinth/Engine/Graphics/LightSystem.hpp>

#include <vector>

namespace lp::gl
{
    GraphicsPrepareSystem::GraphicsPrepareSystem()
    {
        auto& ecsR = lp::g_engine.getECS();
        const lp::ecs::Signature signModel = ecsR.getComponentSignature<lp::ComponentModel>();
        const lp::ecs::Signature cph = ecsR.getComponentSignature<ComponentPhysics>() | signModel;
        const lp::ecs::Signature cpos = ecsR.getComponentSignature<ComponentPosition>() | signModel;
        this->mSystemPhysics = ecsR.registerSystem<GPS_Model_w_Physics>(cph);
        this->mSystemPositions = ecsR.registerSystem<GPS_Model_w_Position>(cpos);
    }

    void GraphicsPrepareSystem::process(ProcessedScene& output)
    {
        auto& ecsR = lp::g_engine.getECS();
        {
            const auto lightSys = ecsR.getSystem<lp::LightSystem>();
            output.mLightCount = lightSys->getLightCount();
            output.mSSB_AliveLights = lightSys->getAliveLightListBuffer();
            output.mSSB_Lights = lightSys->getLightBuffer();
        }
        const auto& modelsPhysics = mSystemPhysics->getSet();
        const auto& modelsPositions = mSystemPositions->getSet();
        output.mModelMap.clear();

        for(const auto& ent: modelsPhysics)
        {
            auto& mdl = ecsR.getComponent<lp::ComponentModel>(ent);
            if(mdl.mModel != nullptr)
            {
                const auto& phy = ecsR.getComponent<lp::ComponentPhysics>(ent);
                glm::mat4 modelMat = glm::translate(glm::mat4(1.0), phy.getPosition());
                modelMat = modelMat * glm::mat4_cast(phy.getRotation());
                this->addModel(output, mdl.mID, mdl.mModel, modelMat);
            }
        }

        for(const auto& ent: modelsPositions)
        {
            auto& mdl = ecsR.getComponent<lp::ComponentModel>(ent);
            if(mdl.mModel != nullptr)
            {
                const auto& psoy = ecsR.getComponent<lp::ComponentPosition>(ent);
                this->addModel(output, mdl.mID, mdl.mModel, psoy.getModelMatrix());
            }
        }
    }

    void GraphicsPrepareSystem::addModel(ProcessedScene& output, const lp::res::ModelID_t id, std::shared_ptr<lp::res::LoadedModel>& ptr, const glm::mat4& matrix)
    {
        const auto iter = output.mModelMap.find(id);
        if(iter == output.mModelMap.end())
        {
            output.mModelMap[id] = ProcessedScene::ModelsHolder();
            output.mModelMap[id].mPtr = ptr;
        }
        output.mModelMap[id].mMatricies.push_back(matrix);
    }

} // namespace lp::gl