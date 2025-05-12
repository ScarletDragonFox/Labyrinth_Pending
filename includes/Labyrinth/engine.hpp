#ifndef LABYRINTH_PENDING_ENGINE_HPP
#define LABYRINTH_PENDING_ENGINE_HPP

#include <soloud.h>
#include <Labyrinth/Engine/ECS/coreECS.hpp>
#include <Labyrinth/Engine/Event/eventManager.hpp>
#include <Labyrinth/Engine/Resource/resourceManager.hpp>

namespace lp
{
    /// @brief the Engine class.
    class Engine
    {
        public:
        /// @brief  Initializes the Engine
        /// @return true if an error occurred, false otherwise
        bool initialize();

        /// @brief get reference to ECS
        /// @return reference to the Entity Component System
        lp::ecs::CoreECS& getECS() { return mECS; }

        /// @brief get reference to the Event Manager
        /// @return reference to Event Manager
        lp::EventManager& getEventManager() { return mEvents; }

        /// @brief get sound engine
        /// @return reference to SoLouds instance
        SoLoud::Soloud& getSoLoud() {return mSound;}

        /// @brief get Resource Manager
        /// @return reference to the resource manager
        lp::ResourceManager& getResurceManager() { return mResources; }

        /// @brief destroy the Engine & all ts data
        void destroy();

        private:
        /// @brief SoLoud's sound engine
        SoLoud::Soloud mSound;
        /// @brief Our ECS
        lp::ecs::CoreECS mECS;
        /// @brief The Event Manager
        lp::EventManager mEvents;
        /// @brief The Resource Manager.
        ///
        /// Loads/Unloads & Stores all types of resources
        lp::ResourceManager mResources;
    };
    
    extern Engine g_engine;
}

#endif //LABYRINTH_PENDING_ENGINE_HPP