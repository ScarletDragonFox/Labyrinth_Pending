#include "Labyrinth/Engine/ComponentPhysics.hpp"

#include "Labyrinth/Helpers/compilerErrors.hpp"

LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH();

#include <bullet/btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

LP_PRAGMA_DISABLE_ALL_WARNINGS_POP();

namespace lp
{

    glm::vec3 ComponentPhysics::getPosition()const
    {
        if(!mRigidBody ||  !mRigidBody->getMotionState()) return glm::vec3(0.0);
        btTransform trans;
        mRigidBody->getMotionState()->getWorldTransform(trans);
        const auto btPos = trans.getOrigin();
        return glm::vec3(btPos.getX(), btPos.getY(), btPos.getZ());
    }

    void ComponentPhysics::setPosition(const glm::vec3 cv_position, const bool resetMomentum)
    { 
        if(!mRigidBody ||  !mRigidBody->getMotionState()) return;
        btTransform trans = {};
        trans.setOrigin(btVector3(cv_position.x, cv_position.y, cv_position.z));
        mRigidBody->setWorldTransform(trans);
        mRigidBody->getMotionState()->setWorldTransform(trans);
        if(resetMomentum)
        {
            mRigidBody->clearForces();
            mRigidBody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
            mRigidBody->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
        }
        mRigidBody->activate();
    }

    glm::quat ComponentPhysics::getRotation() const
    {
        if(!mRigidBody ||  !mRigidBody->getMotionState()) return glm::quat(0, 0, 0, 0);
        btTransform trans;
        mRigidBody->getMotionState()->getWorldTransform(trans);        
        const auto btRot = trans.getRotation();
        return glm::quat(btRot.getW(), btRot.getX(), btRot.getY(), btRot.getZ());
    }

    glm::vec3 ComponentPhysics::getRotationEuler() const
    {
        if(!mRigidBody ||  !mRigidBody->getMotionState()) return  glm::vec3(0.0);
        btTransform trans;
        mRigidBody->getMotionState()->getWorldTransform(trans);
        glm::vec3 out = {};        
        trans.getRotation().getEulerZYX(out.z, out.y, out.x);
        return out;
    }

    void ComponentPhysics::setRotation(const glm::quat cv_rot, const bool resetMomentum)
    {
        if(!mRigidBody ||  !mRigidBody->getMotionState()) return;
        btTransform trans = {};
        trans.setRotation(btQuaternion(cv_rot.z, cv_rot.y, cv_rot.x, cv_rot.w));
        mRigidBody->setWorldTransform(trans);
        mRigidBody->getMotionState()->setWorldTransform(trans);
        if(resetMomentum)
        {
            mRigidBody->clearForces();
            mRigidBody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
            mRigidBody->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
        }
        mRigidBody->activate();
    }

    void ComponentPhysics::setRotation(const glm::vec3 cv_rot, const bool resetMomentum)
    {
        if(!mRigidBody ||  !mRigidBody->getMotionState()) return;
        btTransform trans = {};
        trans.setRotation(btQuaternion(cv_rot.z, cv_rot.y, cv_rot.x));
        mRigidBody->setWorldTransform(trans);
        mRigidBody->getMotionState()->setWorldTransform(trans);
        if(resetMomentum)
        {
            mRigidBody->clearForces();
            mRigidBody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
            mRigidBody->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
        }
        mRigidBody->activate();
    }
}