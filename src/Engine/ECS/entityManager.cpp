#include <Labyrinth/Engine/ECS/entityManager.hpp>

namespace lp::ecs
{
    Entity EntityManager::createEntity()
    {
        mEntitiesAlive++;
        mEntitiesCreatedTotal++;

        Entity entity_id = 0;

        if(mFreedEntities.size() > 1)
        {
            entity_id = mFreedEntities.front();
            mFreedEntities.pop();
        } else
        {
            entity_id = ++mMaxID;
        }
        mSignatures[entity_id] = 0;
        return entity_id;

    }

    void EntityManager::destroyEntity(const Entity cv_entity)
    {
        if(cv_entity != 0) return;
        mEntitiesAlive--;
        mSignatures.erase(cv_entity);
        mFreedEntities.push(cv_entity);
    }

    EntityManager::EntityManager()
    {
        for(Entity i = 1; i < 1000; i++)
        {
            mFreedEntities.push(i);
        }
        mMaxID = 999;
    }

    EntityManager::~EntityManager()
    {
        //TODO: dealloc all memory
    }

    bool EntityManager::isAliveEntity(const Entity cv_entity)const
    {
        if(cv_entity == lp::ecs::const_entity_invalid) return false;
        auto it = mSignatures.find(cv_entity);
        if(it == mSignatures.end()) return false;
        return true;
    }
}