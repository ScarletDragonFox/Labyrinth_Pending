#ifndef LABYRINTH_PENDING_ENGINE_GRAPHICS_LIGHTSYSTEM_HPP
#define LABYRINTH_PENDING_ENGINE_GRAPHICS_LIGHTSYSTEM_HPP
#include <glad/gl.h>
#include <vector>

#include "Labyrinth/Engine/ECS/baseSystem.hpp"
#include "Labyrinth/Engine/ComponentLight.hpp"

namespace lp
{
    class LightSystem: public lp::ecs::System
    {
        public:
        /// @brief update the light system
        void update();

        /// @brief get number of active/alive lights
        GLuint getLightCount() const { return mLightCount; }

        /// @brief get the buffer of ALL lights
        GLuint getLightBuffer() const { return mShaderStorageBufferLights; }

        /// @brief get buffer-list of every live light index
        ///
        /// Used to interate over main light buffer in-shader
        GLuint getAliveLightListBuffer() const { return mAliveLightListBuffer; }

        ///TODO: garbage collection!
        // Currently, the buffer will grow into infinity
        // Add capacity to 

        private:
        /// @brief SSBO of all lights in scene
        GLuint mShaderStorageBufferLights = 0;
        /// @brief count of current amount of storage in buffer
        GLuint mLightCount = 0;
        /// @brief size of mShaderStorageBufferLights. 8 is default (set when constructed)
        std::size_t mMaxLightCount = 0;

        /// @brief A map of Entity -> index
        ///
        /// A value can either be a valid Entity, in which case that slot is taken,
        ///
        /// or lp::ecs::const_entity_invalid, when the slot is empty
        ///
        /// index (at which the Entity is) is used as buffer offset
        ///
        /// offset = index * sizeof(LightGLSL)
        ///
        std::vector<lp::ecs::Entity> mLightBufferEntityMap;

        /// @brief a list of all alive light indicies
        ///
        /// Used as data by mAliveLightListBuffer
        ///
        /// Squashed version of mLightBufferEntityMap, basically
        std::vector<GLuint> mAliveLightList;

        /// @brief SSB of alive lights in scene
        ///
        /// Stores indecies for the actual light buffer for those live lights
        ///
        /// This buffer will always have the same parameters (max size, used count) as mShaderStorageBufferLights
        GLuint mAliveLightListBuffer = 0;


        /// @brief internal function to glBufferSubData update one light
        /// @param cv_offset offset where to put the new light's data
        /// @param r_light reference to light in question
        void updateSingleLight(const GLintptr cv_offset, lp::ComponentLight& r_light);

        /// @brief internal function to add a light to buffer WHEN SPACE IS AVAILABLE!
        ///
        /// Increments mLightCount
        /// @param cv_lightEnt Entity to add
        void addLightSingle(const lp::ecs::Entity cv_lightEnt);


        /// @brief internal function called when dirty == false in update()
        void updateNotDirty();

        /// @brief internal function called when a new light needs to be added to the buffers in update()
        void updateIncreaseLights();

        /// @brief internal function called when the buffers get too big in update()
        ///
        /// This resends the entire mShaderStorageBufferLights & resizes mAliveLightListBuffer
        void updateGarbageCollection();
    };
}

#endif //LABYRINTH_PENDING_ENGINE_GRAPHICS_LIGHTSYSTEM_HPP