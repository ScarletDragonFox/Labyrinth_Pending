#ifndef LABYRINTH_PENDING_ENGINE_COMPONENTLIGHT_HPP
#define LABYRINTH_PENDING_ENGINE_COMPONENTLIGHT_HPP

#include <glm/vec3.hpp>

namespace lp
{
    /// @brief Component representing a Point Light
    ///
    /// the mPosition variable stores a position OR an offset from a position, depending on other components
    class ComponentLight
    {
        public:
        /// @brief ready-made structure for glsl std140 format
        ///
        /// DO NOT CHANGE STRUCTURE WITHOUT CHANGING COMPLEMENTING ONE IN shaders/!!!!
        struct StructuredGLSL
        {
            glm::vec3 position = {}; 
            float radius = 0.0;
            glm::vec3 color = {};
            /// @brief shadow id -1. 0 means no shadow
            int shadowID = 0;
        };
        
        /// @brief gets the light colour
        /// @return light color
        inline glm::vec3 getColor() const { return mColour;} 

        /// @brief sets the light colour
        /// @param cv_colour light color
        inline void setColor(const glm::vec3 cv_colour)
        {
            mColour = cv_colour;
            mDirty = true;
        }

        /// @brief gets the light position/offset
        /// @return light position/offset
        inline glm::vec3 getPosition() const { return mPosition;} 

        /// @brief sets the light position/offset
        /// @param cv_position light position/offset
        inline void setPosition(const glm::vec3 cv_position)
        {
            mPosition = cv_position;
            mDirty = true;
        }

        /// @brief gets the light radius
        /// @return light radius
        inline float getRadius() const { return mRadius;} 

        /// @brief sets the light radius
        /// @param cv_radius light radius
        inline void setRadius(const float cv_radius)
        {
            mRadius = cv_radius;
            mDirty = true;
        }

        /// @brief clears the dirty bit
        ///
        /// Be really careful when calling this, as a number of system relies on this!
        void clearDirty()
        {
            mDirty = false;
        }

        /// @brief get the dirty bit
        /// @return true if component was modified since last clearDirty(), false otherwise
        bool getDirty() const { return mDirty; }

        /// @brief get the data as a glsl-ready structure
        ///
        /// If mPosition is an offset, remember to add (the base position [0,0,0 point] of the offset) TO o_struct.position!
        ///
        /// This function does not handle that!
        /// @param o_struct output of function
        void getGLSLStructure(StructuredGLSL& o_struct)
        {
            o_struct.color = this->mColour;
            o_struct.position = this->mPosition;
            o_struct.radius = this->mRadius;
            o_struct.shadowID = 0;
        }

        private:
        /// @brief position/offset of the light
        glm::vec3 mPosition = {};

        /// @brief colour of the light
        glm::vec3 mColour = {};

        /// @brief radius of the light
        float mRadius = 0.0f;

        /// @brief dirty flag. True if Component was modified since last update.
        bool mDirty = false;
    };
}

#endif //LABYRINTH_PENDING_ENGINE_COMPONENTLIGHT_HPP