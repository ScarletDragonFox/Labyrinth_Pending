#include <iostream>
#include <memory>
#include <vector>
#include <string>

#include "Labyrinth/Helpers/compilerErrors.hpp"
#include "Labyrinth/engine.hpp"
#include "Labyrinth/window.hpp"
#include "Labyrinth/Engine/Graphics/bullet3Debug.hpp"
#include "Labyrinth/Engine/Graphics/regularShader.hpp"
#include "Labyrinth/player.hpp"

LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH();
#include <imgui.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <bullet/btBulletDynamicsCommon.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

LP_PRAGMA_DISABLE_ALL_WARNINGS_POP();

#include "BulletShape.hpp"

namespace
{
    void opengl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, [[maybe_unused]]  GLsizei length, GLchar const* message, [[maybe_unused]] void const* user_param);

    btCollisionWorld::ClosestRayResultCallback RayTestObtain(btDynamicsWorld* v_world, const lp::Player& cr_Player, lp::Window& r_window, const glm::vec2 cv_mousePos);
}

#include "RigidBodyConteiner.hpp"

struct RendererForwardPlus_PlayerData
{
    glm::mat4 mView = {};
    glm::mat4 mProjection = {}; //infinite projection
    glm::vec3 mPosition = {};
    float padding = 0.0f;
};


