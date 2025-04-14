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
}

namespace lp
{

    bool Game::create()
    {
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

        SoLoud::Speech speech;
        speech.setParams(2000U, 9.0f, 0.5f, 20);
        //std::cout << "Using SoLoud backend: " << soloud.getBackendString() << "\n";
        
        bool imguiDoShowDemoWindow = true;

        struct FF
        {
            int id = 0;
            SoLoud::Wav waav;
            SoLoud::handle handle;
            std::string window_name = "Default";
        }; 
        static int id_cunter = 1;
        std::vector<FF*> songs_loaded;

        SoLoud::Soloud &soloud = g_engine.getSoLoud();

        std::cout << "HERE2!\n";

        while(!glfwWindowShouldClose(mWindow))
        {
            glfwPollEvents();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            if (imguiDoShowDemoWindow) ImGui::ShowDemoWindow(&imguiDoShowDemoWindow);

            if(ImGui::Begin("Sound"))
            {
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
            }
            double wbh = 0.0;
            {
                int width = 0, height = 0;
                glfwGetFramebufferSize(mWindow, &width, &height);
                wbh = (double)width / (double)height;
                glViewport(0, 0, width, height);
            }
            
            mPlayer.update(0.01);
            gl::DebugRendererData dtttta;
            
            dtttta.mCamProjection = mPlayer.getProjectionMatrix(wbh);
            dtttta.mCamView = mPlayer.getViewMatrix();

          

            mRenndd.render(dtttta);

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
