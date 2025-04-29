#include <Labyrinth/engine.hpp>

#include <glad/gl.h>

#include <iostream>

#include <Labyrinth/Engine/Resource/shaderManager.hpp>

namespace lp
{
    bool Engine::initialize()
    {
        if(mSound.init() != SoLoud::SO_NO_ERROR)
        {
            std::cerr << "Failed to initialize SoLoud\n";
            return true;
        }
        
        if(mResources.initialize())
        {
            std::cerr << "Resource Manager failed to initialize\n";
            return true;
        }

        return false;
    }

    void Engine::destroy()
    {
        mSound.deinit();
    }
  

    Engine g_engine;
}