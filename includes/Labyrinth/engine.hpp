#ifndef LABYRINTH_PENDING_ENGINE_HPP
#define LABYRINTH_PENDING_ENGINE_HPP

#include <soloud.h>
#include <Labyrinth/Engine/ECS/coreECS.hpp>
#include <Labyrinth/Engine/eventManager.hpp>

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
        /// @return reference to ECS
        lp::ecs::CoreECS& getECS() { return mECS; }

        lp::EventManager& getEventManager() { return mEvents; }

        SoLoud::Soloud& getSoLoud() {return mSound;}

        /// @brief destroy the Engine & all ts data
        void destroy();

        private:
        /// @brief SoLoud's sound engine
        SoLoud::Soloud mSound;
        /// @brief Our ECS
        lp::ecs::CoreECS mECS;
        /// @brief The Event Manager
        lp::EventManager mEvents;
    };
    
    extern Engine g_engine;
}

#endif //LABYRINTH_PENDING_ENGINE_HPP