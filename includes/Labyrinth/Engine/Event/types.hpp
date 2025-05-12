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
        /// Uses  evt::WindowResize as data
        WindowResize,

        /// @brief gets called when any key is interacted with.
        ///
        /// Uses evt::WindowKeyAction as data
        WindowKeyAction,

        /// @brief gets called when the mouse moves.
        ///
        /// Uses evt::WindowMouseMotion as data
        WindowMouseMotion,

        /// @brief gets called when the inputs (keyboard/mouse) are enable/disabled
        ///
        /// Used bool as data, true if inputs are to be disabled, and false otherwise
        PlayerTriggerInputs,


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

        /// @brief stores data for lp::EventTypes::WindowKeyAction
        /// @see https://www.glfw.org/docs/3.3/input_guide.html#input_key
        struct WindowKeyAction
        {
            int key = 0; //key pressed (GLFW_KEY_NAME)
            int scancode = 0; //scancode of the key. More accurate, but platform-dependant.
            bool pressed: 1 = false; //true for GLFW_PRESS, false for GLFW_RELEASE
            bool modShift:1 = false; //was Shift pressed?
            bool modControl:1 = false; //was Control pressed?
            bool modAlt:1 = false; //was Alt pressed?
            bool modSuper:1 = false; //was any of the Super keys pressed? (the 'windows' key)
        };

        /// @brief stores data for lp::EventTypes::WindowMouseMotion
        /// @see https://www.glfw.org/docs/3.4/input_guide.html#cursor_pos
        struct WindowMouseMotion
        {
            /// @brief current X postion of the mouse cursor
            double posX = 0.0;
            /// @brief current Y postion of the mouse cursor
            double posY = 0.0;
            /// @brief offset X of the mouse cursor from the last position reported
            double deltaX = 0.0;
            /// @brief offset Y of the mouse cursor from the last position reported
            double deltaY = 0.0;
        };

    }
}

#endif //LABYRINTH_PENDING_ENGINE_TYPES_HPP