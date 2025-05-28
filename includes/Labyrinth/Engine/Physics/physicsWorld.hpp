#ifndef LABYRINTH_PENDING_ENGINE_PHYSICS_PHYSICSWORLD_HPP
#define LABYRINTH_PENDING_ENGINE_PHYSICS_PHYSICSWORLD_HPP
#include <memory>

#include "Labyrinth/Helpers/compilerErrors.hpp"
#include "Labyrinth/Engine/Graphics/bullet3Debug.hpp"

LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH();

#include <bullet/btBulletDynamicsCommon.h>

LP_PRAGMA_DISABLE_ALL_WARNINGS_POP();

#include "Labyrinth/Engine/Event/eventManager.hpp"

namespace lp::ph
{
    /// @brief represents the Bullet Physics World.
    ///
    /// bullet-classes stored as a shared_ptr so they can be moved.
    class PhysicsWorld
    {
        public:

        /// @brief initialize the world
        void initialize();

        /// @brief steps the simulations by timeStep
        /// @param timeStep time in secs to advance the Simulation by
        void stepSimulation(const double timeStep);

        /// @brief returns a ptr to bullets world
        /// @return pointer. May become invalid, don't store
        btDiscreteDynamicsWorld* getWorld(){ return mWorld.get(); }

        /// @brief get const pointer to debug Bullet Drawing class
        /// @return const pointer
        lp::gl::Bullet3Debug const*  getDebugRenderer() const { return &mDebugRender; }

        void destroy();

        private:
        /// @brief Debug Bullet renderer
        lp::gl::Bullet3Debug mDebugRender;
        /// @brief Broadphase, checks if collision could have occured between objects
        std::shared_ptr<btBroadphaseInterface> mBroadphase;
        /// @brief Config for mDispatcher. Must stay alive for as long as it is
        std::shared_ptr<btDefaultCollisionConfiguration> mCollisionConfiguration;
        /// @brief Dispacher, handles collisions
        std::shared_ptr<btCollisionDispatcher> mDispatcher;
        /// @brief Solver, solves the 'Constraints' of the World
        std::shared_ptr<btConstraintSolver> mSolver;
        /// @brief World, stores everything + logic
        std::shared_ptr<btDiscreteDynamicsWorld> mWorld;

        lp::EventListenerID mComponentCreatedEventID = 0;
        lp::EventListenerID mComponentDestroyedEventID = 0;
    };
} //lp::ph

#endif //LABYRINTH_PENDING_ENGINE_PHYSICS_PHYSICSWORLD_HPP