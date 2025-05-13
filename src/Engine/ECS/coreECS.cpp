#include "Labyrinth/Engine/ECS/coreECS.hpp"
#include "Labyrinth/engine.hpp"

namespace lp::ecs
{
    Entity CoreECS::createEntity()
    {
        return mEntity.createEntity();
    }

    void CoreECS::destroyEntity(const Entity cv_entity)
    {
        mEntity.destroyEntity(cv_entity);
        mSystem.destroyEntity(cv_entity);
        mComponent.destroyEntity(cv_entity);
    }

    bool CoreECS::isAlive(const Entity cv_entity) const
    {
        return mEntity.isAliveEntity(cv_entity);
    }

    void CoreECS::sendEventComponentDestroyed(const Signature cv_signature, const Entity cv_entity)
    {
        const lp::evt::ECS_Component data = 
        {
            .entity = cv_entity,
            .signature = cv_signature
        };
        lp::Event evt(lp::EventTypes::ECS_ComponentDestroyed, data);
        g_engine.getEventManager().emit(evt);
    }

    void CoreECS::sendEventComponentCreated(const Signature cv_signature, const Entity cv_entity)
    {
        const lp::evt::ECS_Component data = 
        {
            .entity = cv_entity,
            .signature = cv_signature
        };
        lp::Event evt(lp::EventTypes::ECS_ComponentDestroyed, data);
        g_engine.getEventManager().emit(evt);
    }
}