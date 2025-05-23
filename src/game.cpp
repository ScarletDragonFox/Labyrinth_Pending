#include <Labyrinth/game.hpp>
#include <Labyrinth/engine.hpp>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui.h>

#include <soloud.h>
#include <soloud_speech.h>
#include <soloud_thread.h>
#include <soloud_wav.h>

#include <glad/gl.h>

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include "Labyrinth/Helpers/compilerErrors.hpp"

#include <GLFW/glfw3.h>


#include "Labyrinth/Engine/ComponentLight.hpp"
#include "Labyrinth/Engine/ComponentPosition.hpp"

#include "Labyrinth/Engine/Resource/resourceManager.hpp"
#include "Labyrinth/Engine/Resource/shaderManager.hpp"

namespace
{
    void opengl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, [[maybe_unused]]  GLsizei length, GLchar const* message, [[maybe_unused]] void const* user_param);


    void int_imgui_shader_window(bool*windowOpened);
}



LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH();

#include <bullet/btBulletDynamicsCommon.h>

LP_PRAGMA_DISABLE_ALL_WARNINGS_POP();

#include "Labyrinth/Engine/ECS/coreECS.hpp"
#include "Labyrinth/Engine/Event/event.hpp"
#include "Labyrinth/Engine/Resource/resourceManager.hpp"

#include "Labyrinth/Engine/Graphics/bullet3Debug.hpp"

namespace lp
{

