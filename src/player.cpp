#include "Labyrinth/player.hpp"

#include "Labyrinth/engine.hpp"

#include <GLFW/glfw3.h>

LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH();
#include <glm/gtc/quaternion.hpp> //glm::angleAxis, glm::quat

#include <soloud.h>

LP_PRAGMA_DISABLE_ALL_WARNINGS_POP();

namespace
{
    /// @brief internal function to convert from btVector3 -> glm::vec3
    inline glm::vec3 int_to_glm(btVector3& vec)
    {
        return glm::vec3(vec.getX(), vec.getY(), vec.getZ());
    }
}

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


    void Player::createCollision(btDynamicsWorld *dynamicsWorld)
    {
        if(characterController) return;

        // Create the collision shape for the character (a simple capsule)
        shape = new btCapsuleShape(0.5, 1.75); // radius, height
        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(mPosition.x, mPosition.y, mPosition.z));

        // Create motion state (used to store the rigid body position)
        motionState = new btDefaultMotionState(startTransform);

        btVector3 localInertia(0, 0, 0);
        shape->calculateLocalInertia(mass, localInertia);

        // Create the ghost object (used by the kinematic character controller)
        ghostObject = new btPairCachingGhostObject();
        ghostObject->setWorldTransform(startTransform);
        ghostObject->setCollisionShape(shape);
        ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

        

        // Create the character controller object
        characterController = new btKinematicCharacterController(ghostObject, shape, (btScalar)0.35f, btVector3(0.0, 1.0, 0.0));
        characterController->setLinearDamping(0.3);
        
        characterController->setUseGhostSweepTest(true);
       // characterController->setGravity(btVector3(0, -9.8, 0));
        ghostObject->setFriction(0.5);
        // Set initial position and attach it to the world
        ghostObject->setWorldTransform(startTransform);

        dynamicsWorld->addCollisionObject(this->ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
        dynamicsWorld->addAction(this->characterController);
    }

    void Player::destroyCollision(btDynamicsWorld *dynamicsWorld)
    {
        dynamicsWorld->removeAction(this->characterController);
        dynamicsWorld->removeCollisionObject(this->ghostObject);
        

        if(characterController) delete characterController; 
        if(ghostObject) delete ghostObject; 
        if(shape) delete shape; 
        if(motionState) delete motionState; 

        characterController = nullptr;
        ghostObject = nullptr;
        shape = nullptr;
        motionState = nullptr;
    }

    glm::mat4 Player::getViewMatrix()const
    {
        return (glm::mat4)glm::lookAt(this->mCameraPosition, this->mCameraPosition + this->mVectorFront, this->mVectorUp);
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
            static bool free_mouse_IsPressed = true;

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

        bool motionOcurred = false;
        {
            mPositionDelta = {};

            if(this->mKeymapps[static_cast<int>(Action::Forward)].pressed)
            {
                this->mPositionDelta += glm::dvec3(1, 0, 0); motionOcurred = true;
            }
            if(this->mKeymapps[static_cast<int>(Action::Back)].pressed)
            {
                this->mPositionDelta += glm::dvec3(-1, 0, 0); motionOcurred = true;
            }
            if(this->mKeymapps[static_cast<int>(Action::Right)].pressed)
            {
                this->mPositionDelta += glm::dvec3(0, 0, 1); motionOcurred = true;
            }
            if(this->mKeymapps[static_cast<int>(Action::Left)].pressed)
            {
                this->mPositionDelta += glm::dvec3(0, 0, -1); motionOcurred = true;
            }
            if(this->mKeymapps[static_cast<int>(Action::Jump)].pressed)
            {
                if(characterController)
                {
                    if (characterController->onGround()) {
                        characterController->jump(); motionOcurred = true;
                    }
                } 
            }
        }
        

        if(characterController)
        {
            if(motionOcurred)
            {
                characterController->setLinearDamping(0.3);
            }else
            {
                characterController->setLinearDamping(0.8);
            }

            if(motionOcurred)
            {
                //mPosition = glm::normalize(mPositionDelta) * mFreeCamSpeed * deltaTime;
                glm::dvec3 deltaCalct = glm::normalize(mPositionDelta) * mFreeCamSpeed * deltaTime;
                {
                    const glm::bvec3 why = glm::isnan(deltaCalct) || glm::isinf(deltaCalct);
                    if(why.x || why.y || why.z){
                        deltaCalct = {0.0, 0.0, 0.0};
                    }
                }
                const glm::dvec3 problemSolver = deltaCalct.x * mVectorFront + deltaCalct.y * mVectorUp + deltaCalct.z * mVectorRight;
                characterController->setWalkDirection(btVector3(problemSolver.x, problemSolver.y, problemSolver.z));
            }

            mPosition = int_to_glm(ghostObject->getWorldTransform().getOrigin());
            mCameraPosition = mPosition + glm::dvec3(0, 1.5, 0); // Camera above character
        } else
        { //if there is no characterController, fallback to this
            mPositionDelta = glm::normalize(mPositionDelta) * mFreeCamSpeed * deltaTime;
             {
                const glm::bvec3 why = glm::isnan(mPositionDelta) || glm::isinf(mPositionDelta);
                if(why.x || why.y || why.z){
                    mPositionDelta = {0.0, 0.0, 0.0};
                }
            }
            mPosition += mPositionDelta.x * mVectorFront;
            mPosition += mPositionDelta.y * mVectorUp;
            mPosition += mPositionDelta.z * mVectorRight;
            mCameraPosition = mPosition;
        }

        
        
        
        if(mOrientationDelta != glm::dvec2(0.0)) //check if the mouse was moved since last update
        {
            mOrientationDelta *= this->mMouseSenstitivity;
            mOrientation += mOrientationDelta;
            this->checkConstraints();
            this->updateVectors();
            mOrientationDelta = {};
        }
        if(motionOcurred)
        {
            if(characterController)
            {
                const auto velo = characterController->getLinearVelocity();
                lp::g_engine.getSoLoud().set3dListenerVelocity(velo.getX(), velo.getY(), velo.getZ());
            } else{
                lp::g_engine.getSoLoud().set3dListenerVelocity(mPositionDelta.x, mPositionDelta.y, mPositionDelta.z);
            }
             
            lp::g_engine.getSoLoud().set3dListenerPosition(mPosition.x, mPosition.y, mPosition.z);
            
        }
    }

    Player::~Player()
    {
        auto& eventMan = g_engine.getEventManager();
        eventMan.unregister(mMouseListenerID);
        eventMan.unregister(mKeyboardListenerID);

        if(characterController) delete characterController;
        if(ghostObject) delete ghostObject;
        if(shape) delete shape;
        if(motionState) delete motionState;
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

        lp::g_engine.getSoLoud().set3dListenerUp(this->mVectorUp.x, this->mVectorUp.y, this->mVectorUp.z);
        lp::g_engine.getSoLoud().set3dListenerAt(this->mVectorFront.x, this->mVectorFront.y, this->mVectorFront.z);
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