int main()
{
    lp::Window window;
    if(window.create("Model BV Creator", 640, 480))
    {
        std::cerr << "ERROR: Window couldn't be created\n";
        return -1;
    }

    if(lp::g_engine.initialize())
    {
        std::cerr << "ERROR: Rngine initialization failed!\n";
        return -2;
    }

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
    glDebugMessageCallback(opengl_message_callback, nullptr);
   
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    lp::Player mPlayer;
    

    lp::gl::Bullet3Debug bulletDebugRenderer;

     // Build the broadphase
     std::unique_ptr<btBroadphaseInterface> broadphase = std::make_unique<btDbvtBroadphase>();
    
     // Set up the collision configuration and dispatcher
     std::unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
     std::unique_ptr<btCollisionDispatcher> dispatcher = std::make_unique<btCollisionDispatcher>(collisionConfiguration.get());
 
     // The actual physics solver
     std::unique_ptr<btSequentialImpulseConstraintSolver> solver = std::make_unique<btSequentialImpulseConstraintSolver>();
  
     // The world.
     std::shared_ptr<btDiscreteDynamicsWorld> dynamicsWorld = std::make_shared<btDiscreteDynamicsWorld>(dispatcher.get(), broadphase.get(), solver.get(), collisionConfiguration.get());
     dynamicsWorld->setDebugDrawer(&bulletDebugRenderer);
     
     dynamicsWorld->setGravity(btVector3(0, 0, 0));
 
    std::unique_ptr<btRigidBody> vPhysicsBody;
    btCollisionShape* vPhysicsShape = new btSphereShape(1.0); //raw ptr for simplicity

    {
        std::unique_ptr<btDefaultMotionState> groundMotionState = std::make_unique<btDefaultMotionState>();
 
        btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState.get(), vPhysicsShape, btVector3(0,0,0));
     
        vPhysicsBody =  std::make_unique<btRigidBody>(groundRigidBodyCI);
    }
    
    vPhysicsBody->setCollisionShape(vPhysicsShape); //use this???

    vPhysicsBody->setMassProps(0.0, btVector3(0.0, 0.0, 0.0));

    dynamicsWorld->addRigidBody(vPhysicsBody.get());


    GLuint VAO_model = 0;
        {
            glCreateVertexArrays(1, &VAO_model);
            glEnableVertexArrayAttrib(VAO_model, 0);
            glEnableVertexArrayAttrib(VAO_model, 1);
            glVertexArrayAttribFormat(VAO_model, 0, 3, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribFormat(VAO_model, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
            glVertexArrayAttribBinding(VAO_model, 0, 0);
            glVertexArrayAttribBinding(VAO_model, 1, 0);
        }

    lpt::RigidBosyContainerCreationClassThing RBCCCT((std::shared_ptr<btDynamicsWorld>)dynamicsWorld);

    RendererForwardPlus_PlayerData playerData;

    GLuint UBOplayerDataBuffer = 0;
    glCreateBuffers(1, &UBOplayerDataBuffer);
    glNamedBufferStorage(UBOplayerDataBuffer, sizeof(RendererForwardPlus_PlayerData), nullptr, GL_DYNAMIC_STORAGE_BIT); 

    bool IMGUI_ShowDemoWindow = false;
    bool IMGUI_ShowRigidBosyContainerWindow = false;
    double lastFrameTime = glfwGetTime();
    while(!window.shouldClose())
    {
        window.pollEvents();

        const double deltaTime = glfwGetTime() - lastFrameTime;
        lastFrameTime = glfwGetTime();
        mPlayer.update(deltaTime);
        {
            playerData.mPosition = mPlayer.getPosition();
            playerData.mView = mPlayer.getViewMatrix();
            int width = 0, height = 0;
            window.debugGetFramebufferSize(width, height);
            glViewport(0, 0, width, height);
            playerData.mProjection = mPlayer.getProjectionMatrix((double)width / (double)height);

            glNamedBufferSubData(UBOplayerDataBuffer, 0, sizeof(RendererForwardPlus_PlayerData), &playerData);
        }

        ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

        {
            ImGui::BeginMainMenuBar();
            if(ImGui::BeginMenu("File"))
            {
                ImGui::BeginDisabled();
                if(ImGui::Button("Save"))
                {
                }
                ImGui::SetItemTooltip("Save the current project");
                if(ImGui::Button("Load"))
                {
                }
                ImGui::SetItemTooltip("Load the project from a file");
                if(ImGui::Button("Import"))
                {
                }
                ImGui::SetItemTooltip("Import a model from a file");
                if(ImGui::Button("Export"))
                {
                }
                ImGui::SetItemTooltip("Export current model");
                ImGui::EndDisabled();
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Windows"))
            {
                ImGui::Checkbox("Demo Window", &IMGUI_ShowDemoWindow);
                ImGui::Checkbox("RigidBody Container ? Window", &IMGUI_ShowRigidBosyContainerWindow);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
        

        if(ImGui::Begin("Basics"))
        {
            {
                const btVector3 scale = vPhysicsShape->getLocalScaling();
                float scale_v[3] = {scale.getX(), scale.getY(), scale.getZ()};
                if(ImGui::SliderFloat3("Scale", scale_v, 0.01f, 10.0f))
                {
                    vPhysicsShape->setLocalScaling(btVector3(scale_v[0], scale_v[1], scale_v[2]));
                }

                ImGui::Text("Mouse: x = %f, y = %f", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
                {
                    int height = 0, width = 0;
                    window.getWindowSize(width, height);
                    ImGui::Text("Window: x = %d, y = %d", width, height);
                }
               
                //ImGui::GetIO().MouseClicked[0]

            }
            //vPhysicsBody->getCenterOfMassPosition(); //TODO: make a move() func for a given btRigidBody (teleport, maybe interpolation as separate?)
            ImGui::Text("Name: \"%s\"", vPhysicsShape->getName());
        }
        ImGui::End();

        if(IMGUI_ShowDemoWindow) ImGui::ShowDemoWindow(&IMGUI_ShowDemoWindow);

        if(IMGUI_ShowRigidBosyContainerWindow)
        {
            if(ImGui::Begin("RigidBody Container ?", &IMGUI_ShowRigidBosyContainerWindow))
            {
                RBCCCT.drawUI();
            }
            ImGui::End();
        }
        

        // @see https://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-a-physics-library/
        //if imgui doesn't want the mouse AND left mouse button was pressed
        if(!ImGui::GetIO().WantCaptureMouse && ImGui::GetIO().MouseClicked[0]) 
        {
            auto RayCallback = RayTestObtain(dynamicsWorld.get(), mPlayer, window, glm::vec2((float)ImGui::GetIO().MousePos.x, (float)ImGui::GetIO().MousePos.y));
            if(RayCallback.hasHit())
            {
                std::cout << "BANG!\n";
            } else
            {
                std::cout << "Wooosh!\n";
            }   
        }




        // make function/window TO:
            // - create rigidbody (shape) - all the usable ones
            // - have a window for vPhysicsShape - specific settings (for all usable shapes)
            // - have a window for all vPhysicsBody - important settings (IMPORTANT ONES!!!)

        // make a menu to load a model & display it
        // make a menu to import/export files

        dynamicsWorld->debugDrawWorld();
        dynamicsWorld->stepSimulation(deltaTime);

        glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE); //set clipping plane to [0,1], instead of the default [-1,1] 
        glDepthFunc(GL_GREATER); //these 3 reverse the depth buffer
        glClearDepth(0); //clear depth to 0
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //sets the clear colour to black (red)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBOplayerDataBuffer);

        if(bulletDebugRenderer.getBuffer() != 0)
        {
            lp::gl::RegularShader shader;
            shader.LoadShader(lp::gl::ShaderType::DebugLine);
            shader.Use();
            shader.SetUniform(3, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));

            glBindVertexArray(VAO_model);
            glVertexArrayVertexBuffer(VAO_model, 0, bulletDebugRenderer.getBuffer(), 0, 6 * sizeof(float));
            glDrawArrays(GL_LINES, 0, bulletDebugRenderer.getDrawCount());
            glBindVertexArray(0);
        }
       
        window.swapBuffers();
    }

    lp::g_engine.destroy();
    window.destroy();
}
    /**
     * TODO: eventually add fullscreen functionality in Window
     * 
     * TODO: functionality of THIS:
     *  - ONE model at a time
     *  - can load any model from file with assimp
     *  - simplest possible graphics
     *  - UI with ImGui
     *  - ONE BoundingVolume class type at a time 
     *  - choose with dropdown?
     *  - than functions sepcific to this wil appear
     *  - ways to TIE physics <-> graphics
     *  - SAVE to file
     *  - LOAD from file (removing all prior data)
     *  - try to avoid globals so we can maybe have this in game proper?
     * 
     * TODO: functions of ALL TOOLS:
     *  - save/load models/physics/sounds to/from file
     *  - every model like this will be a .json file with refs to .bullet + .glfw + (sound like .ogg)
     *  - a way to create:
     *  - STATIC geometry for levels
     *  - DYNAMIC (scripted wit lua) stuff for levels (like gates/doors, buttons, levers)
     *  
     * 
     *  - Figure out some kind of interpolation system for anims (NOT skeletal, 'root' movement only)
     * 
     * 
     * TODO: 
     *  - create new (.gitignore'd) bin/ directory
     *  https://stackoverflow.com/questions/73763985/set-cmake-output-executable-directory
     *  https://stackoverflow.com/questions/543203/cmake-runtime-output-directory-on-windows (comment by hgyxbll)
     *  https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#build-configurations
     *  https://cfd.university/learn/automating-cfd-solver-and-library-compilation-using-cmake/advanced-cmake-features-to-power-up-your-cfd-development/   <- cmake pseudo tutorial (ctest, cpack, etc...)
     * 
     * https://cfd.university/learn/keep-your-users-happy-how-to-document-code-the-right-way/documenting-code-why-bother-and-how-to-do-it-right/
     * 
     *  Set LIBRARY_OUTPUT_DIRECTORY directory in preset?
     *  I want to be able to run my program by running:
     *  ./bin/Release/ OR ./bin/Debug/ ??? or just one for both?
     *  have them be separate, better
     *  Make this work with Visual Studio!!!
     * CMakePresets.json release-base is broken +
     * CMAKE_INSTALL_PREFIX <- outdated, + what is this???
     */

namespace
{
    btCollisionWorld::ClosestRayResultCallback RayTestObtain(btDynamicsWorld* v_world, const lp::Player& cr_Player, lp::Window& r_window, const glm::vec2 cv_mousePos)
    {
        int height = 0, width = 0;
        r_window.getWindowSize(width, height);

        // The ray Start and End positions, in Normalized Device Coordinates (Have you read Tutorial 4 ?)
        glm::vec4 lRayStart_NDC(
            (cv_mousePos.x/(float)width  - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
            (cv_mousePos.y/(float)height - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
            -1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
            1.0f
        );
        glm::vec4 lRayEnd_NDC(
            (cv_mousePos.x/(float)width  - 0.5f) * 2.0f,
            (cv_mousePos.y/(float)height - 0.5f) * 2.0f,
            0.001, //was 0.0, but caused lRayEnd_world.w == 0, which caused division by 0, this fixes it
            1.0f
        );
        glm::mat4 InverseProjectionMatrix = {};
        {
            int FRAM_width = 0, FRAM_height = 0;
            r_window.debugGetFramebufferSize(FRAM_width, FRAM_height);
            InverseProjectionMatrix = cr_Player.getProjectionMatrix((double)FRAM_width / (double)FRAM_height);

            // The Projection matrix goes from Camera Space to NDC.
            // So inverse(ProjectionMatrix) goes from NDC to Camera Space.
            // InverseProjectionMatrix = glm::inverse(InverseProjectionMatrix);
        }
        // The View Matrix goes from World Space to Camera Space.
        // So inverse(ViewMatrix) goes from Camera Space to World Space.
        //glm::mat4 InverseViewMatrix = glm::inverse(mPlayer.getViewMatrix());

        // glm::vec4 lRayStart_camera = InverseProjectionMatrix * lRayStart_NDC;    lRayStart_camera/=lRayStart_camera.w;
        // glm::vec4 lRayStart_world  = InverseViewMatrix       * lRayStart_camera; lRayStart_world /=lRayStart_world .w;
        // glm::vec4 lRayEnd_camera   = InverseProjectionMatrix * lRayEnd_NDC;      lRayEnd_camera  /=lRayEnd_camera  .w;
        // glm::vec4 lRayEnd_world    = InverseViewMatrix       * lRayEnd_camera;   lRayEnd_world   /=lRayEnd_world   .w;

        // Faster way (just one inverse)
        glm::mat4 M = glm::inverse(InverseProjectionMatrix * cr_Player.getViewMatrix());
        glm::vec4 lRayStart_world = M * lRayStart_NDC; lRayStart_world/=lRayStart_world.w;
        
        glm::vec4 lRayEnd_world   = M * lRayEnd_NDC;
        //std::cout << "lRayEnd_world - before division - x = " << lRayEnd_world.x << " y = " << lRayEnd_world.y << " z = " << lRayEnd_world.z << " w = " << lRayEnd_world.w << "\n"; 
        //if(lRayEnd_world.w != 0.0f)
        lRayEnd_world  /=lRayEnd_world.w;

        //std::cout << "lRayEnd_world x = " << lRayEnd_world.x << " y = " << lRayEnd_world.y << " z = " << lRayEnd_world.z << " w = " << lRayEnd_world.w << "\n"; 
        //std::cout << "lRayEnd_NDC x = " << lRayEnd_NDC.x << " y = " << lRayEnd_NDC.y << " z = " << lRayEnd_NDC.z << " w = " << lRayEnd_NDC.w << "\n"; 

        glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
        lRayDir_world = glm::normalize(lRayDir_world);

        glm::vec3 out_origin = lRayStart_world; 
        glm::vec3 out_direction = glm::normalize(lRayDir_world);

        //glm::vec3 out_origin = cr_Player.getPosition(), out_direction = glm::normalize(cr_Player.getViewDirection()); //works, but only for 'middle' of screen, mouse is not accounted for

        glm::vec3 out_end = out_origin + out_direction*1000.0f;

        //std::cout << "From x = " << out_origin.x << " y = " << out_origin.y << " z = " << out_origin.z << "\n";
        //std::cout << "To x = " << out_end.x << " y = " << out_end.y << " z = " << out_end.z << "\n"; 

        btCollisionWorld::ClosestRayResultCallback RayCallback(
            btVector3(out_origin.x, out_origin.y, out_origin.z), 
            btVector3(out_end.x, out_end.y, out_end.z)
        );

        v_world->rayTest(
            btVector3(out_origin.x, out_origin.y, out_origin.z), 
            btVector3(out_end.x, out_end.y, out_end.z), 
            RayCallback
        );
        return RayCallback;
    }


    void opengl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, [[maybe_unused]]  GLsizei length, GLchar const* message, [[maybe_unused]] void const* user_param)
    {
        auto const src_str = [source]() {
            switch (source)
            {
            case GL_DEBUG_SOURCE_API: return "API";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
            case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
            case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
            case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
            case GL_DEBUG_SOURCE_OTHER: return "OTHER";
            default: return "BAD SOURCE";
            }
            }();

        auto const type_str = [type]() {
            switch (type)
            {
            case GL_DEBUG_TYPE_ERROR: return "ERROR";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
            case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
            case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
            case GL_DEBUG_TYPE_MARKER: return "MARKER";
            case GL_DEBUG_TYPE_OTHER: return "OTHER";
            default: return "BAD ERROR TYPE";
            }
            }();

        auto const severity_str = [severity]() {
            switch (severity)
            {
            case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
            case GL_DEBUG_SEVERITY_LOW: return "LOW";
            case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
            case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
            default: return "BAD SEVERITY";
            }
            }();

        std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id << ": " << message << "\n";

    }
}