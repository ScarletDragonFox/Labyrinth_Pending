#include <Labyrinth/engine.hpp>

#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>

#include <Labyrinth/Engine/Resource/shaderManager.hpp>

namespace lp
{
    bool Engine::initialize()
    {
        int glversion = gladLoadGL(glfwGetProcAddress);
        if (glversion == 0) {
            std::cerr << "Failed to initialize OpenGL context\n";
            return true;
        }
        //// Successfully loaded OpenGL
        //std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(glversion) << "." << GLAD_VERSION_MINOR(glversion) << "\n";

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