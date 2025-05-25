#ifndef LABYRINTH_PENDING_ENGINE_COMPONENTSOUNDSOURCE_HPP
#define LABYRINTH_PENDING_ENGINE_COMPONENTSOUNDSOURCE_HPP

#include "Labyrinth/Helpers/compilerErrors.hpp"

LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH()
#include <soloud.h>
#include <glm/vec3.hpp>
LP_PRAGMA_DISABLE_ALL_WARNINGS_POP()

namespace lp
{
    namespace res
    {
        /// @brief forward declaration
        class SoundManager;
    }

    /// @brief Component representing a sound playing
    ///
    /// Gets removed when the sound stops
    class ComponentSoundSource
    {
        public:
        /// @brief set the position of the sound
        /// @param cv_pos new position
        void setPosition(const glm::vec3 cv_pos);

        /// @brief set the velocity of the sound 
        /// @param cv_velo new velocity
        void setVelocity(const glm::vec3 cv_velo);

        /// @brief set the positon AND velocitu of the sound
        /// @param cv_pos new position
        /// @param cv_velo new velocity
        void setPositionVelocity(const glm::vec3 cv_pos, const glm::vec3 cv_velo);

        /// @brief get the SoLoud::handle to the sound source
        /// @return SoLoud::handle
        SoLoud::handle getHandle(){ return mHandle; }

        /// @brief so it can access mHandle & set it!
        friend class lp::res::SoundManager;

        private:
        /// @brief Handle to the Sound Source in SoLoud
        SoLoud::handle mHandle = 0; 
    };
} //lp

#endif //LABYRINTH_PENDING_ENGINE_COMPONENTSOUNDSOURCE_HPP