#ifndef LABYRINTH_PENDING_ENGINE_PHYSICS_COMPONENT_HPP
#define LABYRINTH_PENDING_ENGINE_PHYSICS_COMPONENT_HPP

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>

class btRigidBody;
class btDefaultMotionState;

#include "Labyrinth/Engine/Physics/colliderID_t.hpp"

namespace lp
{
    namespace ph
    {
        /// @brief forward declaration
        class PhysicsWorld;
    }

    class ComponentPhysics
    {
        public:

        /// @brief get the current, interpolated position of this object
        /// @return position OR vec3(0.0) if error
        glm::vec3 getPosition()const;

        /// @brief get the current, interpolated linear velocity of this object 
        /// @return (linear) velocity OR vec3(0.0) if error
        glm::vec3 getVelocity()const;

        /// @brief 'set the current position of the object'
        ///
        /// Teleports the object.
        ///
        /// By default resets objects momentum/forces
        /// @param cv_position new position
        /// @param resetMomentum if true momentum + forces gets set to 0
        void setPosition(const glm::vec3 cv_position, const bool resetMomentum = true);

        /// @brief get the current, interpolated rotation of this object (as a quat)
        /// @return rotation OR quat(0,0,0,0) if error
        glm::quat getRotation() const;

        /// @brief get the current, interpolated rotation of this object (as a vec3 Euler rotation)
        /// @return rotation OR vec3(0.0) if error
        glm::vec3 getRotationEuler() const;

        /// @brief set the object's rotation
        ///
        /// By default resets objects momentum/forces
        /// @param cv_rot quaternion rotation
        /// @param resetMomentum if true momentum + forces gets set to 0
        void setRotation(const glm::quat cv_rot, const bool resetMomentum = true);

        /// @brief set the object's rotation
        ///
        /// By default resets objects momentum/forces
        /// @param cv_rot Euler rotation
        /// @param resetMomentum if true momentum + forces gets set to 0
        void setRotation(const glm::vec3 cv_rot, const bool resetMomentum = true);

        friend class lp::ph::PhysicsWorld;

        //private:
        public: //temporary
        /// @brief this must be a ptr. the address of a component can change,
        ///
        /// But the address of all Bullet-physics-world-aware classes must stay the same!
        ///
        /// The only other way to solve this is to have some kind of static allocation pool to store that in
        std::shared_ptr<btRigidBody> mRigidBody;
        /// @brief the body's motion state
        ///
        /// Stores its position & orientation
        std::shared_ptr<btDefaultMotionState> mState;

        /// @brief 'origin id' of this mRigidBody
        ///
        /// So when the level/ any rigidbody gets unloaded, we know which ComponentPhysics to kill
        lp::ph::ColliderID_t mColliderID = lp::ph::const_collider__id_invalid;

        // the std::shared_ptr's can't be unique_ptr's.
        // those don't like to be moved
        // which causes problems with std library's container classes
    };
}

#endif //LABYRINTH_PENDING_ENGINE_PHYSICS_COMPONENT_HPP