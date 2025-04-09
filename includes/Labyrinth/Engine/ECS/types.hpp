#ifndef LABYRINTH_PENDING_ENGINE_ECS_TYPES_HPP
#define LABYRINTH_PENDING_ENGINE_ECS_TYPES_HPP
#include <cstdint>

namespace lp
{
    /// @brief Entity - simple number
    ///
    /// an id to which Components are ties to via a Signature
    ///
    ///
    using Entity = std::uint32_t;

    /// @brief invalid Entity id
    ///
    /// Since it's 0, it's the default value for an Entity
    constexpr Entity const_entity_invalid = 0;

    /// @brief Signature - also just a number
    ///
    /// With a Max Unique Component Count of 16 stores:
    ///
    /// For an Entity:
    ///
    /// - which Component this Entity has
    ///
    /// For a System:
    ///
    /// - which Component does this System want its registered Entities to have?
    ///
    ///
    /// For a Component:
    ///
    /// - the unique id (bitmask) of this Component (single bit set to true)
    ///
    /// - used for determining what Entity/System has/wants what Component
    using Signature = std::uint16_t;
}

#endif //LABYRINTH_PENDING_ENGINE_ECS_TYPES_HPP