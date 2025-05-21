#ifndef LABYRINTH_PENDING_ENGINE_GRAPHICS_TEXTURES_HPP
#define LABYRINTH_PENDING_ENGINE_GRAPHICS_TEXTURES_HPP

#include "Labyrinth/Helpers/compilerErrors.hpp"

LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH();
#include <glad/gl.h>
#include <glm/glm.hpp>
LP_PRAGMA_DISABLE_ALL_WARNINGS_POP();

namespace lp::gl
{
    /// @brief typdef around GLuint so as not confuse it with an actual number
    ///
    /// this type represents the id of the texture/sampler slot up to the OpenGL maximum queried by glGetInteger
    typedef GLuint TextureUnitType;

    /// @brief enum class representing the common (and universal) formats of a textures
    ///
    /// Stored this way for type-safety.
    ///
    /// OpenGL just uses #define's, so any number could be given instead (which would break them & would be hard to debug easily)
    enum class Format
    {
        R8,
        R16F,
        R32F,
        RG8,
        RG16F,
        RG32F,
        RGB8,
        RGB16F,
        RGB32F,
        RGBA8,
        RGBA16F,
        RGBA32F,
    };

    /// wrapper around OpenGL's texture
    class Texture
    {
    public:
        /// @brief create a new Texture (or replaces an existing one)
        /// @param cv_format - format of the texture
        /// @param cv_size - format of the texture
        /// @param data - pointer to the texture data as specified by cv_format and cv_size
        /// @param cv_mipmapcount - format of the texture (if 0, than calculated internally)
        /// @param noMipmaps if true texture will be created with no mipmaps
        void create(const Format cv_format, const glm::uvec2 cv_size, const void* data, std::uint_fast32_t cv_mipmapcount = 0, bool noMipmaps = false);

        /// @brief obtain a new Texture containing a 1x1 vec4 colour
        /// @param colour - the colour of the texture
        /// @return returns a new Texture object
        static Texture getFullColourTexture(const glm::vec4 colour);

        /// @brief Gets this textures mipmap count
        /// @returns mipmap count
        inline std::uint_fast32_t getMipmapCount()const { return mMipmapCount; }

        /// @brief Calculate the max OpenGL mipmaps for a given size of a texture.
        /// @param c_size - size of the texture
        /// @returns max mipmaps (and a + 1?)
        static std::uint_fast32_t getMaxMipmapCount(const glm::uvec2 c_size);

        /// @brief Get the format of the texture.
        /// @returns Texture::Format of the texture
        inline Format getTextureFormat()const { return mFormat; }

        /// @brief get the internal OpenGL texture ID
        /// @return OpenGL texture ID
        inline GLuint getID() const { return mID; }

        /// @brief bind this texture to specified slot
        /// @param cv_TUT - the texture unit slot number to bind the texture to
        inline void Bind(const TextureUnitType cv_TUT) const
        {
            glBindTextureUnit(static_cast<GLuint>(cv_TUT), this->mID);
        }

        /// @brief unbind textures from specified slot
        /// @param cv_TUT - the texture unit slot number to clear texture from
        inline static void Unbind(const TextureUnitType cv_TUT) 
        {
            glBindTextureUnit(static_cast<GLuint>(cv_TUT), 0);
        }

    private:
        Format mFormat = Format::R8;
        GLuint mID = 0;
        std::uint_fast32_t mMipmapCount = 1;
    };

    /// @brief Warpper around OpenGL's samplers
    class Sampler
    {
    public:
        /// @brief the magnification filter is used when the texture is closer to the camera
        enum class MagnifictionFilter
        {
            Nearest, Linear
        };

        /// @brief the magnification filter is used to make the texture smaller
        enum class MinificationFilter
        {
            /// @brief the 2 types with no mipmaps
            Nearest, Linear,
            Nearest_MipmapNearest, //nearest filtering in texture and nearest between the diffrent mapmaps
            Linear_MipmapNearest,  //linear filtering in texture and nearest between the diffrent mapmaps
            Nearest_MipmapLinear,  //nearest filtering in texture and linear between the diffrent mapmaps
            Linear_MipmapLinear    //linear filtering in texture and linear between the diffrent mapmaps
        };

        /// @brief warppping mode of the sampler
        enum class WrappingMode
        {
            Repeat, //the texture is repeated
            ClampToEdge, //the texture is clamped to the edge, after that the colours of the edge are extended
            ClampToBorder, //the texture is clamped to the border, the colour of witch is determined by setBorderColour
        };
    public:
        /// @brief creates or replaces the Sampler
        /// @param cv_mag - the magnifictaion filter - used for making textures bigger
        /// @param cv_min - the minification filter  - used for making textures smaller
        /// @param cv_wrapmode - wrapping mode
        void create(const MagnifictionFilter cv_mag, const MinificationFilter cv_min, const WrappingMode cv_wrapmode);

        /// @brief Destroys the Sampler
        void destroy();

        /// @brief Change samplers mMaxAnisotropy to cv_val.
        /// 
        /// 1.0f (off) to glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &value)
        /// @param cv_val - 1.0f is off (no anisotropy)
        void changeMaxAnisotropy(const float cv_val = 1.0);

        /// @brief Set (min LoD) distance
        /// 
        /// Sets the distance from the camera where the texture's chosen Mipmap will be smallest one
        /// @param cv_minlod - default is -1000.0f
        void setMinLoD(const float cv_minlod = -1000.0f);

        /// @brief Set (max LoD) distance
        /// 
        /// Sets the distance from the camera where the texture's chosen Mipmap will be largest one
        /// @param cv_maxlod - default is 1000.0f
        void setMaxLoD(const float cv_maxlod = 1000.0f);

        /// @brief Sets the samplers LoD Bias
        /// @param cv_MipLodBias - default is 0.0
        void setMipLoDBias(const float cv_MipLodBias = 0.0f);

        /// @brief sets the colour of the texture's border - float version
        /// @param cv_color - border colour with (color.a == 0.0f) being 'transparent'
        void setBorderColour(const glm::vec4 cv_color);

        /// @brief sets the colour of the texture's border - int version
        /// @param cv_color - border colour with (color.a == 0) being 'transparent'
        void setBorderColour(const glm::ivec4 cv_color);

        /// @brief bind this sampler to the specified slot
        /// @param cv_TUT - the texture unit slot number to bind the sampler to
        inline void Bind(const TextureUnitType cv_TUT)
        {
            glBindSampler(static_cast<GLuint>(cv_TUT), this->mID);
        }

        /// @brief unbind sampler from specified slot
        /// @param cv_TUT - the texture unit slot number to clear the sampler from
        inline static void Unbind(const TextureUnitType cv_TUT)
        {
            glBindSampler(static_cast<GLuint>(cv_TUT), 0);
        }

        /// @brief get OpenGL id of the sampler
        /// @return 
        inline GLuint getID() const { return mID; }

    private:
        GLuint mID = 0;
    };
}

#endif //LABYRINTH_PENDING_ENGINE_GRAPHICS_TEXTURES_HPP