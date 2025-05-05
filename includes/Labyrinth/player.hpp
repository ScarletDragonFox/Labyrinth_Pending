#ifndef LABYRINTH_PENDING_PLAYER_HPP
#define LABYRINTH_PENDING_PLAYER_HPP
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp> //glm::half_pi

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
        inline void setPosition(const glm::dvec3 cv_NewPos) { mPosition = cv_NewPos;}

        /// @brief set the player's orientation (or the camera's orientation)
        /// @param cv_yaw yaw of the camera
        /// @param cv_pitch pitch of the camera, in degrees from 90 to -90
        inline void setOrientation(const double cv_yaw, const double cv_pitch)
        {
            mOrientation = {cv_yaw, cv_pitch};
            this->checkConstraints();
            this->updateVectors();
        }

        /// @brief calculates & returns the camera's view matrix
        /// @return the camera's view matrix
        glm::mat4 getViewMatrix()const;

        /// @brief  calculates & returns the camera's infinite projection matrix
        /// @param width_by_height width/height of the output framebuffer
        /// @return the camera's infinite projection matrix
        glm::mat4 getProjectionMatrix(double width_by_height)const;

        /// @brief get the players position (or the camera's position)
        /// @return the players position (or the camera's position)
        inline glm::vec3 getPosition() const { return mPosition; }

        /// @brief get the direction the camera is 'looking' at, as a vector
        /// @return vector in question
        inline glm::vec3 getViewDirection() const {return this->mVectorFront;}

        /// @brief get the players orientation (or the camera's orientation)
        ///
        /// .x is yaw, .y is pitch
        /// @return the players orientation (or the camera's orientation)
        inline glm::vec2 getOrientation() const { return mOrientation; }

        /// @brief get the reference to the speed of the camera
        /// @return reference to the speed of the camera
        double& getSpeedRef() { return this->mFreeCamSpeed; }

        /// @brief get the reference to the mouse sensitivity of the camera's motions (rotations)
        /// @return reference to mouse sensitivity
        double& getMouseSensitivityRef() { return this->mMouseSenstitivity; }

        /// @brief Get reference to Camera FoV in radians.
        /// @return reference to Camera FoV in radians
        double& getFoVRef() { return mCameraFoV; }

        /// @brief update the camera, moving it by deltaTime
        /// @param deltaTime time it took to process + render last frame in seconds
        void update(const double deltaTime);

        ~Player();
        private:

        /// @brief Enum class representing all actions requiring a keypress.
        enum class Action: uint_fast8_t
        {
            Forward,
            Back,
            Left,
            Right,
            Jump,
            DisableInput,
            /// @brief Last element. Used as reference Action enum size
            Count
        };

        /// @brief Struct representing all data needed to store 1 keypress.
        ///
        /// Used for the key mapping.
        struct KeyData
        {
            int key = 0; //key being pressed (GLFW_KEY_XXX)
            bool pressed:1; //true for GLFW_PRESS, false for GLFW_RELEASE
            bool modShift:1; //was Shift pressed?
            bool modControl:1; //was Control pressed?
            bool modAlt:1; //was Alt pressed?
            bool modSuper:1; //was any of the Super keys pressed? (the 'windows' key)
        };
        
        /// @brief The Key Mappping of key -> Action
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

        /// @brief Vector(s) used for getViewMatrix()
        ///
        /// As the name implies, points upwards relative to the camera view direction.
        //
        // In other words, this is the Camera 'view' direction vector 
        glm::dvec3 mVectorFront = {};

        /// @brief Vector(s) used for getViewMatrix()
        ///
        /// As the name implies, points upwards relative to the camera
        glm::dvec3 mVectorUp = {};

        /// @brief Vector(s) used for getViewMatrix()
        ///
        /// As the name implies, points to the right side relative to the camera
        glm::dvec3 mVectorRight = {};

        /// @brief Rate at which mouse motions translate to camera movements/
        double mMouseSenstitivity = 0.1;

        /// @brief Speed at which the camera travels at
        double mFreeCamSpeed = 10.0;

        /// @brief Cameras FoV in radians?
        double mCameraFoV = glm::half_pi<double>();

        /// @brief Is input enabled?
        ///
        /// if false, disables input & frees mouse (by sending an event)
        bool mTriggerInput = false;

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