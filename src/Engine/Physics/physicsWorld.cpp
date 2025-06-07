#include "Labyrinth/Engine/Physics/physicsWorld.hpp"

#include <Labyrinth/engine.hpp>
#include <Labyrinth/Engine/ECS/coreECS.hpp>
#include <Labyrinth/Engine/ComponentPhysics.hpp>

#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h> //btGhostPairCallback

namespace lp::ph
{
    void PhysicsWorld::initialize(){
        // Build the broadphase
        mBroadphase = std::make_shared<btDbvtBroadphase>();

        mBroadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback()); //this is here so btGhostObject can have a callback, therefore experiencing collsion.
        // Used by the player collision, so DON'T TOUCH!

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

    ColliderID_t PhysicsWorld::registerCollisionShape(btCollisionShape* v_collShape)
    {
        if(v_collShape == nullptr) return lp::ph::const_collider__id_invalid;
        const lp::ph::ColliderID_t ret = mNextColliderID;
        mCollisionShapes[mNextColliderID] = v_collShape;
        mNextColliderID++;
        return ret;
    }

    void PhysicsWorld::unloadCollisionShape(const ColliderID_t cv_id)
    {
        if(mCollisionShapes.contains(cv_id)){
            delete mCollisionShapes[cv_id];
            mCollisionShapes.erase(cv_id);
        }
    }

    btCollisionShape* PhysicsWorld::unregisterCollisionShape(const ColliderID_t cv_id)
    {
        if(mCollisionShapes.contains(cv_id)){
            btCollisionShape* ptr = mCollisionShapes[cv_id];
            mCollisionShapes.erase(cv_id);
            return ptr;
        } else return nullptr;
    }

    void PhysicsWorld::destroy()
    {
        auto& ReventM = g_engine.getEventManager();
        ReventM.unregister(this->mComponentCreatedEventID);
        ReventM.unregister(this->mComponentDestroyedEventID);
        for(const auto&i:this->mCollisionShapes)
        {
            delete i.second;
        }
    }
}