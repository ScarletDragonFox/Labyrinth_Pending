#ifndef LABYRINTH_PENDING_ENGINE_TYPES_HPP
#define LABYRINTH_PENDING_ENGINE_TYPES_HPP
#include <cstdint>

namespace lp
{
    /// @brief enum for all possible Event types.
    /// @note The enums MUST be sequential, from '0' to EventTypes::Count
    enum class EventTypes: std::uint16_t
    {
        /// @brief uses as a default, invalid value for EventTypes
        Invalid = 0,

        /// @brief gets called when the window gets resized.
        /// 
        /// Uses @ref evt::WindowResize as data
        WindowResize,

        /// @brief count of how many event types there are.
        /// @details Leave this here as the last event
        Count,
    };

    /// @brief contains all the structs for the std::any data storage for the events
    namespace evt
    {
        /// @brief stores data for lp::EventTypes::WindowResize
        /// @see https://www.glfw.org/docs/3.3/window_guide.html#window_fbsize
        struct WindowResize
        {
            int width = 0; //width of the screens buffer
            int height = 0; //height of the screens buffer
        };


    }
}

#endif //LABYRINTH_PENDING_ENGINE_TYPES_HPP