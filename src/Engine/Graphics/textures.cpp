#include "Labyrinth/Engine/Graphics/textures.hpp"



namespace
{
    using namespace lp::gl;

	/// @brief internal function to store texture data for this file only
	struct testINTERNAL_TEXTURE_INFO
	{
		/// @brief OpenGL's format of the texture
		GLenum TextureFormat = GL_NONE;
        /// @brief OpenGL's type of data (i.e. a char, int, float, etc.)
		GLenum Type = GL_UNSIGNED_BYTE;
        /// @brief OpenGL's internal format of texture
		GLenum InternalTextureFormat = GL_NONE;
	};

    /// @brief internal function to get OpenGL enums from our Format
    /// @param vForm our texture format
    /// @return OpenGL's version of our data
    inline testINTERNAL_TEXTURE_INFO INTERNAL_FUNCTION_GetInfo(const Format vForm)
    {
        testINTERNAL_TEXTURE_INFO ret;
        switch (vForm)
        {
        case Format::R8:
            ret.TextureFormat = GL_RED;
            ret.Type = GL_UNSIGNED_BYTE;
            ret.InternalTextureFormat = GL_R8;
            break;
        case Format::R16F:
            ret.TextureFormat = GL_RED;
            ret.Type = GL_FLOAT;
            ret.InternalTextureFormat = GL_R16F;
            break;
        case Format::R32F:
            ret.TextureFormat = GL_RED;
            ret.Type = GL_FLOAT;
            ret.InternalTextureFormat = GL_R32F;
            break;
        case Format::RG8:
            ret.TextureFormat = GL_RG;
            ret.Type = GL_UNSIGNED_BYTE;
            ret.InternalTextureFormat = GL_RG8;
            break;
        case Format::RG16F:
            ret.TextureFormat = GL_RG;
            ret.Type = GL_FLOAT;
            ret.InternalTextureFormat = GL_RG16F;
            break;
        case Format::RG32F:
            ret.TextureFormat = GL_RG;
            ret.Type = GL_FLOAT;
            ret.InternalTextureFormat = GL_RG32F;
            break;
        case Format::RGB8:
            ret.TextureFormat = GL_RGB;
            ret.Type = GL_UNSIGNED_BYTE;
            ret.InternalTextureFormat = GL_RGB8;
            break;
        case Format::RGB16F:
            ret.TextureFormat = GL_RGB;
            ret.Type = GL_FLOAT;
            ret.InternalTextureFormat = GL_RGB16F;
            break;
        case Format::RGB32F:
            ret.TextureFormat = GL_RGB;
            ret.Type = GL_FLOAT;
            ret.InternalTextureFormat = GL_RGB32F;
            break;
        case Format::RGBA8:
            ret.TextureFormat = GL_RGBA;
            ret.Type = GL_UNSIGNED_BYTE;
            ret.InternalTextureFormat = GL_RGBA8;
            break;
        case Format::RGBA16F:
            ret.TextureFormat = GL_RGBA;
            ret.Type = GL_FLOAT;
            ret.InternalTextureFormat = GL_RGBA16F;
            break;
        case Format::RGBA32F:
            ret.TextureFormat = GL_RGBA;
            ret.Type = GL_FLOAT;
            ret.InternalTextureFormat = GL_RGBA32F;
            break;

        default:
            //std::cerr << "Unsupported image format!\n";
            break;
        };
        return ret;
    }
}

namespace lp::gl
{
    void Texture::create(const Format cv_format, const glm::uvec2 cv_size, const void* data, std::uint_fast32_t cv_mipmapcount, bool noMipmaps)
    {
        if(noMipmaps)
        {
            cv_mipmapcount = 0;
        }
        else
        {
            if (cv_mipmapcount == 0)cv_mipmapcount = Texture::getMaxMipmapCount(cv_size);
        }
        this->mMipmapCount = cv_mipmapcount;
        this->mFormat = cv_format;
        const testINTERNAL_TEXTURE_INFO info = INTERNAL_FUNCTION_GetInfo(cv_format);
        GLuint mINT = 0;
        glCreateTextures(GL_TEXTURE_2D, 1, &mINT);
        glTextureStorage2D(mINT, cv_mipmapcount + 1, info.InternalTextureFormat, cv_size.x, cv_size.y);
        glTextureSubImage2D(mINT, 0, 0, 0, cv_size.x, cv_size.y, info.TextureFormat, info.Type, data);
        if(!noMipmaps)
            glGenerateTextureMipmap(mINT);
        this->mID = mINT;
    }

