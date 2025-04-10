#include "Labyrinth/Engine/ECS/coreECS.hpp"

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
}