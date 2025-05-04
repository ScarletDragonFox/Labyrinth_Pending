#include "Labyrinth/window.hpp"

#include <GLFW/glfw3.h>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui.h>
#include <glad/gl.h>


#include <Labyrinth/engine.hpp>

namespace
{
    /// @brief GLFW callback function for every key press/release
    void glfw_key_callback([[maybe_unused]] GLFWwindow* window, int key, int scancode, int action, int mods);
    /// @brief GLFW callback function for every cursor movment
    void glfw_cursor_position_callback([[maybe_unused]] GLFWwindow* window, double xpos, double ypos);
    /// @brief GLFW callback function for every window size change
    void glfw_window_size_callback([[maybe_unused]] GLFWwindow* window, int v_width, int v_height);
    /// @brief GLFW callback function for GLFW error (to be printed to console)
    void glfw_error_callback(int error, const char* description);
}

namespace lp
{

    bool Window::create(const char* cv_title, const unsigned int cv_width, const unsigned int cv_height)
    {
        glfwSetErrorCallback(glfw_error_callback);
        if(!glfwInit()) return true;
        
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        this->mWindow = glfwCreateWindow(cv_width, cv_height, cv_title, nullptr, nullptr);
        if(!mWindow)
        {
            glfwTerminate();
            return true;
        }
        glfwMakeContextCurrent(mWindow);

        int glversion = gladLoadGL(glfwGetProcAddress);
        if (glversion == 0) {
            std::cerr << "Failed to initialize OpenGL context\n";
            return true;
        }

        if(glfwRawMouseMotionSupported())
        {
            glfwSetInputMode(mWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
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

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark(); //ImGui::StyleColorsLight();
       
        ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
        ImGui_ImplOpenGL3_Init(); //TODO: these functions seem to have bool success/fail return values. They are undocumented. Utilize?
        
        return false;
    }

    void Window::pollEvents()
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void Window::swapBuffers()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(this->mWindow);
    }

    void Window::debugGetFramebufferSize(int& o_width, int& o_height)
    {
        glfwGetFramebufferSize(mWindow, &o_width, &o_height);
    }

    void Window::getWindowSize(int& o_width, int& o_height)
    {
        glfwGetWindowSize(mWindow, &o_width, &o_height);
    }

    bool Window::shouldClose()const
    {
        return glfwWindowShouldClose(this->mWindow);
    }

    void Window::destroy()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(mWindow); mWindow = nullptr;
        glfwTerminate();
    }

}

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

}