    Texture Texture::getFullColourTexture(const glm::vec4 colour)
    {
        Texture newt;
        newt.create(Format::RGBA16F, glm::uvec2(1, 1), &colour.x, 0, true);
        return newt;
    }

    std::uint_fast32_t Texture::getMaxMipmapCount(const glm::uvec2 c_size)
    {
        return static_cast<std::uint_fast32_t>(glm::floor(glm::log2(glm::max(static_cast<float>(c_size.x), static_cast<float>(c_size.y)))));
    }

    

    void Sampler::create(const MagnifictionFilter cv_mag, const MinificationFilter cv_min, const WrappingMode cv_wrapmode)
    {
        if(this->mID != 0)
        {
            glDeleteSamplers(1, &this->mID);
        }

        GLuint tempID = 0;
        glCreateSamplers(1, &tempID);
        
        switch (cv_mag)
        {
            case Sampler::MagnifictionFilter::Linear:
                glSamplerParameteri(tempID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
            case Sampler::MagnifictionFilter::Nearest:
                glSamplerParameteri(tempID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                break;
        }

        switch (cv_min)
        {
            case Sampler::MinificationFilter::Linear:
                glSamplerParameteri(tempID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                break;
            case Sampler::MinificationFilter::Nearest:
                glSamplerParameteri(tempID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                break;
            case Sampler::MinificationFilter::Nearest_MipmapNearest:
                glSamplerParameteri(tempID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                break;
            case Sampler::MinificationFilter::Nearest_MipmapLinear:
                glSamplerParameteri(tempID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
                break;
            case Sampler::MinificationFilter::Linear_MipmapNearest:
                glSamplerParameteri(tempID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
                break;
            case Sampler::MinificationFilter::Linear_MipmapLinear:
                glSamplerParameteri(tempID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                break;
        }

        switch (cv_wrapmode)
        {
            case Sampler::WrappingMode::Repeat:
                glSamplerParameteri(tempID, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glSamplerParameteri(tempID, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glSamplerParameteri(tempID, GL_TEXTURE_WRAP_R, GL_REPEAT);
                break;
            case Sampler::WrappingMode::ClampToEdge:
                glSamplerParameteri(tempID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glSamplerParameteri(tempID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glSamplerParameteri(tempID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                break;
            case Sampler::WrappingMode::ClampToBorder:
                glSamplerParameteri(tempID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                glSamplerParameteri(tempID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                glSamplerParameteri(tempID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
                break;
        }

        this->mID = tempID;
    }

    void Sampler::destroy()
    {
        glDeleteSamplers(1, &this->mID);
        this->mID = 0;
    }

    void Sampler::changeMaxAnisotropy(const float cv_val)
    {
        glSamplerParameterf(this->mID, GL_TEXTURE_MAX_ANISOTROPY, cv_val);
    }

    void Sampler::setMinLoD(const float cv_minlod)
    {
        glSamplerParameterf(this->mID, GL_TEXTURE_MIN_LOD, cv_minlod);
    }

    void Sampler::setMaxLoD(const float cv_maxlod)
    {
        glSamplerParameterf(this->mID, GL_TEXTURE_MAX_LOD, cv_maxlod);
    }

    void Sampler::setMipLoDBias(const float cv_MipLodBias)
    {
        glSamplerParameterf(this->mID, GL_TEXTURE_LOD_BIAS, cv_MipLodBias);
    }

    void Sampler::setBorderColour(const glm::vec4 cv_color)
    {
        glSamplerParameterfv(this->mID, GL_TEXTURE_BORDER_COLOR, &cv_color.x);
    }

    void Sampler::setBorderColour(const glm::ivec4 cv_color)
    {
        glSamplerParameteriv(this->mID, GL_TEXTURE_BORDER_COLOR, &cv_color.x);
    }
}