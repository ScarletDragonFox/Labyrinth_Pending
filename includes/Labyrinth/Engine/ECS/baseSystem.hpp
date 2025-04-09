#ifndef LABYRINTH_PENDING_ENGINE_ECS_BASESYSTEM_HPP
#define LABYRINTH_PENDING_ENGINE_ECS_BASESYSTEM_HPP
#include "types.hpp"

#include <set>

namespace lp::ecs
{
    /// @brief Base class for all Systems
    ///
    /// this just has a std::set with the Entities in it, 
    ///
    /// alongside a dirty flag for when the Entities changed (added, removed, doesn't trigger for Component changes)
    class System
    {
        protected:
        /// @brief set of all Entities for this System
        ///
        /// Do not modify within the System!! (won't break, just unintented)
        std::set<Entity> mEntities;

        /// @brief dirtly flag for the set of Entities.
        ///
        /// Set this to false after an update
        bool mDirty = true;

        /// @brief Here so the it can access the Systems data & update it
        friend class SystemManager;
    };
}

#endif //LABYRINTH_PENDING_ENGINE_ECS_BASESYSTEM_HPP