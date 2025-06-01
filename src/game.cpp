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

#include <stb_image.h>

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include "Labyrinth/Helpers/compilerErrors.hpp"

#include <GLFW/glfw3.h>


#include "Labyrinth/Engine/ComponentLight.hpp"
#include "Labyrinth/Engine/ComponentPosition.hpp"
#include "Labyrinth/Engine/ComponentPhysics.hpp"
#include "Labyrinth/Engine/ComponentSoundSource.hpp"

#include "Labyrinth/Engine/Resource/resourceManager.hpp"
#include "Labyrinth/Engine/Resource/shaderManager.hpp"

namespace
{
    void opengl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, [[maybe_unused]]  GLsizei length, GLchar const* message, [[maybe_unused]] void const* user_param);


    void int_imgui_shader_window(bool*windowOpened);

    void int_imgui_all_entities_window(bool* windowOpen);
}



LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH();

#include <bullet/btBulletDynamicsCommon.h>

LP_PRAGMA_DISABLE_ALL_WARNINGS_POP();

#include "Labyrinth/Engine/ECS/coreECS.hpp"
#include "Labyrinth/Engine/Event/event.hpp"
#include "Labyrinth/Engine/Resource/resourceManager.hpp"

#include "Labyrinth/Engine/Graphics/bullet3Debug.hpp"

#include "Labyrinth/Engine/Graphics/graphicsPrepareSystem.hpp"

#include "Labyrinth/Engine/Physics/physicsWorld.hpp"



