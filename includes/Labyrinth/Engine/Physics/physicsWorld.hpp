#ifndef LABYRINTH_PENDING_ENGINE_PHYSICS_PHYSICSWORLD_HPP
#define LABYRINTH_PENDING_ENGINE_PHYSICS_PHYSICSWORLD_HPP
#include <memory>

#include "Labyrinth/Helpers/compilerErrors.hpp"
#include "Labyrinth/Engine/Graphics/bullet3Debug.hpp"

LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH();

#include <bullet/btBulletDynamicsCommon.h>

LP_PRAGMA_DISABLE_ALL_WARNINGS_POP();

#include "Labyrinth/Engine/ECS/baseSystem.hpp"

#include "Labyrinth/Engine/Event/eventManager.hpp"

#include <unordered_map>

#include "Labyrinth/Engine/Physics/colliderID_t.hpp"

namespace lp::ph
{
    /// @brief system used by the PhysicsWorld to keep track of all entities with a ComponentPhysics
    class PhysicsWorldRemovalSystem: public lp::ecs::System
    {
        public:
        /// @brief get the dirty flag
        /// @return the dirty flag
        bool getDirty() const { return this->mDirty; }

        /// @brief set the dirty flag
        /// @param cv_Dirty the dirty flag
        void setDirty(const bool cv_Dirty) { this->mDirty = cv_Dirty; }

        /// @brief get the set of Entities
        /// @return const reference to set
        std::set<lp::ecs::Entity> const & getSet() const{ return this->mEntities; }
    };

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

        /// @brief 'register' a new btCollisionShape, taking over its ownership
        /// @param v_collShape the btCollisionShape pointer to take ownership of
        /// @return id or lp::ph::const_collider__id_invalid if v_collShape == nullptr
        ColliderID_t registerCollisionShape(btCollisionShape* v_collShape);

        /// @brief unload a btCollisionShape
        /// @param cv_id id to the collision shape
        /// @warning Make sure that nothing uses this Collider BEFORE calling this function!
        void unloadCollisionShape(const ColliderID_t cv_id);

        /// @brief unregister a btCollisionShape
        ///
        /// The specified collider will loose its assosiation with this physics world!
        /// @param cv_id id to the collision shape
        /// @note unlike unloadCollisionShape(), this doesn't unload anything
        /// @return the collision shape in question, or nullptr if not found
        btCollisionShape* unregisterCollisionShape(const ColliderID_t cv_id);

        /// @brief get a Debug const reference to btCollisionShape Container
        /// @return const reference to Container
        const std::unordered_map<ColliderID_t, btCollisionShape*>& getDebugCollisionShapeContainer() const { return mCollisionShapes; }

        /// @brief destroy the physics world, unload all the things
        void destroy();

        /// @brief if true, will draw bullet debug lines
        bool mDrawDebug = false;

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

        /// @brief id of event listener, used for destroying it in the destructor
        lp::EventListenerID mComponentCreatedEventID = 0;
         /// @brief id of event listener, used for destroying it in the destructor
        lp::EventListenerID mComponentDestroyedEventID = 0;

        /// @brief container of all collisionshapes
        std::unordered_map<ColliderID_t, btCollisionShape*> mCollisionShapes;

        /// @brief id of the next ColliderID_t
        ColliderID_t mNextColliderID = const_collider__id_invalid + 1;
    };
} //lp::ph

#endif //LABYRINTH_PENDING_ENGINE_PHYSICS_PHYSICSWORLD_HPP