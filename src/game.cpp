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

        std::cout << "HERE!\n";

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
        
            ImGui::Render();
            
            int display_w, display_h;
            glfwGetFramebufferSize(mWindow, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClear(GL_COLOR_BUFFER_BIT);
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
