#ifndef LABYRINTH_PENDING_ENGINE_GRAPHICS_BASICFORWARDRENDERER_HPP
#define LABYRINTH_PENDING_ENGINE_GRAPHICS_BASICFORWARDRENDERER_HPP
#include "framebuffer.hpp"
#include <glm/glm.hpp>

namespace lp::gl
{
    struct DebugRendererData
    {
        glm::mat4 mCamView = {};
        glm::mat4 mCamProjection = {};
    };

    class ForwardRenderer
    {
        public:
        void setup(const unsigned int cv_width, unsigned int cv_height);
        void render(const DebugRendererData& cv_data);
        private:
        Framebuffer mOutBuff;
        unsigned short mFramebufferAttachColorID = 0;
        unsigned short mFramebufferAttachDepthID = 0;
    };
}

#endif //LABYRINTH_PENDING_ENGINE_GRAPHICS_BASICFORWARDRENDERER_HPP