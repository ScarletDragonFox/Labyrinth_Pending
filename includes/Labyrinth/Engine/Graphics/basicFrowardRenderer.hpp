#ifndef LABYRINTH_PENDING_ENGINE_GRAPHICS_BASICFORWARDRENDERER_HPP
#define LABYRINTH_PENDING_ENGINE_GRAPHICS_BASICFORWARDRENDERER_HPP
#include "framebuffer.hpp"

#include "Labyrinth/Helpers/compilerErrors.hpp"
LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH();
#include <glm/glm.hpp>
LP_PRAGMA_DISABLE_ALL_WARNINGS_POP();

#include <glad/gl.h>

#include "Labyrinth/Engine/Resource/loadedModel.hpp"

#include "Labyrinth/player.hpp"

#include <Labyrinth/Engine/Graphics/processedScene.hpp>

#include <vector>
#include <Labyrinth/Engine/Graphics/textures.hpp>

///UBO Bindings:
// 0 - DebugRendererData / mUBO_Player

//SSBO Bindings:
// 0 - Point lights allocation array
// 1 - Point lights uint array of live


namespace lp::gl
{
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
        void render(const lp::gl::ProcessedScene& cv_pscene);
        void debug001(std::vector<glm::vec3>& lightpositions, lp::gl::Texture& texture);

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

        bool mTriggerDrawDebugLightIcons = false;
        bool mTriggerDrawDebugSoundIcons = false;

        // @brief if true, will draw bullet debug lines
        bool mTriggerDrawDebugBullet = false;

        private:

        void DrawDebugIcons();

        /// @brief VAO used for all textured models
        GLuint mVertexArrayModelTextured = 0;

        /// @brief VAO used for bullet debug drawing
        GLuint mVertexArrayBulletLineDebug = 0;

        /// @brief VAO used by the debug light range drawing
        GLuint mVertexArrayISOSphere = 0;

        /// @brief empty VAO to stop OpenGL complaints
        GLuint mVertexArrayDummy = 0;

        /// @brief VBO used by the debug light range drawing
        GLuint mVertexBufferISOSphere = 0;

        /// @brief EBO used by the debug light range drawing
        GLuint mElementBufferISOSphere = 0;

        //GLsync mSyncCamera = {};
        GLuint mUBO_Player = 0;
        RendererForwardPlus_PlayerData mPlayerData;

        lp::gl::Texture mTexLightIcon;
        lp::gl::Texture mTexSoundIcon;

        Framebuffer mOutBuff;
        unsigned short mFramebufferAttachColorID = 0;
        unsigned short mFramebufferAttachDepthID = 0;
    };
}

#endif //LABYRINTH_PENDING_ENGINE_GRAPHICS_BASICFORWARDRENDERER_HPP