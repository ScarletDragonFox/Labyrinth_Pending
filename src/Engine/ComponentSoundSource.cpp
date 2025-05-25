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
}