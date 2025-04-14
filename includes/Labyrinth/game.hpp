#ifndef LABYRINTH_PENDING_GAME_HPP
#define LABYRINTH_PENDING_GAME_HPP

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Labyrinth/Engine/Graphics/basicFrowardRenderer.hpp"
#include "Labyrinth/player.hpp"

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

        lp::gl::ForwardRenderer mRenndd;
        lp::Player mPlayer;
        GLFWwindow* mWindow = nullptr;
    };
}

#endif //LABYRINTH_PENDING_GAME_HPP