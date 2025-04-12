#ifndef LABYRINTH_PENDING_ENGINE_GRAPHICS_FRAMEBUFFER_HPP
#define LABYRINTH_PENDING_ENGINE_GRAPHICS_FRAMEBUFFER_HPP

#include <glad/gl.h>
#include <vector>

namespace lp::gl
{
    /// @brief Class that represents a framebuffer (a RenderBuffer in OpenGL terminology).
    ///
    /// Stores information about attachments in order they are added with createXXX() functions.
    ///
    /// Used for reloading the framebuffer for resizing
    class Framebuffer
    {
    public:
        enum class Attachment_Type :uint_fast8_t 
        {
            Image,// Faster Read
            RenderBuffer // Faster Write
        };
    
        enum class Image_Format :uint_fast8_t
        {
            SRGB8,
            R8, R16F, R32F,
            RG8, RG16F, RG32F,
            RGB8, RGB16F, RGB32F,
            RGBA8, RGBA16F, RGBA32F,
            //Depth only
            DEPTH16, DEPTH24, DEPTH32, DEPTH32F,
            //Depth-Stencil
            DEPTH24_STENCIL8, DEPTH32F_STENCIL8,
        };
    
        /// @brief create a framebuffer
        /// @param width - width of the new framebuffer
        /// @param height - height of the new framebuffer
        /// @param samples - the samples count of the framebuffer
        void create(unsigned int width, unsigned int height, short samples = 1);
    
        inline ~Framebuffer() { this->unload(); }
    
        /// @brief get the Width/Height
        /// @return double(Width) / double(Height)
        inline double getWidthDivHeight()const { return double(Width) / double(Height); }
    
        /// @brief get the width of this framebuffer
        /// @return width of this framebuffer
        inline unsigned int getWidth() const { return Width; }
    
        /// @brief get the height of this framebuffer
        /// @return height of this framebuffer
        inline unsigned int getHeight() const { return Height; }
    
        /// @brief bind this framebuffer with all color buffers
        inline void bind()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, this->FramebufferID);
            glViewport(0, 0, Width, Height);
            if(mColorBufferAttachmentEnums.size() > 1)
                glNamedFramebufferDrawBuffers(FramebufferID, static_cast<GLsizei>(mColorBufferAttachmentEnums.size()), mColorBufferAttachmentEnums.data());
        }
    
        /// @brief bind this framebuffer with chosen color buffers
        /// @param n - count of the table
        /// @param table - pointer to a n-sized table of GL_COLOR_ATTACHMENT's
        inline void bind(unsigned short n, GLenum* table)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, this->FramebufferID);
            glViewport(0, 0, Width, Height);
            if (n > 1)
                glNamedFramebufferDrawBuffers(FramebufferID, n, table);
        }
    
        /// @brief ONLY bind the framebuffer, do not update the viewport or set the drawbuffers
        inline void bindPure()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, this->FramebufferID);
        }
    
        /// @brief get the id of the attachment
        /// @param index - index of the attachment
        /// @return OpenGL id of the texture/renderbuffer (the 0-starting increasing index of createTexture's and createRenderBuffer's)
        inline GLuint getAttachmentID(unsigned short index)const
        {
            return this->mData[index].mId;
        }
    
        /// @brief resize this framebuffer
        /// @param width - new width
        /// @param height - new height
        void resize(unsigned int width, unsigned int height);
    
        /// @brief 'load' (recreate) this framebuffer (if is unloaded)
        void load();
    
        /// @brief 'unload' (destroy) this framebuffer (if is loaded)
        void unload();
    
        /// @brief create a texture
        /// @param IF - image format
        /// @return index of this texture
        unsigned short createTexture(const Image_Format IF);
    
        /// @brief create a RenderBuffer
        /// @param IF - image format
        /// @return index of this RenderBuffer
        unsigned short createRenderBuffer(const Image_Format IF);
        
        /// @brief print all the data of this framebuffer
        /// @see https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
        void test();
    
        /// @brief blit the depth buffer of this Framebuffer TO target
        /// @param target - destination of the blit - 'copy'
        inline void blitDepthOnlyTo(Framebuffer& target)
        {
            glBlitNamedFramebuffer(FramebufferID, target.FramebufferID, 0, 0, target.Width, target.Height, 0, 0, Width, Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        }
    
        /// @brief  blit the color buffers of this Framebuffer TO target
        /// @param target - destination of the blit - 'copy'
        /// @param ReadColorAttachNum - GL_COLOR_ATTACHMENT id of the color buffer source
        /// @param DrawColorAttachNum - GL_COLOR_ATTACHMENT id of the color buffer destination
        void blitColorTo(Framebuffer& target, short ReadColorAttachNum, short DrawColorAttachNum)
        {
            glNamedFramebufferReadBuffer(FramebufferID, GL_COLOR_ATTACHMENT0 + ReadColorAttachNum);
            glNamedFramebufferDrawBuffer(target.FramebufferID, GL_COLOR_ATTACHMENT0 + DrawColorAttachNum);
            glBlitNamedFramebuffer(FramebufferID, target.FramebufferID, 0, 0, target.Width, target.Height, 0, 0, Width, Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }
    
        /// @brief  blit the color and depth buffers of this Framebuffer TO target
        /// @param target - destination of the blit - 'copy'
        /// @param ReadColorAttachNum - GL_COLOR_ATTACHMENT id of the color buffer source
        /// @param DrawColorAttachNum - GL_COLOR_ATTACHMENT id of the color buffer destination
        void blitColorDepthTo(Framebuffer& target, short ReadColorAttachNum, short DrawColorAttachNum)
        {
            glNamedFramebufferReadBuffer(FramebufferID, GL_COLOR_ATTACHMENT0 + ReadColorAttachNum);
            glNamedFramebufferDrawBuffer(target.FramebufferID, GL_COLOR_ATTACHMENT0 + DrawColorAttachNum);
            glBlitNamedFramebuffer(FramebufferID, target.FramebufferID, 0, 0, target.Width, target.Height, 0, 0, Width, Height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        }
    
    
    private:
        /// @brief hidden helper struct storing all info about an attachment
        struct AttachmentData
        {
            GLuint mId = 0; //id of the attachment
            Attachment_Type mType = Attachment_Type::Image; // type of the attachemnt
            Image_Format mFormat = Image_Format::R8; //format of the attachment
        };
        
        /// @brief id of the framebuffer
        GLuint FramebufferID = 0;
        /// @brief height of the framebuffer
        unsigned int Width = 0;
        /// @brief height of the framebuffer
        unsigned int Height = 0;
        /// @brief sample count of the framebuffer. 1 means no multisampling, max value checked in create()
        short samples_count = 1;
        /// @brief is this loaded
        bool mLoaded = true;
        
        /// @brief vector storing attachment data ONLY when the framebuffer is loaded
        std::vector<AttachmentData> mData;
        /// @brief copy of the data when the framebuffer is unloaded, used for loading it again
        std::vector<AttachmentData>* mDataCopy = nullptr;

        std::vector<GLenum> mColorBufferAttachmentEnums; //table for glNamedFramebufferDrawBuffers
    };
}

#endif //LABYRINTH_PENDING_ENGINE_GRAPHICS_FRAMEBUFFER_HPP