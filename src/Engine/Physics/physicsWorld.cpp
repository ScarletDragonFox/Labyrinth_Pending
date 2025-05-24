#include "Labyrinth/Engine/Physics/physicsWorld.hpp"

namespace lp::ph
{
    void PhysicsWorld::initialize(){
        // Build the broadphase
        mBroadphase = std::make_shared<btDbvtBroadphase>();
        // Set up the collision configuration and dispatcher
        mCollisionConfiguration = std::make_shared<btDefaultCollisionConfiguration>();
        mDispatcher = std::make_shared<btCollisionDispatcher>(mCollisionConfiguration.get());
        // The actual physics solver
        mSolver = std::make_shared<btSequentialImpulseConstraintSolver>();
        // The world.
        mWorld = std::make_shared<btDiscreteDynamicsWorld>(mDispatcher.get(), mBroadphase.get(), mSolver.get(), mCollisionConfiguration.get());

        mWorld->setDebugDrawer(&mDebugRender);
    }

    void PhysicsWorld::stepSimulation(const double timeStep)
    {
        mWorld->stepSimulation(static_cast<btScalar>(timeStep), 100);
        mWorld->debugDrawWorld();
        mDebugRender.flushLines();
    }
}