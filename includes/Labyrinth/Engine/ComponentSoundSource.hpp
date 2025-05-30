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
        inline SoLoud::handle getHandle(){ return mHandle; }

        /// @brief get min of this sound instance
        /// @note below this distance, volume is always at 100% (Attenuation)
        inline float getMin() const { return mMin; }

        /// @brief get max of this sound instance
        /// @note Above mMax, there (should) be no sound
        inline float getMax() const { return mMax; }
        
        /// @brief set the min distance of the sound source
        /// @param cv_min below this distance, volume is always at 100%
        void setMin(const float cv_min);

        /// @brief set the max distance of the sound source
        /// @param cv_max Above mMax, there (should) be no sound
        void setMax(const float cv_max);

        /// @brief set the rollff factor. Used for calculating attenuation
        ///
        /// volume is interpolated beteen mMin & mMax using the Rolloff Factor
        /// @param cv_RolloffFactor 1.0 is default
        void setRolloffFactor(const float cv_RolloffFactor);

        /// @brief set the attenuation model
        /// @param cv_model SoLoud::AudioSource::LINEAR_DISTANCE is default
        /// @see https://solhsa.com/soloud/concepts3d.html
        void setAttenuationModel(const  SoLoud::AudioSource::ATTENUATION_MODELS cv_model);

        /// @brief so it can access mHandle & set it!
        friend class lp::res::SoundManager;

        private:
        /// @brief Handle to the Sound Source in SoLoud
        SoLoud::handle mHandle = 0;

        /// @brief below this distance volume is always at 100% (Attenuation)
        float mMin = 100.0;

        /// @brief volume is interpolated beteen mMin & mMax (Attenuation)
        ///
        // Above mMax, there (should) be no sound
        float mMax = 200.0;

        /// @brief constant for the Attenuation equation
        /// @see https://solhsa.com/soloud/concepts3d.html
        float mRolloffFactor = 1.0;

        /// @brief model used for calculating attenuation
        /// @see https://solhsa.com/soloud/concepts3d.html
        SoLoud::AudioSource::ATTENUATION_MODELS mAttenuation = SoLoud::AudioSource::LINEAR_DISTANCE;
    };
} //lp

#endif //LABYRINTH_PENDING_ENGINE_COMPONENTSOUNDSOURCE_HPP