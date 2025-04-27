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

namespace
{
    void glfw_key_callback([[maybe_unused]] GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if(action == GLFW_REPEAT) return;
        const lp::evt::WindowKeyAction kay = 
        {
            .key = key,
            .scancode = scancode,
            .pressed = (action == GLFW_PRESS),
            .modShift = static_cast<bool>(mods & GLFW_MOD_SHIFT),
            .modControl = static_cast<bool>(mods & GLFW_MOD_CONTROL),
            .modAlt = static_cast<bool>(mods & GLFW_MOD_ALT),
            .modSuper = static_cast<bool>(mods & GLFW_MOD_SUPER)
        };
        lp::Event evt(lp::EventTypes::WindowKeyAction, kay);
        lp::g_engine.getEventManager().emit(evt);
    }

    void glfw_cursor_position_callback([[maybe_unused]] GLFWwindow* window, double xpos, double ypos)
    {
        static double previousPosX = xpos;
        static double previousPosY = ypos;
        const lp::evt::WindowMouseMotion msy = 
        {
            .posX = xpos,
            .posY = ypos,
            .deltaX = xpos - previousPosX,
            .deltaY = previousPosY - ypos
        };
        previousPosX = xpos;
        previousPosY = ypos;
        lp::Event evt(lp::EventTypes::WindowMouseMotion, msy);
        lp::g_engine.getEventManager().emit(evt);
    }

    void glfw_window_size_callback([[maybe_unused]] GLFWwindow* window, int v_width, int v_height)
    {
        static int lastWidth = -1;
        static int lastHeight = -1;
        if(lastWidth != v_width || lastHeight != v_height)
        {
            const lp::evt::WindowResize wre = 
            {
                .width = v_width,
                .height = v_height
            };
            lp::Event evt(lp::EventTypes::WindowResize, wre);
            lp::g_engine.getEventManager().emit(evt);
        }
    }

