#include <glad/gl.h> //
#define GLFW_INCLUDE_NONE //we use our own opnegl header <- put this into cmake
#include <GLFW/glfw3.h> //

#include <assimp/Importer.hpp>
#include <glm/glm.hpp>
#include <stb_image.h>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui.h>

#include <nlohmann/json.hpp>
#include <bullet/btBulletCollisionCommon.h>

#include <lua.h>
//https://lucasklassmann.com/blog/2023-02-26-more-advanced-examples-of-embedding-lua-in-c/
//https://lucasklassmann.com/blog/2019-02-02-embedding-lua-in-c/

#include <soloud.h>
#include <soloud_speech.h>
#include <soloud_thread.h>
#include <soloud_wav.h>

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

void glfw_error_callback(int error, const char* description);

int main()
{
    GLFWwindow* window = nullptr;
    if(!glfwInit()) return -1;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //GLFW_SCALE_TO_MONITOR  GLFW_SCALE_FRAMEBUFFER 
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);

    if(!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    
    int glversion = gladLoadGL(glfwGetProcAddress);
    if (glversion == 0) {
        std::cerr << "Failed to initialize OpenGL context\n";
        return -1;
    }

    // Successfully loaded OpenGL
    std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(glversion) << "." << GLAD_VERSION_MINOR(glversion) << "\n";
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark(); 
    //ImGui::StyleColorsLight();
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    SoLoud::Soloud soloud;  // SoLoud engine core
    SoLoud::Speech speech;  // A sound source (speech, in this case)

    speech.setParams(2000U, 9.0f, 0.5f, 20);
    soloud.init();
    std::cout << "Using SoLoud backend: " << soloud.getBackendString() << "\n";
    
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

    while (!glfwWindowShouldClose(window))
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
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());



        glfwSwapBuffers(window);
    }
    soloud.deinit(); // Clean up SoLoud

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;

    // // Wait until sounds have finished
    // while (soloud.getActiveVoiceCount() > 0)
    // {
    //     // Still going, sleep for a bit
    //     SoLoud::Thread::sleep(100);
    // }
}


void glfw_error_callback(int error, const char* description)
{
    std::cerr << "ERROR::GLFW " << error << " - " << description << "\n";
}