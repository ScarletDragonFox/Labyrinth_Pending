#ifndef LABYRINTH_PENDING_ENGINE_COMPONENTPOSITION_HPP
#define LABYRINTH_PENDING_ENGINE_COMPONENTPOSITION_HPP

#include "Labyrinth/Helpers/compilerErrors.hpp"

LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH()
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
LP_PRAGMA_DISABLE_ALL_WARNINGS_POP()

namespace lp
{
    /// @brief Component that represents a simple position+orientation+scale
    ///
    /// An Entity can either use have this, or have a ComponentPhysics
    ///
    /// Do NOT mix them together!
    class ComponentPosition
    {
        public:
        /// @brief set the position
        /// @param cv_newPos new position
        void setPosition(const glm::vec3 cv_newPos)
        {
            mPosisiton = cv_newPos;
        }

        /// @brief set the rotation (radians)
        /// @param cv_newRot new orientation
        void setRotation(const glm::quat cv_newRot)
        {
            mOrientation = cv_newRot;
        }

        /// @brief set the scale
        /// @param cv_newPos new scale
        void setScale(const glm::vec3 cv_newScale)
        {
            mScale = cv_newScale;
        }

        /// @brief get the position
        /// @return the position
        glm::vec3 getPosition() const { return this->mPosisiton; }

        /// @brief get the rotation
        /// @return the rotation (radians)
        glm::quat getRotation() const { return this->mOrientation; }

        /// @brief get the scale
        /// @return the scale
        glm::vec3 getScale() const { return this->mScale; }

        /// @brief calculate the model matrix
        /// @return the model matrix
        glm::mat4 getModelMatrix() const;
        private:
        glm::vec3 mPosisiton = {};
        glm::vec3 mScale = {1.0f, 1.0f, 1.0f};
        glm::quat mOrientation = {};
    };
}

#endif //LABYRINTH_PENDING_ENGINE_COMPONENTPOSITION_HPP