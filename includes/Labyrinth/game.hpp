#ifndef LABYRINTH_PENDING_GAME_HPP
#define LABYRINTH_PENDING_GAME_HPP

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>



namespace lp
{
    class Game
    {
        public:
        /// @brief  Initializes the Window
        /// @return true if an error occurred, false otherwise
        bool create();

        /// @brief main game loop
        void loop();

        /// @brief called to destroy the window
        void destroy();
        private:

        GLFWwindow* mWindow = nullptr;
    };
}

#endif //LABYRINTH_PENDING_GAME_HPP