#ifndef LABYRINTH_PENDING_PLAYER_HPP
#define LABYRINTH_PENDING_PLAYER_HPP
#include <glm/glm.hpp>
#include <array>

#include "Labyrinth/Engine/eventManager.hpp"

namespace lp
{
    class Player
    {
        public:
        Player();

        /// @brief set the player's position (or the camera's position)
        /// @param cv_NewPos new position
        inline void setPosition(const glm::dvec3 cv_NewPos)
        {
            mPosition = cv_NewPos;
        }

        /// @brief set the player's orientation (or the camera's orientation)
        /// @param cv_yaw yaw of the camera
        /// @param cv_pitch pitch of the camera, in degrees from 90 to -90
        inline void setOrientation(const double cv_yaw, const double cv_pitch)
        {
            mOrientation = {cv_yaw, cv_pitch};
            this->checkConstraints();
            this->updateVectors();
        }

        void update(const double deltaTime);

        ~Player();
        private:

        enum class Action: uint_fast8_t
        {
            Forward,
            Back,
            Left,
            Right,
            Jump,
            Count
        };

        struct KeyData
        {
            unsigned int key = 0; //key being pressed (GLFW_KEY_XXX)
            unsigned int scancode = 0; //scan (platform-specific key code)
            bool pressed:1; //true for GLFW_PRESS, false for GLFW_RELEASE
            bool modShift:1; //was Shift pressed?
            bool modControl:1; //was Control pressed?
            bool modAlt:1; //was Alt pressed?
            bool modSuper:1; //was any of the Super keys pressed? (the 'windows' key)
        };
        
        std::array<KeyData, static_cast<int>(Action::Count)> mKeymapps;


        /// @brief CURRENT position of the player (or player's camera)
        glm::dvec3 mPosition = {};

        /// @brief CURRENT orientation of the player (or player's camera)
        ///
        /// .x is yaw, .y is pitch
        glm::dvec2 mOrientation = {};

        /// @brief offset for the players position
        ///
        /// gets added onto the Current position with an update()
        glm::dvec3 mPositionDelta = {};

        /// @brief offset for the players orientation
        ///
        /// .x is yaw, .y is pitch
        /// 
        /// gets added onto the Current position with an update()
        glm::dvec2 mOrientationDelta = {};

        glm::dvec3 mVectorFront = {};
        glm::dvec3 mVectorUp = {};
        glm::dvec3 mVectorRight = {};

        double mMouseSenstitivity = 0.1;
        double mFreeCamSpeed = 10.0;

        /// @brief ID of the keyboard press state change event listener
        lp::EventListenerID mKeyboardListenerID = 0;
        /// @brief ID of cursor/mouse motion keyboard listener
        lp::EventListenerID mMouseListenerID = 0;


        /// @brief private function to make sure the Yaw/Pitch values are in bounds
        void checkConstraints(); 

        /// @brief privaye function to update mVectorFront, mVectorRight, mVectorUp
        void updateVectors();

        /// @brief sets default values for the key mapping & sensitivity
        void loadDefaultConfig();
    };
}

#endif //LABYRINTH_PENDING_PLAYER_HPP