#ifndef LABYRINTH_PENDING_WINDOW_HPP
#define LABYRINTH_PENDING_WINDOW_HPP

/// @brief Forward declaration of opaque pointer for GLFW's window.
struct GLFWwindow;

namespace lp
{
    /// @brief Represents the GLFW window
    ///
    /// Due to GLFW - GLAD (OpenGL) loading rules also loads OpenGL
    /// 
    /// Must be initialized() BEFORE calling g_engine.initialize() due to this.
    ///
    /// This class handles:
    ///
    /// - All GLFW window operations
    ///
    /// - Disabling/Enabling Mouse motion on PlayerTriggerInputs Event
    ///
    /// - Setting up ImGui + Backends (i.e. <imgui_impl_glfw.h> & <imgui_impl_opengl3.h>)
    class Window
    {
        public:

        /// @brief Creates the window.
        ///
        /// Also Loads OpenGL & Sets up ImGui
        /// @param cv_title title of the window
        /// @param cv_width width of the window
        /// @param cv_height height of the window
        /// @return true if an error occurred, false otherwise
        bool create(const char* cv_title, const unsigned int cv_width, const unsigned int cv_height);

        /// @brief poll GLFW for events. Calls glfwPollEvents().
        void pollEvents();

        /// @brief returns true if the window should close.
        ///
        /// This is just glfwWindowShouldClose(), but bool
        bool shouldClose()const;

        /// @brief swap the Buffers. (and run ImGui's Render())
        /// @todo ImGui's render function is here, MIGHT at some point cause a problem, so leaving this comment here
        void swapBuffers();

        /// TODO: Remove me!!!
        void debugGetFramebufferSize(int& o_width, int& o_height);

        /// @brief Peramamently destroys the window & its OpenGL context.
        ///
        /// Should only be called on shutdown.
        ///
        /// @note Unrecoverable. Calling other functions after this SHOULD fail.
        void destroy();

        private:
        /// @brief ptr to opaque ptr of GLFW's window
        GLFWwindow* mWindow = nullptr;
    };
}

#endif //LABYRINTH_PENDING_WINDOW_HPP