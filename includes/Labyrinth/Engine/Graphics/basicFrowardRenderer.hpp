#ifndef LABYRINTH_PENDING_ENGINE_GRAPHICS_BASICFORWARDRENDERER_HPP
#define LABYRINTH_PENDING_ENGINE_GRAPHICS_BASICFORWARDRENDERER_HPP
#include "framebuffer.hpp"
#include <glm/glm.hpp>
#include <glad/gl.h>

#include "Labyrinth/Engine/Resource/loadedModel.hpp"

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
        public:
        void setup(const unsigned int cv_width, unsigned int cv_height);
        void render(const DebugRendererData& cv_data);
        void destroy();
        private:

        /// @brief VAO used for all textured models
        GLuint mVertexArrayModelTextured = 0;

        GLuint mUBO_Camera = 0;
        GLsync mSyncCamera = {};

        Framebuffer mOutBuff;
        unsigned short mFramebufferAttachColorID = 0;
        unsigned short mFramebufferAttachDepthID = 0;
    };
}

#endif //LABYRINTH_PENDING_ENGINE_GRAPHICS_BASICFORWARDRENDERER_HPP