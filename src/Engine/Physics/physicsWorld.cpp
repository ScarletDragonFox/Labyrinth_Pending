#include "Labyrinth/Engine/Physics/physicsWorld.hpp"

#include <Labyrinth/engine.hpp>
#include <Labyrinth/Engine/ECS/coreECS.hpp>
#include <Labyrinth/Engine/ComponentPhysics.hpp>

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

        auto& ReventM = g_engine.getEventManager();
        const auto signCompPhysics = g_engine.getECS().getComponentSignature<ComponentPhysics>();
        mComponentCreatedEventID = ReventM.on(lp::EventTypes::ECS_ComponentCreated, [signCompPhysics, this](lp::Event& evt)
        {
            auto data = evt.getData<evt::ECS_Component>();
            if(data.signature &signCompPhysics)
            {
                auto& comp = g_engine.getECS().getComponent<lp::ComponentPhysics>(data.entity);
                if(comp.mRigidBody != nullptr && comp.mState != nullptr)
                {
                    this->mWorld->addRigidBody(comp.mRigidBody.get());
                }
            }
        });

        mComponentDestroyedEventID = ReventM.on(lp::EventTypes::ECS_ComponentDestroyed, [signCompPhysics, this](lp::Event& evt)
        {
            auto data = evt.getData<evt::ECS_Component>();
            if(data.signature &signCompPhysics)
            {
                auto& comp = g_engine.getECS().getComponent<lp::ComponentPhysics>(data.entity);
                if(comp.mRigidBody != nullptr && comp.mState != nullptr)
                {
                    this->mWorld->removeRigidBody(comp.mRigidBody.get());
                }
            }
            
        });
    }

    void PhysicsWorld::stepSimulation(const double timeStep)
    {
        mWorld->stepSimulation(static_cast<btScalar>(timeStep), 100);
        mWorld->debugDrawWorld();
        mDebugRender.flushLines();
    }

    void PhysicsWorld::destroy()
    {
        auto& ReventM = g_engine.getEventManager();
        ReventM.unregister(this->mComponentCreatedEventID);
        ReventM.unregister(this->mComponentDestroyedEventID);
    }
}