#ifndef LABYRINTH_PENDING_ENGINE_PHYSICS_COLLIDERID_T_HPP
#define LABYRINTH_PENDING_ENGINE_PHYSICS_COLLIDERID_T_HPP

#include <cstdint> //for std::uint32_t

///@file a completile unneeded file, execpt for spaghetti code.s

namespace lp::ph
{
    /// @brief id of the btCollisionShape
    using ColliderID_t = std::uint32_t;

    /// @brief invalid ColliderID_t. Used as default
    constexpr ColliderID_t const_collider__id_invalid = 0;
}

#endif //LABYRINTH_PENDING_ENGINE_PHYSICS_COLLIDERID_T_HPP