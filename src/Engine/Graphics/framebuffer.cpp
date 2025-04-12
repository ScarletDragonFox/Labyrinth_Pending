#include "Labyrinth/Engine/Graphics/framebuffer.hpp"

#include <iostream>
#include <string_view>
#include <cassert>

namespace lp::gl
{

    namespace
    {
        constexpr uint8_t CONST_RenderTarget_Colour_Attachment = 1;
        constexpr uint8_t CONST_RenderTarget_Depth_Attachment = 2;
        constexpr uint8_t CONST_RenderTarget_DepthStencil_Attachment = 3;

        struct Image_Formats_Information {
            GLuint internal_format = 0;
            GLuint image_format = 0;
            std::string_view format_name;
            GLuint type = 0;
            uint8_t AttType = 0;
        };

        Image_Formats_Information Decode_IFI(const Framebuffer::Image_Format i) {
            Image_Formats_Information info;
            switch (i) {
                //COLOR only
            case Framebuffer::Image_Format::SRGB8:
                info.internal_format = GL_SRGB8;
                info.image_format = GL_SRGB;
                info.type = GL_UNSIGNED_BYTE;
                info.AttType = CONST_RenderTarget_Colour_Attachment;
                info.format_name = "RGB8";
                break;
            case Framebuffer::Image_Format::R8:
                info.internal_format = GL_R8;
                info.image_format = GL_RED;
                info.type = GL_UNSIGNED_BYTE;
                info.AttType = CONST_RenderTarget_Colour_Attachment;
                info.format_name = "R8";
                break;
            case Framebuffer::Image_Format::R16F:
                info.internal_format = GL_R16F;
                info.image_format = GL_RED;
                info.type = GL_FLOAT;
                info.AttType = CONST_RenderTarget_Colour_Attachment;
                info.format_name = "R16F";
                break;
            case Framebuffer::Image_Format::R32F:
                info.internal_format = GL_R32F;
                info.image_format = GL_RED;
                info.type = GL_FLOAT;
                info.AttType = CONST_RenderTarget_Colour_Attachment;
                info.format_name = "R32F";
                break;
            case Framebuffer::Image_Format::RG8:
                info.internal_format = GL_RG8;
                info.image_format = GL_RG;
                info.type = GL_UNSIGNED_BYTE;
                info.AttType = CONST_RenderTarget_Colour_Attachment;
                info.format_name = "RG8";
                break;
            case Framebuffer::Image_Format::RG16F:
                info.internal_format = GL_RG16F;
                info.image_format = GL_RG;
                info.type = GL_FLOAT;
                info.AttType = CONST_RenderTarget_Colour_Attachment;
                info.format_name = "RG16F";
                break;
            case Framebuffer::Image_Format::RG32F:
                info.internal_format = GL_RG32F;
                info.image_format = GL_RG;
                info.type = GL_FLOAT;
                info.AttType = CONST_RenderTarget_Colour_Attachment;
                info.format_name = "RG32F";
                break;
            case Framebuffer::Image_Format::RGB8:
                info.internal_format = GL_RGB8;
                info.image_format = GL_RGB;
                info.type = GL_UNSIGNED_BYTE;
                info.AttType = CONST_RenderTarget_Colour_Attachment;
                info.format_name = "RGB8";
                break;
            case Framebuffer::Image_Format::RGB16F:
                info.internal_format = GL_RGB16F;
                info.image_format = GL_RGB;
                info.type = GL_FLOAT;
                info.AttType = CONST_RenderTarget_Colour_Attachment;
                info.format_name = "RGB16F";
                break;
            case Framebuffer::Image_Format::RGB32F:
                info.internal_format = GL_RGB32F;
                info.image_format = GL_RGB;
                info.type = GL_FLOAT;
                info.AttType = CONST_RenderTarget_Colour_Attachment;
                info.format_name = "RGB32F";
                break;
            case Framebuffer::Image_Format::RGBA8:
                info.internal_format = GL_RGBA8;
                info.image_format = GL_RGBA;
                info.type = GL_UNSIGNED_BYTE;
                info.AttType = CONST_RenderTarget_Colour_Attachment;
                info.format_name = "RGBA8";
                break;
            case Framebuffer::Image_Format::RGBA16F:
                info.internal_format = GL_RGBA16F;
                info.image_format = GL_RGBA;
                info.type = GL_FLOAT;
                info.AttType = CONST_RenderTarget_Colour_Attachment;
                info.format_name = "RGBA16F";
                break;
            case Framebuffer::Image_Format::RGBA32F:
                info.internal_format = GL_RGBA32F;
                info.image_format = GL_RGBA;
                info.type = GL_FLOAT;
                info.AttType = CONST_RenderTarget_Colour_Attachment;
                info.format_name = "RGBA32F";
                break;
                //DEPTH only
            case Framebuffer::Image_Format::DEPTH16:
                info.internal_format = GL_DEPTH_COMPONENT16;
                info.image_format = GL_DEPTH_COMPONENT;
                info.type = GL_FLOAT;
                info.AttType = CONST_RenderTarget_Depth_Attachment;
                info.format_name = "DEPTH16";
                break;
            case Framebuffer::Image_Format::DEPTH24:
                info.internal_format = GL_DEPTH_COMPONENT24;
                info.image_format = GL_DEPTH_COMPONENT;
                info.type = GL_FLOAT;
                info.AttType = CONST_RenderTarget_Depth_Attachment;
                info.format_name = "DEPTH24";
                break;
            case Framebuffer::Image_Format::DEPTH32:
                info.internal_format = GL_DEPTH_COMPONENT32;
                info.image_format = GL_DEPTH_COMPONENT;
                info.type = GL_FLOAT;
                info.AttType = CONST_RenderTarget_Depth_Attachment;
                info.format_name = "DEPTH32";
                break;
            case Framebuffer::Image_Format::DEPTH32F:
                info.internal_format = GL_DEPTH_COMPONENT32F;
                info.image_format = GL_DEPTH_COMPONENT;
                info.type = GL_FLOAT;
                info.AttType = CONST_RenderTarget_Depth_Attachment;
                info.format_name = "DEPTH32F";
                break;
                //DEPTH_STENCIL only
            case Framebuffer::Image_Format::DEPTH24_STENCIL8:
                info.internal_format = GL_DEPTH24_STENCIL8;
                info.image_format = GL_DEPTH_STENCIL;
                info.type = GL_FLOAT;
                info.AttType = CONST_RenderTarget_DepthStencil_Attachment;
                info.format_name = "DEPTH24_STENCIL8";
                break;
            case Framebuffer::Image_Format::DEPTH32F_STENCIL8://
                info.internal_format = GL_DEPTH32F_STENCIL8;
                info.image_format = GL_DEPTH_STENCIL;
                info.type = GL_FLOAT;
                info.AttType = CONST_RenderTarget_DepthStencil_Attachment;
                info.format_name = "DEPTH32F_STENCIL8";
                break;
            default:
                std::cerr << "Unknown framebuffer format\n";
                assert(true);
                break;
            }
            return info;
        }
    }



