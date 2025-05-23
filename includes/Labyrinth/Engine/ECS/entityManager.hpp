#ifndef LABYRINTH_PENDING_ENGINE_ENTITYMANAGER_HPP
#define LABYRINTH_PENDING_ENGINE_ENTITYMANAGER_HPP

#include "types.hpp"

#include <queue>
#include <unordered_map>
#include <cstdint>

#include <iostream>

namespace lp::ecs
{
    /// @brief class that manages the creation and destruction of Entities
    ///
    /// This class manages:
    ///
    ///  -  creating & destroying alive Entities
    ///
    ///  - storing a list of old (dead) Entities to get new IDs from
    ///
    ///  - stroring Entity Signatures
    class EntityManager
    {
        public:
        /// @brief initializes the Entity Manager
        EntityManager();

        /// @brief create a new entity
        /// @return id of the new entity
        Entity createEntity();

        /// @brief destroy the entity
        /// @param cv_entity the entity to destroy
        void destroyEntity(const Entity cv_entity);

        /// @brief destroys the Entity Manager & deinitializes all allocations
        ~EntityManager();

        /// @brief get the total number of created Entities
        /// @return total number of created Entities
        inline std::uint32_t getTotalCreatedEntities() const { return mEntitiesCreatedTotal; }

        /// @brief get the number of currently alive Entities
        /// @return number of currently alive Entities
        inline std::uint32_t getAliveEntities() const { return mEntitiesAlive; }

        /// @brief get the reference to the signature of an entity
        /// @param cv_entity the entity in question
        /// @return reference to signature
        /// @warning if cv_entity is not an alive entity, this will crash!!
        inline Signature& getSignature(const Entity cv_entity) { return mSignatures[cv_entity]; }

        /// @brief checks if a given entity is still alive
        /// @param cv_entity the entity id to check
        /// @return true for alive, false for not
        /// @warning Can be resource-intesive! uses std::find!
        bool isAliveEntity(const Entity cv_entity)const;
        
        /// @brief get reference-to-const map of Alive Entity -> its Signature
        /// @return reference-to-const
        const std::unordered_map<Entity, Signature>& getDebugEntityMap() const { return mSignatures; }

        private:
        Entity mMaxID = 0; ///maximum currently used Entity id
        std::uint32_t mEntitiesCreatedTotal = 0; //number of total created Entities
        std::uint32_t mEntitiesAlive = 0; //number of currently alive Entities
        std::queue<Entity> mFreedEntities; //list of freed Entity ids to use
        std::unordered_map<Entity, Signature> mSignatures; //map of signatures-Entities
    };
}

#endif //LABYRINTH_PENDING_ENGINE_ENTITYMANAGER_HPP