#ifndef LABYRINTH_PENDING_ENGINE_GRAPHICS_BASICFORWARDRENDERER_HPP
#define LABYRINTH_PENDING_ENGINE_GRAPHICS_BASICFORWARDRENDERER_HPP
#include "framebuffer.hpp"
#include <glm/glm.hpp>
#include <glad/gl.h>

#include "Labyrinth/Engine/Resource/loadedModel.hpp"

#include "Labyrinth/player.hpp"

///UBO Bindings:
// 0 - DebugRendererData / mUBO_Player

namespace lp::gl
{
    struct DebugRendererData
    {
        glm::mat4 mCamView = {};
        glm::mat4 mCamProjection = {};
        GLuint VBO = 0;
        GLuint VAO = 0;
        GLuint drawCount = 0;
        const lp::res::LoadedModel* mdl = nullptr;
    };

    class ForwardRenderer
    {
        struct RendererForwardPlus_PlayerData
        {
            glm::mat4 mView = {};
            glm::mat4 mProjection = {}; //infinite projection
            glm::vec3 mPosition = {};
            float padding = 0.0f;
        };
        public:
        void setup(const unsigned int cv_width, unsigned int cv_height);
        void render(const DebugRendererData& cv_data);

        /// @brief update the RendererForwardPlus_PlayerData and the UBO storing it.
        ///
        /// This is called directly after parsing player inputs
        ///
        /// If cv_wasChanged == false does nothing
        ///
        /// Done this way so we can avoid calculating some camera-based data on every frame
        /// @param cr_player const reference to the player whose data we use
        /// @param cv_wasChanged did the player move (position/orientation/fov etc..)
        void updatePlayer(const lp::Player& cr_player, bool cv_wasChanged = true);

        void destroy();
        private:

        /// @brief VAO used for all textured models
        GLuint mVertexArrayModelTextured = 0;

        //GLsync mSyncCamera = {};
        GLuint mUBO_Player = 0;
        RendererForwardPlus_PlayerData mPlayerData;

        Framebuffer mOutBuff;
        unsigned short mFramebufferAttachColorID = 0;
        unsigned short mFramebufferAttachDepthID = 0;
    };
}

#endif //LABYRINTH_PENDING_ENGINE_GRAPHICS_BASICFORWARDRENDERER_HPP