#ifndef LABYRINTH_PENDING_ENGINE_HPP
#define LABYRINTH_PENDING_ENGINE_HPP

#include <memory>

#include "Labyrinth/Engine/Resource/soundManager.hpp"

namespace SoLoud
{
    /// @brief SoLoud class forward definition.
    class Soloud;
}

namespace lp
{
    namespace ecs
    {
        /// @brief CoreECS class forward definition.
        class CoreECS;
    }
    
    /// @brief EventManager class forward definition.
    class EventManager;

    /// @brief ResourceManager class forward definition.
    class ResourceManager;

    /// @brief the Engine class.
    class Engine
    {
        public:

        /// @brief constructs the shared pointers
        Engine();

        /// @brief  Initializes the Engine
        /// @return true if an error occurred, false otherwise
        bool initialize();

        /// @brief get reference to ECS
        /// @return reference to the Entity Component System
        lp::ecs::CoreECS& getECS() { return *mECSPtr.get(); }

        /// @brief get reference to the Event Manager
        /// @return reference to Event Manager
        lp::EventManager& getEventManager() { return *mEventsPtr.get(); }

        /// @brief get sound engine
        /// @return reference to SoLouds instance
        SoLoud::Soloud& getSoLoud() {return *mSoundPtr.get();}

        /// @brief get Resource Manager
        /// @return reference to the resource manager
        lp::ResourceManager& getResurceManager() { return *mResourcesPtr.get(); }

        /// @brief destroy the Engine & all ts data
        void destroy();

        private:
        /// @brief SoLoud's sound engine
        std::shared_ptr<SoLoud::Soloud> mSoundPtr;

        /// @brief Our ECS
        std::shared_ptr<lp::ecs::CoreECS> mECSPtr;

        /// @brief The Event Manager
        std::shared_ptr<lp::EventManager> mEventsPtr;

        /// @brief The Resource Manager.
        ///
        /// Loads/Unloads & Stores all types of resources
        std::shared_ptr<lp::ResourceManager> mResourcesPtr;

        /// @brief Manager of Sound
        lp::res::SoundManager mSoundMan;
    };
    
    extern Engine g_engine;
}

#endif //LABYRINTH_PENDING_ENGINE_HPP