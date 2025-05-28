#include <Labyrinth/engine.hpp>

#include <glad/gl.h>

#include <iostream>

#include <Labyrinth/Engine/Resource/shaderManager.hpp>

#include <soloud.h>
#include <Labyrinth/Engine/ECS/coreECS.hpp>
#include <Labyrinth/Engine/Event/eventManager.hpp>
#include <Labyrinth/Engine/Resource/resourceManager.hpp>
#include <Labyrinth/Engine/Physics/physicsWorld.hpp>

#include "Labyrinth/Engine/ComponentLight.hpp"
#include "Labyrinth/Engine/ComponentPosition.hpp"
#include "Labyrinth/Engine/ComponentPhysics.hpp"
#include "Labyrinth/Engine/ComponentSoundSource.hpp"

namespace lp
{

    Engine::Engine()
    {
        mSoundPtr = std::make_shared<SoLoud::Soloud>();
        mECSPtr = std::make_shared<lp::ecs::CoreECS>();
        mEventsPtr = std::make_shared<lp::EventManager>();
        mResourcesPtr = std::make_shared<lp::ResourceManager>();
        mPhysicsWorldPtr = std::make_shared<lp::ph::PhysicsWorld>();
    }

    bool Engine::initialize()
    {
        mECSPtr->registerComponent<lp::ComponentLight>();
        mECSPtr->registerComponent<lp::ComponentPosition>();
        mECSPtr->registerComponent<lp::ComponentPhysics>();
        mECSPtr->registerComponent<lp::ComponentSoundSource>();

        if(mSoundPtr->init() != SoLoud::SO_NO_ERROR)
        {
            std::cerr << "Failed to initialize SoLoud\n";
            return true;
        }
        
        if(mResourcesPtr->initialize())
        {
            std::cerr << "Resource Manager failed to initialize\n";
            return true;
        }

        if(mSoundMan.initialize("assets/Audio/index.json"))
        {
            std::cerr << "Sound Manager failed to initialize\n";
            return true;
        }

        mPhysicsWorldPtr->initialize();

        return false;
    }

    void Engine::destroy()
    {
        mSoundPtr->deinit();
        mPhysicsWorldPtr->destroy();
        mResourcesPtr->destroy();
    }
  

    Engine g_engine;
}