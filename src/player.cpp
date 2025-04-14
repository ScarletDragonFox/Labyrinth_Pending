#include "Labyrinth/player.hpp"

#include "Labyrinth/engine.hpp"

#include <GLFW/glfw3.h>
#include <cstring>

#include <glm/gtc/quaternion.hpp> //glm::angleAxis, glm::quat

namespace lp
{
    Player::Player()
    {
        auto& eventMan = g_engine.getEventManager();

        this->loadDefaultConfig(); //TODO: get this info from a file!!

        this->mKeyboardListenerID = eventMan.on(lp::EventTypes::WindowKeyAction, [this](Event& v_event) -> void
        {
            const auto check = [](lp::evt::WindowKeyAction left, KeyData right) -> bool
            {
                if(left.scancode != right.scancode) return false;
                if(left.modAlt == right.modAlt && left.modControl == right.modControl && left.modShift == right.modShift && left.modSuper == right.modSuper)
                {
                    return true;
                }
                return false;
            };

            const auto data = v_event.getData<lp::evt::WindowKeyAction>();

            if(check(data, this->mKeymapps[static_cast<int>(Action::Forward)]))
            {
                this->mPositionDelta += glm::dvec3(1, 0, 0);
            }
            if(check(data, this->mKeymapps[static_cast<int>(Action::Back)]))
            {
                this->mPositionDelta += glm::dvec3(-1, 0, 0);
            }
            if(check(data, this->mKeymapps[static_cast<int>(Action::Right)]))
            {
                this->mPositionDelta += glm::dvec3(0, 0, 1);
            }
            if(check(data, this->mKeymapps[static_cast<int>(Action::Left)]))
            {
                this->mPositionDelta += glm::dvec3(0, 0, -1);
            }
            if(check(data, this->mKeymapps[static_cast<int>(Action::Jump)]))
            {
                this->mPositionDelta += glm::dvec3(0, 1, 0); //TODO: jump should 'launch' the player up
            }
        });

        this->mMouseListenerID = eventMan.on(lp::EventTypes::WindowMouseMotion, [this](Event& v_event) -> void
        {
            const auto data = v_event.getData<lp::evt::WindowMouseMotion>();
            this->mOrientationDelta += glm::dvec2(data.deltaX, data.deltaY);
        });
    }

    void Player::update(const double deltaTime)
    {
        if(mOrientationDelta != glm::dvec2(0.0)) //check if the mouse was moved since last update
        {
            mOrientationDelta *= this->mMouseSenstitivity;
            mOrientation += mOrientationDelta;
            this->checkConstraints();
            this->updateVectors();
            mOrientationDelta = {};
        }
        if(mPositionDelta != glm::dvec3(0.0))
        {
            mPositionDelta *= mFreeCamSpeed * deltaTime;
            mPosition += mPositionDelta.x * mVectorFront;
            mPosition += mPositionDelta.y * mVectorUp;
            mPosition += mPositionDelta.z * mVectorRight;
            mPositionDelta = {};
        }
    }

    Player::~Player()
    {
        auto& eventMan = g_engine.getEventManager();
        eventMan.unregister(mMouseListenerID);
        eventMan.unregister(mKeyboardListenerID);
    }

    void Player::checkConstraints()
    {
        //fix pitch
        if (mOrientation.y > 89.9f)
            mOrientation.y = 89.9f;

        if (mOrientation.y < -89.9f)
            mOrientation.y = -89.9f;

        //fix yaw (keep it in bounds by wrapping it)
        if (mOrientation.x > 360.0f)
            mOrientation.x -= 360.0f;

        if (mOrientation.x < -360.0f)
            mOrientation.x += 360.0f;
    }

    void Player::updateVectors()
    {
        constexpr glm::dvec3 ConstexprCameraUp = glm::dvec3(0.0f, 1.0f, 0.0f);
        constexpr glm::dvec3 ConstexprCameraFront = glm::dvec3(0.0f, 0.0f, -1.0f);
        constexpr glm::dvec3 ConstexprCameraRight = glm::dvec3(1.0f, 0.0f, 0.0f);

        glm::dquat qPitch = glm::angleAxis(glm::radians(-this->mOrientationDelta.y), ConstexprCameraRight); //'right'
        glm::dquat qYaw = glm::angleAxis(glm::radians(this->mOrientationDelta.x), ConstexprCameraUp); //'up'
        //glm::quat qRoll = glm::angleAxis(0.0f, -ConstexprCameraFront);//this does not work? //'front'

        glm::dquat orient = normalize(qPitch * qYaw);

        glm::highp_dmat4 cast = glm::mat4_cast(orient);

        glm::dvec3 front = glm::dvec4(ConstexprCameraFront, 1.0) * cast;
        glm::dvec3 right = glm::dvec4(ConstexprCameraRight, 1.0) * cast;
        glm::dvec3 up = glm::dvec4(ConstexprCameraUp, 1.0) * cast;

        this->mVectorFront = glm::normalize(front);
        this->mVectorRight = glm::normalize(right);
        this->mVectorUp = glm::normalize(up);
    }

    void Player::loadDefaultConfig()
    {
        std::memset(&this->mKeymapps[0], 0, this->mKeymapps.size() * sizeof(KeyData));
        {
            auto& val = this->mKeymapps[static_cast<int>(Action::Forward)];
            val.key = GLFW_KEY_W;
            val.scancode = glfwGetKeyScancode(val.key);
        }{
            auto& val = this->mKeymapps[static_cast<int>(Action::Back)];
            val.key = GLFW_KEY_S;
            val.scancode = glfwGetKeyScancode(val.key);
        }{
            auto& val = this->mKeymapps[static_cast<int>(Action::Left)];
            val.key = GLFW_KEY_A;
            val.scancode = glfwGetKeyScancode(val.key);
        }{
            auto& val = this->mKeymapps[static_cast<int>(Action::Right)];
            val.key = GLFW_KEY_D;
            val.scancode = glfwGetKeyScancode(val.key);
        }{
            auto& val = this->mKeymapps[static_cast<int>(Action::Jump)];
            val.key = GLFW_KEY_SPACE;
            val.scancode = glfwGetKeyScancode(val.key);
        }

        mMouseSenstitivity = 0.1; //default value
        mFreeCamSpeed = 10.0;
    }
}