        void Framebuffer::create(unsigned int width, unsigned int height, short samples)
    {
        this->Width = width;
        this->Height = height;
        

        GLint data = 0;

        glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &data);
        if (data < samples)
        {
            std::cerr << "RENDERTARGET: Bad samples count, samples= " << samples << "\n";
            std::cout << "GL_MAX_DEPTH_TEXTURE_SAMPLES: " << data << " < samples\n";
            samples = std::min(data, GLint(samples));
        }
        glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &data);
        if (data < samples)
        {
            std::cerr << "RENDERTARGET: Bad samples count, samples= " << samples << "\n";
            std::cout << "GL_MAX_COLOR_TEXTURE_SAMPLES: " << data << " < samples\n";
            samples = std::min(data, GLint(samples));
        }
        glGetIntegerv(GL_MAX_INTEGER_SAMPLES, &data);
        if (data < samples)
        {
            std::cerr << "RENDERTARGET: Bad samples count, samples= " << samples << "\n";
            std::cout << "GL_MAX_INTEGER_SAMPLES: " << data << " < samples\n";
            samples = std::min(data, GLint(samples));
        }

        this->samples_count = samples;

        glCreateFramebuffers(1, &FramebufferID);
        //GLint maxDrawBuf = 0;
        //glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuf);
    }

    void Framebuffer::resize(unsigned int width, unsigned int height)
    {
        this->Width = width;
        this->Height = height;

        if (this->mLoaded)
        {
            this->unload();
            this->load();
        }
    }

    void Framebuffer::load()
    {
        if (!this->mLoaded)
        {
            assert(mDataCopy != nullptr);
            if (mDataCopy == nullptr)
            {
                std::cerr << "ERROR::FRAMEBUFFER: mDataCopy is nullptr durning a load()!\n";
                return;
            }
            glCreateFramebuffers(1, &FramebufferID);
            for (int i = 0; i < mDataCopy->size(); i++) {
                if ((*mDataCopy)[i].mType == Framebuffer::Attachment_Type::Image)
                {
                    this->createTexture((*mDataCopy)[i].mFormat);
                }
                else
                {
                    this->createRenderBuffer((*mDataCopy)[i].mFormat);
                }
            }
            delete mDataCopy;
            mDataCopy = nullptr;
            this->mLoaded = true;
        }
    }

    void Framebuffer::unload()
    {
        if (this->mLoaded)
        {
            if (mDataCopy != nullptr) delete mDataCopy;
            mDataCopy = new std::vector<AttachmentData>;
            mDataCopy->swap(mData);
            //JustBind();
            for (int i = 0; i < mDataCopy->size(); i++) {
                if ((*mDataCopy)[i].mType == Framebuffer::Attachment_Type::Image)
                {
                    glDeleteTextures(1, &(*mDataCopy)[i].mId);
                }
                else
                {
                    glDeleteRenderbuffers(1, &(*mDataCopy)[i].mId);
                }
            }
            glDeleteFramebuffers(1, &FramebufferID);
            FramebufferID = 0;
            mColorBufferAttachmentEnums.clear();
            mData.clear();
            this->mLoaded = false;
        }
    }

    unsigned short Framebuffer::createTexture(const Image_Format IF)
    {
        const Image_Formats_Information info = Decode_IFI(IF);
        AttachmentData adata;
        adata.mType = Framebuffer::Attachment_Type::Image;
        adata.mFormat = IF;

        if (samples_count > 1) {
            glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &adata.mId);
            glTextureStorage2DMultisample(adata.mId, samples_count, info.internal_format, Width, Height, true);
        }
        else {
            glCreateTextures(GL_TEXTURE_2D, 1, &adata.mId);
            glTextureStorage2D(adata.mId, 1, info.internal_format, Width, Height);
        }
        switch (info.AttType) {
        case CONST_RenderTarget_Colour_Attachment:
            glNamedFramebufferTexture(FramebufferID, GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(mColorBufferAttachmentEnums.size()), adata.mId, 0);
            mColorBufferAttachmentEnums.push_back(GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(mColorBufferAttachmentEnums.size()));
            break;
        case CONST_RenderTarget_Depth_Attachment:
            glNamedFramebufferTexture(FramebufferID, GL_DEPTH_ATTACHMENT, adata.mId, 0);
            break;
        case CONST_RenderTarget_DepthStencil_Attachment:
            glNamedFramebufferTexture(FramebufferID, GL_DEPTH_STENCIL_ATTACHMENT, adata.mId, 0);
            break;
        default:
            std::cerr << "Null attachment type detected\n";
            std::cerr << "(HOW?)\n";
            assert(true);
            break;
        }

        if (glCheckNamedFramebufferStatus(FramebufferID, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "ERROR::FRAMEBUFFER::Framebuffer is not complete!\n";
            this->test();
        }

        mData.push_back(adata);
        return static_cast<unsigned short>(mData.size() - 1);
    }

    //TODO: why does this one use glBindFramebuffer and glCheckFramebufferStatus
    //		and the createTexture() uses glCheckNamedFramebufferStatus(), skipping the glBindFramebuffer ?
    //      not going to touch it, just in case it breaks but should be investigated

    unsigned short Framebuffer::createRenderBuffer(const Image_Format IF)
    {
        const Image_Formats_Information info = Decode_IFI(IF);
        AttachmentData adata;
        adata.mType = Framebuffer::Attachment_Type::Image;
        adata.mFormat = IF;

        glCreateRenderbuffers(1, &adata.mId);
        if (samples_count > 1) {
            glNamedRenderbufferStorageMultisample(adata.mId, samples_count, info.internal_format, Width, Height);
        }
        else {
            glNamedRenderbufferStorage(adata.mId, info.internal_format, Width, Height);
        }

        switch (info.AttType) {
        case CONST_RenderTarget_Colour_Attachment:
            glNamedFramebufferRenderbuffer(FramebufferID, GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(mColorBufferAttachmentEnums.size()), GL_RENDERBUFFER, adata.mId);
            mColorBufferAttachmentEnums.push_back(GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(mColorBufferAttachmentEnums.size()));
            break;
        case CONST_RenderTarget_Depth_Attachment:
            glNamedFramebufferRenderbuffer(FramebufferID, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, adata.mId);
            break;
        case CONST_RenderTarget_DepthStencil_Attachment:
            glNamedFramebufferRenderbuffer(FramebufferID, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, adata.mId);
            break;
        default:
            std::cerr << "Render Null attachment type detected\n";
            std::cerr << "(HOW?)\n";
            assert(true);
            break;
        }

        if (glCheckNamedFramebufferStatus(FramebufferID, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "ERROR::FRAMEBUFFER::Framebuffer is not complete!\n";
            this->test();
        }

        mData.push_back(adata);
        return static_cast<unsigned short>(mData.size() - 1);
    }


    void Framebuffer::test()
    {
        std::cout << "\033[1;34mSamples:\033[0m " << samples_count << "\n";
        for (int i = 0; i < mData.size(); i++)
        {
            Image_Formats_Information info = Decode_IFI(mData[i].mFormat);
            std::cout << "\033[1;35mAttachment[\033[0m" << i << "\033[1;35m]:\033[0m\n" <<
                "\t-\033[1;34mID:\033[0m " << mData[i].mId <<
                "\n\t-\033[1;34mformat:\033[0m " << info.format_name <<
                "\n\t-\033[1;34mis_image:\033[0m " << ((mData[i].mType == Framebuffer::Attachment_Type::Image) ? "\033[1;32mtrue\033[0m" : "\033[1;31mfalse\033[0m") << "\n";
        }
    }
}