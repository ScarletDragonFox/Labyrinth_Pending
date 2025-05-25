#ifndef LABYRINTH_PENDING_ENGINE_PHYSICS_COMPONENT_HPP
#define LABYRINTH_PENDING_ENGINE_PHYSICS_COMPONENT_HPP

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>

class btRigidBody;
class btDefaultMotionState;

// the .cpp file has not been added to CmakeListts!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

namespace lp
{
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

        private:
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

        //we might need to have these NOT be unique_ptr.
        // those don't like to be moved
        // which causes problems with std library's container classes


        // std::shared_ptr-like class for storing a ref to the btCollsisionShape
        // (the shape MUST stay alive for at least as long as every obj using it does!!!)


        //mass is stored in the CollisionShape, but it looks like we can overload it?
        //CollisionShapes CAN be shared among multiple objects


        //have a seperate static & dynamic components??
        // ANSWER: don't bother, just disapprove changing it.

            // 1. When a component with physics dies...
            // 2. Bullet needs to be notified
            // 3. to remove the Entity object from its World
            // Adding \ removing component types is dynamic-ish
            // We need to sent an EntityDeath event?

        //Another thing to do is use that cmake 'filter?' function to put files in right folders for msvc
        // Also try to make msvc work out-of-the-box
        // Not just compile-wise, but whole experience-wise
        // (like set the default startup project & stuff)
        // (maybe hide the non-runnable library dependancies in some kind of hierarchy if we can????)

    };
}

#endif //LABYRINTH_PENDING_ENGINE_PHYSICS_COMPONENT_HPP