    void glfw_error_callback(int error, const char* description)
    {
        const char* errorName = "GLFW_NO_ERROR";
        switch(error)
        {
            case GLFW_NOT_INITIALIZED:
                errorName = "GLFW_NOT_INITIALIZED";
                break;
            case GLFW_NO_CURRENT_CONTEXT:
                errorName = "GLFW_NO_CURRENT_CONTEXT";
                break;
            case GLFW_INVALID_ENUM:
                errorName = "GLFW_INVALID_ENUM";
                break;
            case GLFW_INVALID_VALUE:
                errorName = "GLFW_INVALID_VALUE";
                break;
            case GLFW_OUT_OF_MEMORY:
                errorName = "GLFW_OUT_OF_MEMORY";
                break;
            case GLFW_API_UNAVAILABLE:
                errorName = "GLFW_API_UNAVAILABLE";
                break;
            case GLFW_VERSION_UNAVAILABLE:
                errorName = "GLFW_VERSION_UNAVAILABLE";
                break;
            case GLFW_PLATFORM_ERROR:
                errorName = "GLFW_PLATFORM_ERROR";
                break;
            case GLFW_FORMAT_UNAVAILABLE:
                errorName = "GLFW_FORMAT_UNAVAILABLE";
                break;
            case GLFW_NO_WINDOW_CONTEXT:
                errorName = "GLFW_NO_WINDOW_CONTEXT";
                break;
            case GLFW_CURSOR_UNAVAILABLE:
                errorName = "GLFW_CURSOR_UNAVAILABLE";
                break;
            case GLFW_FEATURE_UNAVAILABLE:
                errorName = "GLFW_FEATURE_UNAVAILABLE";
                break;
            case GLFW_FEATURE_UNIMPLEMENTED:
                errorName = "GLFW_FEATURE_UNIMPLEMENTED";
                break;
            case GLFW_PLATFORM_UNAVAILABLE:
                errorName = "GLFW_PLATFORM_UNAVAILABLE";
                break;
            default:
                errorName = "GLFW unrecoginsed error!";
                break;
        }
        std::cerr << "ERROR: " << errorName << " " << description << "\n";
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



LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH();

#include <bullet/btBulletDynamicsCommon.h>

LP_PRAGMA_DISABLE_ALL_WARNINGS_POP();

#include <memory>

#include "Labyrinth/Engine/Graphics/bullet3Debug.hpp"

#include "Labyrinth/Engine/Resource/modelLoader.hpp"

namespace lp
{

    bool Game::create()
    {
        glfwSetErrorCallback(glfw_error_callback);

        if(!glfwInit()) return true;
        
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        

        //GLFW_SCALE_TO_MONITOR  GLFW_SCALE_FRAMEBUFFER 
        mWindow = glfwCreateWindow(640, 480, "Labyrinth Pending", NULL, NULL);
        if(!mWindow)
        {
            glfwTerminate();
            return true;
        }
        glfwMakeContextCurrent(mWindow);
        
        if(lp::g_engine.initialize())
        {
            return true;
        }

        if(glfwRawMouseMotionSupported())
        {
            glfwSetInputMode(mWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        }

        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
        glDebugMessageCallback(opengl_message_callback, nullptr);
       
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        glfwSetKeyCallback(mWindow, glfw_key_callback);
        glfwSetCursorPosCallback(mWindow, glfw_cursor_position_callback);
        glfwSetWindowSizeLimits(mWindow, 10, 2, GLFW_DONT_CARE, GLFW_DONT_CARE); //stop user from creating a 0 x 0 size framebuffer.
        glfwSetWindowSizeCallback(mWindow, glfw_window_size_callback);

        g_engine.getEventManager().on(lp::EventTypes::PlayerTriggerInputs, [this](Event& rv_evt)
        {
            bool enable = rv_evt.getData<bool>();
            if(enable){
                glfwSetInputMode(this->mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }else{
                glfwSetInputMode(this->mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        });


        mRenndd.setup(640, 480);

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark(); //ImGui::StyleColorsLight();
       
        ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
        ImGui_ImplOpenGL3_Init();
        

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
        std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(dispatcher.get(), broadphase.get(), solver.get(), collisionConfiguration.get());
        dynamicsWorld->setDebugDrawer(&bulletDebugRenderer);
        
        dynamicsWorld->setGravity(btVector3(0,-10,0));
    
        std::unique_ptr<btRigidBody> groundRigidBody;
        std::unique_ptr<btRigidBody> fallRigidBody;
        std::unique_ptr<btCollisionShape> groundShape = std::make_unique<btStaticPlaneShape>(btVector3(0,1,0), 1);
        std::unique_ptr<btCollisionShape> fallShape = std::make_unique<btSphereShape>((btScalar)1.0f);
        {
            std::unique_ptr<btDefaultMotionState> groundMotionState = std::make_unique<btDefaultMotionState>(btTransform(btQuaternion(0,0,0,1), btVector3(0,-1,0)));
    
            btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState.get(), groundShape.get(), btVector3(0,0,0));
        
            groundRigidBody =  std::make_unique<btRigidBody>(groundRigidBodyCI);
        }
        
        
    
        dynamicsWorld->addRigidBody(groundRigidBody.get());
    
        std::unique_ptr<btDefaultMotionState> fallMotionState = std::make_unique<btDefaultMotionState>(btTransform(btQuaternion(0,0,0,1), btVector3(0,500,0)));
    
        btScalar mass = 1;
        btVector3 fallInertia(0,0,0);
        fallShape->calculateLocalInertia(mass,fallInertia);
    
        btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState.get() ,fallShape.get(), fallInertia);
        fallRigidBody = std::make_unique<btRigidBody>(fallRigidBodyCI);
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
        
        lp::res::ModelLoader mLoad;

        const char* temp_modelNamePath = "backpack.obj";

        mLoad.scheduleLoad(temp_modelNamePath);
        //mLoad.scheduleLoad("Spheres.gltf");
        

        dynamicsWorld->debugDrawWorld();

        
        GLuint VAO_temp = 0;
        {
            glCreateVertexArrays(1, &VAO_temp);
            glEnableVertexArrayAttrib(VAO_temp, 0);
            glEnableVertexArrayAttrib(VAO_temp, 1);
            glVertexArrayAttribFormat(VAO_temp, 0, 3, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribFormat(VAO_temp, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
            glVertexArrayAttribBinding(VAO_temp, 0, 0);
            glVertexArrayAttribBinding(VAO_temp, 1, 0);
        }

        //https://web.archive.org/web/20130419113144/http://bulletphysics.org/mediawiki-1.5.8/index.php/Hello_World
        //https://guibraga.medium.com/my-favorite-visual-studio-code-extensions-11573442008b

        bool IMGUIDoShowDemoWindow = false;
        bool IMGUIDoShowInfoWindow = true;

        bool mDoPhysics = false;
        double lastFrameTime = glfwGetTime();
        while(!glfwWindowShouldClose(mWindow))
        {
            glfwPollEvents();
            const double deltaTime = glfwGetTime() - lastFrameTime;
            lastFrameTime = glfwGetTime();

            {
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();

                ImGui::BeginMainMenuBar();
                if(ImGui::BeginMenu("Options"))
                {
                    ImGui::Checkbox("Open Debug ImGui Window", &IMGUIDoShowDemoWindow);
                    ImGui::Checkbox("Open Info Window", &IMGUIDoShowInfoWindow);
                    ImGui::Separator();
                    ImGui::Checkbox("Do Physics", &mDoPhysics);
                    if(ImGui::Button("Reset"))
                    {
                        {
                            btTransform transform2 = {};
                            const btVector3 pos = btVector3(0, 50, 0);
                            transform2.setOrigin(pos);
                            fallRigidBody->setWorldTransform(transform2);
                            fallRigidBody->getMotionState()->setWorldTransform(transform2);
                            fallRigidBody->clearForces();
                            fallRigidBody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
                            fallRigidBody->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
                            fallRigidBody->activate();
                        }
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
                        ImGui::End();
                    }
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
                    ImGui::End();
                }

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
            mLoad.update(1.0/12.0);
         //   std::cout << "After update() of load\n";

            if(mDoPhysics)
            {
                dynamicsWorld->stepSimulation(deltaTime, 100);
                dynamicsWorld->debugDrawWorld();
            }
            
          //  std::cout << "After update() of physics\n";

            double wbh = 0.0;
            {
                int width = 0, height = 0;
                glfwGetFramebufferSize(mWindow, &width, &height);
                wbh = (double)width / (double)height;
                glViewport(0, 0, width, height);
            }
            
            mPlayer.update(deltaTime);
            gl::DebugRendererData dtttta;
            
            dtttta.mCamProjection = mPlayer.getProjectionMatrix(wbh);
            dtttta.mCamView = mPlayer.getViewMatrix();

            if(!bulletDebugRenderer.verticies.empty())
            {
                dtttta.VAO = VAO_temp;
                dtttta.drawCount = bulletDebugRenderer.drawCount;
                glCreateBuffers(1, &dtttta.VBO);
                glNamedBufferStorage(dtttta.VBO, bulletDebugRenderer.verticies.size() * sizeof(lp::gl::Bullet3Debug::Vertex), bulletDebugRenderer.verticies.data(), 0);
                glVertexArrayVertexBuffer(VAO_temp, 0, dtttta.VBO, 0, 6 * sizeof(float));
            }
         //   std::cout << "After update() of bulletDebugRenderer.verticies\n";

            const lp::res::LoadedModel* modeel = mLoad.getLoadedModel(temp_modelNamePath);
            dtttta.mdl = modeel;
            
            mRenndd.render(dtttta);
          //  std::cout << "After mRenndd.render(dtttta)\n";

            glDeleteBuffers(1, &dtttta.VBO);

            ImGui::Render();

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(mWindow);
        }
    }

    void Game::destroy()
    {   
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        g_engine.destroy();
        
        glfwDestroyWindow(mWindow); mWindow = nullptr;
        glfwTerminate();
    }
}