    bool Game::create()
    {

        if(mWindow.create("Labyrinth Pending", 640, 480))
        {
            return true;
        }
        
        if(lp::g_engine.initialize())
        {
            std::cerr << "lp::Engine failed to initialize!\n";
            return true;
        }

        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
        glDebugMessageCallback(opengl_message_callback, nullptr);
       
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        mRenndd.setup(640, 480);

        auto& ecsr = g_engine.getECS();

        ecsr.registerComponent<lp::ComponentLight>();
        ecsr.registerComponent<lp::ComponentPosition>();
        mLightSystem = ecsr.registerSystem<lp::LightSystem>(ecsr.getComponentSignature<lp::ComponentLight>());
        mLightSystem->update();
        for(int i = 0; i < 10; i++)
        {
            lp::ecs::Entity ent = ecsr.createEntity();
            lp::ComponentLight light;
            light.setRadius((rand() %20'000) /1000.0f);
            ecsr.addComponent<lp::ComponentLight>(ent, light);
        }
        
        return false;
    }

    void Game::loop()
    {
        //https://stackoverflow.com/questions/21421074/how-to-create-a-full-screen-window-on-the-current-monitor-with-glfw
        //https://github.com/glfw/glfw/issues/1699

        // SoLoud::Speech speech;
        // speech.setParams(2000U, 9.0f, 0.5f, 20);

        // struct FF
        // {
        //     int id = 0;
        //     SoLoud::Wav waav;
        //     SoLoud::handle handle;
        //     std::string window_name = "Default";
        // }; 
        // static int id_cunter = 1;
        // std::vector<FF*> songs_loaded;

        lp::gl::Bullet3Debug bulletDebugRenderer;

        // Build the broadphase
        std::unique_ptr<btBroadphaseInterface> broadphase = std::make_unique<btDbvtBroadphase>();
    
        // Set up the collision configuration and dispatcher
        std::unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
        std::unique_ptr<btCollisionDispatcher> dispatcher = std::make_unique<btCollisionDispatcher>(collisionConfiguration.get());
    
        // The actual physics solver
        std::unique_ptr<btSequentialImpulseConstraintSolver> solver = std::make_unique<btSequentialImpulseConstraintSolver>();
     
        // The world.
        std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(dispatcher.get(), broadphase.get(), solver.get(), collisionConfiguration.get()); //this is what crashes. in the destructor it tries to free some stuff, and ... (std::unique_ptr)
        dynamicsWorld->setDebugDrawer(&bulletDebugRenderer);
        
        dynamicsWorld->setGravity(btVector3(0,-10,0));
    
        std::unique_ptr<btRigidBody> groundRigidBody;
        std::unique_ptr<btRigidBody> fallRigidBody;
        std::unique_ptr<btCollisionShape> groundShape = std::make_unique<btStaticPlaneShape>(btVector3(0,1,0), 1);
        std::unique_ptr<btCollisionShape> fallShape = std::make_unique<btSphereShape>((btScalar)1.0f);

       // {
            std::unique_ptr<btDefaultMotionState> groundMotionState = std::make_unique<btDefaultMotionState>(btTransform(btQuaternion(0,0,0,1), btVector3(0,-1,0)));
    
            btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState.get(), groundShape.get(), btVector3(0,0,0));
        
            groundRigidBody =  std::make_unique<btRigidBody>(groundRigidBodyCI);
       // }
        
    
        dynamicsWorld->addRigidBody(groundRigidBody.get());
    
        
       // {
            std::unique_ptr<btDefaultMotionState> fallMotionState = std::make_unique<btDefaultMotionState>(btTransform(btQuaternion(0,0,0,1), btVector3(0,500,0)));
    
            btScalar mass = 1;
            btVector3 fallInertia(0,0,0);
            fallShape->calculateLocalInertia(mass,fallInertia);
        
            btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState.get() ,fallShape.get(), fallInertia);
            fallRigidBody = std::make_unique<btRigidBody>(fallRigidBodyCI);

      //  }
        
         dynamicsWorld->addRigidBody(fallRigidBody.get());
    
        // for (int i=0 ; i<3000 ; i++) {
        //     dynamicsWorld->stepSimulation(deltaTime, 100);
        //     btTransform trans;
        //     fallRigidBody->getMotionState()->getWorldTransform(trans);
        //     std::cout << "sphere " << i <<" height: " << trans.getOrigin().getY() << "\n";
        //     if(glm::distance(trans.getOrigin().getY(), (btScalar)1) < 0.0001f)
        //     {
        //         break;
        //     }
        // }   

        // https://www.youtube.com/watch?app=desktop&v=BGAwRKPlpCw&t=14s
        // https://pybullet.org/Bullet/BulletFull/classbtCollisionShape.html
        //"D:/Semester_3/GK1/DELME/Relic Engine Framework/Assets/backpack/backpack.obj";
        //"C:/Programming/Personal/Solutions/ApplicationTestBuild/AppBuild/Models/Spheres/Spheres.gltf"


        const char* temp_modelNamePath = "C:/Programming/Personal/Solutions/ApplicationTestBuild/AppBuild/Models/SponzaPBR-Intel/Main.1_Sponza/NewSponza_Main_glTF_002.gltf";

        const auto modelRef = g_engine.getResurceManager().loadModel(temp_modelNamePath);
        
        //https://web.archive.org/web/20130419113144/http://bulletphysics.org/mediawiki-1.5.8/index.php/Hello_World
        //https://guibraga.medium.com/my-favorite-visual-studio-code-extensions-11573442008b

        bool IMGUIDoShowDemoWindow = false;
        bool IMGUIDoShowInfoWindow = true;

        bool IMGUIDoShowLightSystemWindow = false;
        bool IMGUIDoShowShaderManagerWindow = false;

        bool mDoPhysics = false;
        double lastFrameTime = glfwGetTime();
        while(!mWindow.shouldClose())
        {
            mWindow.pollEvents();

            const double deltaTime = glfwGetTime() - lastFrameTime;
            lastFrameTime = glfwGetTime();

            mPlayer.update(deltaTime);
            mRenndd.updatePlayer(mPlayer);

            mLightSystem->update();
            
            {
                ImGui::BeginMainMenuBar();
                if(ImGui::BeginMenu("Options"))
                {
                    ImGui::Checkbox("Open Debug ImGui Window", &IMGUIDoShowDemoWindow);
                    ImGui::Checkbox("Open Info Window", &IMGUIDoShowInfoWindow);
                    ImGui::Checkbox("Open Light System Window", &IMGUIDoShowLightSystemWindow);
                    ImGui::Checkbox("Open Shader Manager Window", &IMGUIDoShowShaderManagerWindow);
                    ImGui::Separator();
                    ImGui::Checkbox("Do Physics", &mDoPhysics);
                    // if(ImGui::Button("Reset"))
                    // {
                    //     {
                    //         btTransform transform2 = {};
                    //         const btVector3 pos = btVector3(0, 50, 0);
                    //         transform2.setOrigin(pos);
                    //         fallRigidBody->setWorldTransform(transform2);
                    //         fallRigidBody->getMotionState()->setWorldTransform(transform2);
                    //         fallRigidBody->clearForces();
                    //         fallRigidBody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
                    //         fallRigidBody->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
                    //         fallRigidBody->activate();
                    //     }
                    // }
                    {
                        const auto btPos = fallRigidBody->getWorldTransform().getOrigin();
                        ImGui::Text("Pos: x:%.2f ,y:%.2f ,z:%.2f", btPos.getX(), btPos.getY(), btPos.getZ());
                    }
                    {
                        const auto btPos = fallRigidBody->getCenterOfMassPosition();
                        ImGui::Text("CenterOfMassPositio: x:%.2f ,y:%.2f ,z:%.2f", btPos.getX(), btPos.getY(), btPos.getZ());
                    }
                    {
                        btTransform trans;
                        fallRigidBody->getMotionState()->getWorldTransform(trans);
                        
                        const auto btPos = trans.getOrigin();
                        ImGui::Text("Pos - 2: x:%.2f ,y:%.2f ,z:%.2f", btPos.getX(), btPos.getY(), btPos.getZ()); //Interpolated position
                    }
                        
                    
                    ImGui::EndMenu();
                }
                if(ImGui::BeginMenu("Camera"))
                {
                    float CamSpeed = this->mPlayer.getSpeedRef();
                    float CamMouseSensitivity = this->mPlayer.getMouseSensitivityRef();
                    float CamFoV = glm::degrees(this->mPlayer.getFoVRef());
                    
                    if(ImGui::SliderFloat("Speed", &CamSpeed, 0.01f, 200.0f))
                    {
                        this->mPlayer.getSpeedRef() = CamSpeed; //looks cursed
                    }
                    if(ImGui::SliderFloat("Mouse Sensitivity", &CamMouseSensitivity, 0.01f, 5.0f))
                    {
                        this->mPlayer.getMouseSensitivityRef() = CamMouseSensitivity; //also looks cursed
                    }
                    if(ImGui::SliderFloat("FoV", &CamFoV, 5.0f, 170.0f, "%.3f deg"))
                    {
                        this->mPlayer.getFoVRef() = glm::radians(CamFoV);
                    }

                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
                if(IMGUIDoShowDemoWindow) ImGui::ShowDemoWindow(&IMGUIDoShowDemoWindow);
                
                if(IMGUIDoShowInfoWindow)
                {
                    if(ImGui::Begin("Info Window", &IMGUIDoShowInfoWindow))
                    {
                        ImGui::Text("average: %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                        ImGui::Text("deltaTime: %.3f ms", deltaTime * 1000);
                        ImGui::Text("total: %.2f s", glfwGetTime());
                        const glm::vec3 playerPos = mPlayer.getPosition();
                        const glm::vec2 playerOrient = mPlayer.getOrientation();
                        ImGui::Text("Pos: x:%.2f ,y:%.2f ,z:%.2f", playerPos.x, playerPos.y, playerPos.z);
                        ImGui::Text("Orientation: Yaw: %.2f Pitch: %.2f", playerOrient.x, playerOrient.y);
                        
                    }
                    ImGui::End();
                }
                
                if(IMGUIDoShowShaderManagerWindow) int_imgui_shader_window(&IMGUIDoShowShaderManagerWindow);

                if(IMGUIDoShowLightSystemWindow)
                {
                    if(ImGui::Begin("Light System Window", &IMGUIDoShowLightSystemWindow))
                    {
                        int id = 0;
                        ImGui::Text("mAliveLightList.size() = %u", (unsigned int)mLightSystem->mAliveLightList.size());
                        ImGui::Text("mLightBufferEntityMap.size() = %u", (unsigned int)mLightSystem->mLightBufferEntityMap.size());
                        ImGui::Text("mLightCount = %u", (unsigned int)mLightSystem->mLightCount);
                        ImGui::Text("mMaxLightCount = %u", (unsigned int)mLightSystem->mMaxLightCount);
                        if(ImGui::Button("Create New Light"))
                        {
                            lp::ecs::Entity ent = g_engine.getECS().createEntity();
                            lp::ComponentLight light;
                            light.setRadius((rand() %20'000) /1000.0f);
                            light.setColor({(rand() %20'000) /1000.0f, (rand() %20'000) /1000.0f, (rand() %20'000) /1000.0f});
                            g_engine.getECS().addComponent<lp::ComponentLight>(ent, light);
                        }
                        if(ImGui::Button("Kill top light"))
                        {
                            if(this->mLightSystem->mAliveLightList.size() > 1)
                            {
                                const lp::ecs::Entity ent = this->mLightSystem->mLightBufferEntityMap[this->mLightSystem->mAliveLightList[0]];
                                g_engine.getECS().destroyEntity(ent);
                            }
                        }
                        if(ImGui::TreeNode("mLightSystem->mAliveLightList"))
                        {
                            for(const auto iref: this->mLightSystem->mAliveLightList)
                            {
                                if(ImGui::TreeNode((std::string("light - ") + std::to_string(id++)).c_str()))
                                {
                                    const lp::ecs::Entity lightEnt = this->mLightSystem->mLightBufferEntityMap[iref];
                                    if(g_engine.getECS().hasComponent<lp::ComponentLight>(lightEnt))
                                    {
                                        const auto& light = g_engine.getECS().getComponent<lp::ComponentLight>(lightEnt);
                                    const glm::vec3 l_col = light.getColor();
                                    const float l_radius = light.getRadius();
                                    const glm::vec3 l_pos = light.getPosition();
                                    ImGui::TextColored(ImVec4(l_col.x, l_col.y, l_col.z, 0), "Color of light");
                                    ImGui::Text("radius: %f", l_radius);
                                    ImGui::Text("position: x = %f, y = %f, z = %f", l_pos.x, l_pos.y, l_pos.z);
                                    }else
                                    {
                                        ImGui::Text("Not a light");
                                    }
                                    
                                    ImGui::TreePop();
                                }
                            }
                            ImGui::TreePop();
                        }
                        
                        
                    }
                    ImGui::End();
                }

                if(ImGui::Begin("phyy"))
                {
                    static glm::vec3 vIctooor;
                    ImGui::SliderFloat3("Force", &vIctooor.x, -100.0, 100.0);
                    if(ImGui::Button("push"))
                    {
                        fallRigidBody->applyCentralForce({vIctooor.x, vIctooor.y, vIctooor.z});
                        //fallRigidBody->applyCentralPushImpulse({vIctooor.x, vIctooor.y, vIctooor.z});
                        fallRigidBody->activate();
                    }
                }
                ImGui::End();
                 /*
            if(ImGui::Begin("Sound"))
            {
                SoLoud::Soloud &soloud = g_engine.getSoLoud();

                static char speechbuffer[50] = {};
                static char soundpathbuffer[257] = {};
                speechbuffer[49] = '\0'; soundpathbuffer[256] = '\0';

                ImGui::InputText("text to say", speechbuffer, 49);
                if(ImGui::Button("Speak"))
                {
                    speech.setText(speechbuffer);
                    soloud.play(speech);
                }
                ImGui::Separator();
                
                ImGui::InputTextWithHint("path to file to play", "sound file path", soundpathbuffer, 256);
                ImGui::SameLine();
                if(ImGui::Button("Load & Play"))
                {
                    std::filesystem::path file = std::filesystem::path(soundpathbuffer);
                    
                    if(std::filesystem::exists(file))
                    {
                        FF* ssn = new FF; 
                        ssn->id = id_cunter++;
                        std::cout << "Trying to load " << file.generic_string().c_str() << "\n";
                        int resul = ssn->waav.load(file.generic_string().c_str());
                        if(resul == 0)
                        {
                            ssn->handle = soloud.play(ssn->waav);
                            std::cout << "Playing " << file.generic_string() << " @ " << ssn->handle << "\n";
                            
                            ssn->window_name = file.filename().generic_string();
                            ssn->window_name += " ###" + std::to_string(ssn->id);
                            songs_loaded.push_back(ssn);
                            ImGui::OpenPopup(ssn->window_name.c_str());
                        } else 
                        {
                            delete ssn;
                            ssn = nullptr;
                            std::cerr << "Sound could not be loaded(" << resul << ")!\n";
                        }
                    } else std::cerr << "File doesn't exist!\n";
                    
                }
                for(auto& item: songs_loaded)
                {
                    if(ImGui::BeginPopupModal(item->window_name.c_str()))
                    {
                        if (ImGui::Button("End Song & Close"))
                        {
                            soloud.stop(item->handle);
                            item->waav.stop();
                            std::cout << "Before delete!\n";
                            delete(item);
                            std::cout << "After delete!\n";
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }
                }
                
                ImGui::End();
            }*/
            }
            

           // std::cout << "After ImGui!\n";
           g_engine.getResurceManager().getModelLoaderRef().update(1.0/12.0);
         //   std::cout << "After update() of load\n";

            if(mDoPhysics)
            {
                dynamicsWorld->stepSimulation(deltaTime, 100);
                dynamicsWorld->debugDrawWorld();
                bulletDebugRenderer.flushLines();
            }
            
          //  std::cout << "After update() of physics\n";

            double wbh = 0.0;
            {
                int width = 0, height = 0;
                mWindow.debugGetFramebufferSize(width, height);
                wbh = (double)width / (double)height;
                glViewport(0, 0, width, height);
            }
            
            
            gl::DebugRendererData dtttta;
            
            dtttta.mCamProjection = mPlayer.getProjectionMatrix(wbh);
            dtttta.mCamView = mPlayer.getViewMatrix();
            if(bulletDebugRenderer.getBuffer())
            {
                dtttta.drawCount = bulletDebugRenderer.getDrawCount();
                dtttta.VBO = bulletDebugRenderer.getBuffer();
                //std::cout << "VAO: " << VAO_temp << "\n";
                //std::cout << "VBO: " << bulletDebugRenderer.getBuffer() << "\n";
                //std::cout << "drawCount: " << bulletDebugRenderer.getDrawCount() << "\n";
            }

            dtttta.mdl = g_engine.getResurceManager().getLoadedModel(modelRef);
            
            mRenndd.render(dtttta);
          //  std::cout << "After mRenndd.render(dtttta)\n";

            mWindow.swapBuffers();
        }
         dynamicsWorld->removeRigidBody(groundRigidBody.get());
         dynamicsWorld->removeRigidBody(fallRigidBody.get());
    }

    void Game::destroy()
    {
        g_engine.destroy();
        mWindow.destroy();
        throw "exit exception thrown to stop infinite stall!"; //nothing to see here, move along ...
    }
}

namespace
{
    void int_imgui_shader_window(bool* windowOpened)
    {
        if(ImGui::Begin("Shader window", windowOpened))
        {
            auto& shaderMan = lp::g_engine.getResurceManager().getShaderManager();
            if(ImGui::Button("Reload all shaders"))
            {
                shaderMan.reloadAllShaders();
            }
            ImGui::SetItemTooltip("Relads all of the shaders. May take a while!");
            if(ImGui::TreeNode("Regular shaders"))
            {
                auto lamShade = [&shaderMan](lp::gl::ShaderType st, const char* tooltip)
                {
                    ImGui::Text("%s: %u", lp::gl::getName(st).data(), shaderMan.getShaderID(st));
                    ImGui::SetItemTooltip(tooltip);
                    ImGui::SameLine();
                    std::string name = "Reload ##" + std::to_string(static_cast<unsigned int>(st) + 1);
                    if(ImGui::Button(name.c_str()))
                    {
                        shaderMan.reloadShader(st);
                    }
                };
                lamShade(lp::gl::ShaderType::DebugLine, "Shader used for debug bullet physics drawing");
                lamShade(lp::gl::ShaderType::ModelTextured, "Shader right now used for drawing a textured model");
                lamShade(lp::gl::ShaderType::SimpleColor, "Shader that draws everything with a simple colour");
                ImGui::TreePop();
            }
            if(ImGui::TreeNode("Compute shaders"))
            {
                ImGui::Text("There are no compute shaders right now.");
                //lp::gl::ShaderTypeCompute;
                ImGui::TreePop();
            }
            
        }
        ImGui::End();
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