#include <Labyrinth/Engine/Resource/soundManager.hpp>

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

        mLightSystem = ecsr.registerSystem<lp::LightSystem>(ecsr.getComponentSignature<lp::ComponentLight>());
        mLightSystem->update();
        for(int i = 0; i < 10; i++)
        {
            lp::ecs::Entity ent = ecsr.createEntity();
            lp::ComponentLight light;
            light.setRadius((rand() %20'000) /1000.0f);
            light.setColor(glm::vec3((rand() %20'000) /20000.0f, (rand() %20'000) /20000.0f, (rand() %20'000) /20000.0f));
            light.setPosition(glm::vec3((rand() %20'000) /100.0f, (rand() %20'000) /100.0f, (rand() %20'000) /100.0f) - glm::vec3(100));
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
        {
            lp::ecs::Signature olf = g_engine.getECS().getComponentEventListenablesSignature();
            olf |= g_engine.getECS().getComponentSignature<lp::ComponentPhysics>();
            g_engine.getECS().setComponentEventListenablesSignature(olf);     
        }
        

        btDiscreteDynamicsWorld* dynamicsWorld = g_engine.getPhysicsWorld().getWorld();
        
        dynamicsWorld->setGravity(btVector3(0,-10,0));
    

        lp::ph::ColliderID_t fallSphere_collider = lp::ph::const_collider__id_invalid;
        lp::ecs::Entity allSphere_entity = lp::ecs::const_entity_invalid;
        {
            auto& Recs = g_engine.getECS();
            allSphere_entity = Recs.createEntity();
            lp::ComponentPhysics phy;
            btCollisionShape* csfallSphere = new btSphereShape(1.0);
            fallSphere_collider = g_engine.getPhysicsWorld().registerCollisionShape(csfallSphere);
            const btScalar mass = 1; btVector3 fallInertia(0,0,0);
            csfallSphere->calculateLocalInertia(mass, fallInertia);
            phy.mState = std::make_shared<btDefaultMotionState>(btTransform(btQuaternion(0,0,0,1), btVector3(0,500,0)));
            btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(mass, phy.mState.get(), csfallSphere, fallInertia);
            phy.mRigidBody = std::make_shared<btRigidBody>(groundRigidBodyCI);
            Recs.addComponent(allSphere_entity, phy);
        }
        
        
        lp::ph::ColliderID_t floor_collider = lp::ph::const_collider__id_invalid;
        lp::ecs::Entity floor_entity = lp::ecs::const_entity_invalid;
        {
            btCollisionShape* csfloor = new btStaticPlaneShape(btVector3(0,1,0), 1);
            floor_collider = g_engine.getPhysicsWorld().registerCollisionShape(csfloor);
            auto& Recs = g_engine.getECS();
            floor_entity = Recs.createEntity();
            lp::ComponentPhysics phy;
            phy.mState = std::make_shared<btDefaultMotionState>(btTransform(btQuaternion(0,0,0,1), btVector3(0, 0, 0)));
            btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, phy.mState.get(), csfloor, btVector3(0,0,0));
            phy.mRigidBody = std::make_shared<btRigidBody>(groundRigidBodyCI);
            Recs.addComponent(floor_entity, phy);
        }

        // https://www.youtube.com/watch?app=desktop&v=BGAwRKPlpCw&t=14s
        // https://pybullet.org/Bullet/BulletFull/classbtCollisionShape.html
        //"D:/Semester_3/GK1/DELME/Relic Engine Framework/Assets/backpack/backpack.obj";
        //"C:/Programming/Personal/Solutions/ApplicationTestBuild/AppBuild/Models/Spheres/Spheres.gltf"


        const char* temp_modelNamePath = "C:/Programming/Personal/Solutions/ApplicationTestBuild/AppBuild/Models/SponzaPBR-Intel/Main.1_Sponza/NewSponza_Main_glTF_002.gltf";

        const auto modelRef = g_engine.getResurceManager().loadModel(temp_modelNamePath);
        
        lp::gl::GraphicsPrepareSystem GlobalPositioningSystem;

        {
            lp::ecs::Entity EE = g_engine.getECS().createEntity();
            lp::ComponentPosition CPos;
            CPos.setPosition({-100, 100, 0});
            lp::ComponentModel CModl;
            CModl.mID = modelRef;
            g_engine.getECS().addComponent(EE, CPos);
            g_engine.getECS().addComponent(EE, CModl);
        }


        lp::gl::Texture debugTextureLight;
        lp::gl::Texture debugTextureSound;
        {
            int x = 0; int y = 0; int channels = 0;
            stbi_uc* data = stbi_load("assets/images/icons/reshot-icon-light-max.png", &x, &y, &channels, 4); //
            if(data){
                debugTextureLight.create(lp::gl::Format::RGBA8, glm::uvec2(x, y), data, 0, true);
                stbi_image_free(data);
            } else std::cout << "Couldn't load icon-light-max: " << stbi_failure_reason() << "\n";
            
            data = stbi_load("assets/images/icons/reshot-icon-high-audio.png ", &x, &y, &channels, 4);
            if(data){
                debugTextureSound.create(lp::gl::Format::RGBA8, glm::uvec2(x, y), data, 0, true);
                stbi_image_free(data);
            } else std::cout << "Couldn't load icon-high-audio: " << stbi_failure_reason() << "\n";
        }

        //https://web.archive.org/web/20130419113144/http://bulletphysics.org/mediawiki-1.5.8/index.php/Hello_World
        //https://guibraga.medium.com/my-favorite-visual-studio-code-extensions-11573442008b

        bool IMGUIDoShowDemoWindow = false;
        bool IMGUIDoShowInfoWindow = true;

        bool IMGUIDoShowLightSystemWindow = false;
        bool IMGUIDoShowShaderManagerWindow = false;

        bool IMGUIDoShowDebugAllEntitiesWindow = false;

        

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
            
            lp::gl::ProcessedScene pScene;
            GlobalPositioningSystem.process(pScene, *g_engine.getPhysicsWorld().getDebugRenderer());

            {
                ImGui::BeginMainMenuBar();
                if(ImGui::BeginMenu("Options"))
                {
                    ImGui::Checkbox("Open Debug ImGui Window", &IMGUIDoShowDemoWindow);
                    ImGui::Checkbox("Open Info Window", &IMGUIDoShowInfoWindow);
                    ImGui::Checkbox("Open Light System Window", &IMGUIDoShowLightSystemWindow);
                    ImGui::Checkbox("Open Shader Manager Window", &IMGUIDoShowShaderManagerWindow);
                    ImGui::Checkbox("Open All Entities Window", &IMGUIDoShowDebugAllEntitiesWindow);
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
                    // {
                    //     const auto btPos = fallRigidBody->getWorldTransform().getOrigin();
                    //     ImGui::Text("Pos: x:%.2f ,y:%.2f ,z:%.2f", btPos.getX(), btPos.getY(), btPos.getZ());
                    // }
                    // {
                    //     const auto btPos = fallRigidBody->getCenterOfMassPosition();
                    //     ImGui::Text("CenterOfMassPositio: x:%.2f ,y:%.2f ,z:%.2f", btPos.getX(), btPos.getY(), btPos.getZ());
                    // }
                    // {
                    //     btTransform trans;
                    //     fallRigidBody->getMotionState()->getWorldTransform(trans);
                        
                    //     const auto btPos = trans.getOrigin();
                    //     ImGui::Text("Pos - 2: x:%.2f ,y:%.2f ,z:%.2f", btPos.getX(), btPos.getY(), btPos.getZ()); //Interpolated position
                    // }
                        
                    
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

                if(IMGUIDoShowDebugAllEntitiesWindow) int_imgui_all_entities_window(&IMGUIDoShowDebugAllEntitiesWindow);

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
                        auto& physicsC = g_engine.getECS().getComponent<lp::ComponentPhysics>(allSphere_entity);
                        physicsC.mRigidBody->applyCentralForce({vIctooor.x, vIctooor.y, vIctooor.z});
                        //physicsC.mRigidBody->applyCentralPushImpulse({vIctooor.x, vIctooor.y, vIctooor.z});
                        physicsC.mRigidBody->activate();
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
            

           g_engine.getResurceManager().getModelLoaderRef().update(1.0/12.0);

            if(mDoPhysics)
            {
                g_engine.getPhysicsWorld().stepSimulation(deltaTime);
            }
            std::vector<glm::vec3> mLightPositions;
            {
                auto& Recs = g_engine.getECS();
                for(const auto& entit: this->mLightSystem->mLightBufferEntityMap)
                {
                    if(Recs.isAlive(entit) && Recs.hasComponent<lp::ComponentLight>(entit))
                    {
                        const lp::ComponentLight& lig = Recs.getComponent<lp::ComponentLight>(entit);
                        mLightPositions.push_back(lig.getPosition());
                    }
                }
            }
            mRenndd.render(pScene);
            mRenndd.debug001(mLightPositions, debugTextureLight);


            mWindow.swapBuffers();
        }
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
    void int_imgui_all_entities_window(bool* windowOpen)
    {
        auto& Recs = lp::g_engine.getECS();
        auto& Rsol = lp::g_engine.getSoLoud();
        static const lp::ecs::Signature c_sign_light = Recs.getComponentSignature<lp::ComponentLight>();
        static const lp::ecs::Signature c_sign_model = Recs.getComponentSignature<lp::ComponentModel>();
        static const lp::ecs::Signature c_sign_physics = Recs.getComponentSignature<lp::ComponentPhysics>();
        static const lp::ecs::Signature c_sign_position = Recs.getComponentSignature<lp::ComponentPosition>();
        static const lp::ecs::Signature c_sign_soundsource = Recs.getComponentSignature<lp::ComponentSoundSource>();

        static std::pair<lp::ecs::Entity, lp::ecs::Signature> pair{lp::ecs::const_entity_invalid, 0};

        if(ImGui::Begin("Debug All Entities View", windowOpen))
        {
            const auto& allEntities = Recs.getDebugEntityMap();
            if (ImGui::BeginTable("table3", 6, ImGuiTableFlags_Borders))
            {
                ImGui::TableSetupColumn("Entity ID");
                ImGui::TableSetupColumn("Light C.");
                ImGui::TableSetupColumn("Model C.");
                ImGui::TableSetupColumn("Physics/Position C.");
                ImGui::TableSetupColumn("SoundSource C.");
                ImGui::TableSetupColumn("Inspect");
                ImGui::TableHeadersRow();
                for(const auto& ent:allEntities)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%u", ent.first);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%s", (ent.second & c_sign_light ? "X": " "));
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%s", (ent.second & c_sign_model ? "X": " "));
                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%s", ((ent.second & c_sign_physics || ent.second & c_sign_position) ? "X": " "));
                    ImGui::TableSetColumnIndex(4);
                    ImGui::Text("%s", (ent.second & c_sign_soundsource ? "X": " "));
                    ImGui::TableSetColumnIndex(5);
                    ImGui::PushID(ent.first);
                    if(ImGui::Button("Select"))
                    {
                        pair = ent;
                    }
                    ImGui::PopID();
                }
                 
                ImGui::EndTable();
            }
            if(Recs.isAlive(pair.first))
            {
                if(pair.second & c_sign_light)
                {
                    if(ImGui::TreeNode("Light"))
                    {
                        const auto& light = Recs.getComponent<lp::ComponentLight>(pair.first);
                        float colour[3] = {light.getColor().r, light.getColor().g, light.getColor().b};
                        ImGui::ColorEdit3("colour", colour, ImGuiColorEditFlags_NoInputs);
                        ImGui::Text("Position: x = %f, y = %f, z = %f", light.getPosition().x, light.getPosition().y, light.getPosition().z);
                        ImGui::Text("Radius: %f", light.getRadius());
                        ImGui::TreePop();
                    }
                }
                if(pair.second & c_sign_model)
                {
                    if(ImGui::TreeNode("Model"))
                    {
                        const auto& model = Recs.getComponent<lp::ComponentModel>(pair.first);
                        ImGui::Text("ModelID_t = %u", model.mID);
                        if(model.mModel != nullptr)
                        {
                            ImGui::Text("Filename = %s", model.mModel->mFile.filename().generic_string().c_str());
                            ImGui::Text("Mesh Count = %llu", (unsigned long long)model.mModel->mMeshes.size());
                            ImGui::Text("Material Count = %llu", (unsigned long long)model.mModel->mMaterials.size());
                        } else
                        {
                            ImGui::Text("Mesh not yet loaded!");
                        }
                        
                        ImGui::TreePop();
                    }
                }
                if(pair.second & c_sign_physics)
                {
                    if(ImGui::TreeNode("Physics"))
                    {
                        ImGui::Text("Sample Text");
                        ImGui::TreePop();
                    }
                }
                if(pair.second & c_sign_position)
                {
                    if(ImGui::TreeNode("Physics"))
                    {
                        const auto& model = Recs.getComponent<lp::ComponentPosition>(pair.first);
                        ImGui::Text("Position: x = %f, y = %f, z = %f", model.getPosition().x, model.getPosition().y, model.getPosition().z);
                        ImGui::Text("Scale: x = %f, y = %f, z = %f", model.getScale().x, model.getScale().y, model.getScale().z);
                        ImGui::Text("Position: x = %f, y = %f, z = %f, w = %f", model.getRotation().x, model.getRotation().y, model.getRotation().z, model.getRotation().w);
                        ImGui::TreePop();
                    }
                }
                if(pair.second & c_sign_soundsource)
                {
                    if(ImGui::TreeNode("Sound Source"))
                    {
                        auto& sound = Recs.getComponent<lp::ComponentSoundSource>(pair.first);
                        SoLoud::handle hand = sound.getHandle();
                        
                        ImGui::Text("Looping %s", (Rsol.getLooping(hand) ? "yes": "no"));
                        ImGui::Text("Loop point: %f", (float)Rsol.getLoopPoint(hand));
                        ImGui::Text("Overall Volume: %f", (float)Rsol.getOverallVolume(hand));
                        ImGui::Text("Pan: %f", (float)Rsol.getPan(hand));
                        ImGui::Text("Protect Voice %s", (Rsol.getProtectVoice(hand) ? "yes": "no"));
                        ImGui::Text("Relative Play Speed: %f", (float)Rsol.getRelativePlaySpeed(hand));
                        ImGui::Text("Samplerate: %f", (float)Rsol.getSamplerate(hand));
                        ImGui::TreePop();
                    }
                }
            }
        }
        ImGui::End();
    }


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
