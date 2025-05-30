#include "Labyrinth/Engine/ComponentSoundSource.hpp"

#include "Labyrinth/engine.hpp"

namespace lp
{
    void ComponentSoundSource::setPosition(const glm::vec3 cv_pos)
    {
        lp::g_engine.getSoLoud().set3dSourcePosition(mHandle, cv_pos.x, cv_pos.y, cv_pos.z);
    }

    void ComponentSoundSource::setVelocity(const glm::vec3 cv_velo)
    {
        lp::g_engine.getSoLoud().set3dSourceVelocity(mHandle, cv_velo.x, cv_velo.y, cv_velo.z);
    }

    void ComponentSoundSource::setPositionVelocity(const glm::vec3 cv_pos, const glm::vec3 cv_velo)
    {
        lp::g_engine.getSoLoud().set3dSourceParameters(mHandle, cv_pos.x, cv_pos.y, cv_pos.z, cv_velo.x, cv_velo.y, cv_velo.z);
    }

    void ComponentSoundSource::setMin(const float cv_min)
    {
        this->mMin = cv_min;
        lp::g_engine.getSoLoud().set3dSourceMinMaxDistance(this->mHandle, mMin, mMax);
    }

    void ComponentSoundSource::setMax(const float cv_max)
    {
        this->mMax = cv_max;
        lp::g_engine.getSoLoud().set3dSourceMinMaxDistance(this->mHandle, mMin, mMax);
    }

    void ComponentSoundSource::setRolloffFactor(const float cv_RolloffFactor)
    {
        this->mRolloffFactor = cv_RolloffFactor;
        lp::g_engine.getSoLoud().set3dSourceAttenuation(this->mHandle, mAttenuation, mRolloffFactor);
    }

    void ComponentSoundSource::setAttenuationModel(const  SoLoud::AudioSource::ATTENUATION_MODELS cv_model)
    {
        this->mAttenuation = cv_model;
        lp::g_engine.getSoLoud().set3dSourceAttenuation(this->mHandle, mAttenuation, mRolloffFactor);
    }
}