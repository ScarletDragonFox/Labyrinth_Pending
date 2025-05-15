#include <Labyrinth/engine.hpp>

#include <glad/gl.h>

#include <iostream>

#include <Labyrinth/Engine/Resource/shaderManager.hpp>

#include <soloud.h>
#include <Labyrinth/Engine/ECS/coreECS.hpp>
#include <Labyrinth/Engine/Event/eventManager.hpp>
#include <Labyrinth/Engine/Resource/resourceManager.hpp>


namespace lp
{

    Engine::Engine()
    {
        mSoundPtr = std::make_shared<SoLoud::Soloud>();
        mECSPtr = std::make_shared<lp::ecs::CoreECS>();
        mEventsPtr = std::make_shared<lp::EventManager>();
        mResourcesPtr = std::make_shared<lp::ResourceManager>();
    }

    bool Engine::initialize()
    {
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

        return false;
    }

    void Engine::destroy()
    {
        mSoundPtr->deinit();
    }
  

    Engine g_engine;
}