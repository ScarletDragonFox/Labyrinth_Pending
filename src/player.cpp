#include "Labyrinth/player.hpp"

#include "Labyrinth/engine.hpp"

#include <GLFW/glfw3.h>

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
                if(left.key != right.key) return false;
                if(left.modAlt == right.modAlt && left.modControl == right.modControl && left.modShift == right.modShift && left.modSuper == right.modSuper)
                {
                   return true;
                }
                return false;
            };

            const auto data = v_event.getData<lp::evt::WindowKeyAction>();

            for(auto&i: this->mKeymapps)
            {
                if(check(data, i))
                {
                    i.pressed = data.pressed;
                    return;
                }
            }
        });

        this->mMouseListenerID = eventMan.on(lp::EventTypes::WindowMouseMotion, [this](Event& v_event) -> void
        {
            const auto data = v_event.getData<lp::evt::WindowMouseMotion>();
            this->mOrientationDelta += glm::dvec2(data.deltaX, data.deltaY);
        });
    }

    glm::mat4 Player::getViewMatrix()const
    {
        return (glm::mat4)glm::lookAt(this->mPosition, this->mPosition + this->mVectorFront, this->mVectorUp);
    }

    namespace{
        inline glm::mat4 MakeInfReversedZProjRH(float fovY_radians, float aspectWbyH, float zNear)
        {
            const float f = 1.0f / tan(fovY_radians / 2.0f);
            return glm::mat4(
                f / aspectWbyH, 0.0f, 0.0f, 0.0f,
                0.0f, f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, -1.0f,
                0.0f, 0.0f, zNear, 0.0f);
        }
    }

    glm::mat4 Player::getProjectionMatrix(double width_by_height)const
    {
        return MakeInfReversedZProjRH(mCameraFoV, (float)width_by_height, 0.1f);
    }

    void Player::update(const double deltaTime)
    {
        {
            const bool lockPressed = this->mKeymapps[static_cast<int>(Action::DisableInput)].pressed;
            static bool free_mouse_IsPressed = false;

            if(!free_mouse_IsPressed){
                if(lockPressed)
                {
                    free_mouse_IsPressed = true;
                    mTriggerInput = !mTriggerInput;

                    lp::Event evt(lp::EventTypes::PlayerTriggerInputs, mTriggerInput);
                    g_engine.getEventManager().emit(evt);
                }
            }else{
                if(!lockPressed)
                {
                    free_mouse_IsPressed = false;
                }
            }
        }


        if(!mTriggerInput) return;

        {
            if(this->mKeymapps[static_cast<int>(Action::Forward)].pressed)
            {
                this->mPositionDelta += glm::dvec3(1, 0, 0);
            }
            if(this->mKeymapps[static_cast<int>(Action::Back)].pressed)
            {
                this->mPositionDelta += glm::dvec3(-1, 0, 0);
            }
            if(this->mKeymapps[static_cast<int>(Action::Right)].pressed)
            {
                this->mPositionDelta += glm::dvec3(0, 0, 1);
            }
            if(this->mKeymapps[static_cast<int>(Action::Left)].pressed)
            {
                this->mPositionDelta += glm::dvec3(0, 0, -1);
            }
            if(this->mKeymapps[static_cast<int>(Action::Jump)].pressed)
            {
                this->mPositionDelta += glm::dvec3(0, 1, 0); //TODO: jump should 'launch' the player up
            }
        }

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
            mPositionDelta = glm::normalize(mPositionDelta) * mFreeCamSpeed * deltaTime;
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

        glm::dquat qPitch = glm::angleAxis(glm::radians(-this->mOrientation.y), ConstexprCameraRight); //'right'
        glm::dquat qYaw = glm::angleAxis(glm::radians(this->mOrientation.x), ConstexprCameraUp); //'up'
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
        this->mKeymapps.fill({});
        {
            auto& val = this->mKeymapps[static_cast<int>(Action::Forward)];
            val.key = GLFW_KEY_W;
        }{
            auto& val = this->mKeymapps[static_cast<int>(Action::Back)];
            val.key = GLFW_KEY_S;
        }{
            auto& val = this->mKeymapps[static_cast<int>(Action::Left)];
            val.key = GLFW_KEY_A;
        }{
            auto& val = this->mKeymapps[static_cast<int>(Action::Right)];
            val.key = GLFW_KEY_D;
        }{
            auto& val = this->mKeymapps[static_cast<int>(Action::Jump)];
            val.key = GLFW_KEY_SPACE;
        }{
            auto& val = this->mKeymapps[static_cast<int>(Action::DisableInput)];
            val.key = GLFW_KEY_F1;
        }

        mMouseSenstitivity = 0.1; //default value
        mFreeCamSpeed = 10.0;
    }
}