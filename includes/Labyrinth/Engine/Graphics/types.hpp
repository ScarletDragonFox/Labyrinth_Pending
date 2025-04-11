#ifndef LABYRINTH_PENDING_ENGINE_GRAPHICS_TYPES_HPP
#define LABYRINTH_PENDING_ENGINE_GRAPHICS_TYPES_HPP

#include <glad/gl.h>
#include <string_view>

namespace lp::gl
{
    /// @brief enum for all different shaders (vertex/fragment shaders types ONLY)
    enum class ShaderType
    {
        /// @brief Simple Color Shader
        ///
        /// As input takes in a vec3 position & uniform vec3 colour TODO: update this description!!!
        ///
        /// Outputs the given color to the framebuffer in the 0-slot
        SimpleColor,

        /// @brief Last ''shader'', serves as an invalid value and a count of how many shader there are in here 
        /// @warning DO NOT TOUCH! must be last
        Count
    };

    /// @brief enum for all different compute shaders
    enum class ShaderTypeCompute
    {

        /// @brief Last ''shader'', serves as an invalid value and a count of how many shader there are in here 
        /// @warning DO NOT TOUCH! must be last
        Count
    };

    /// @brief get name of shader
    /// @param cv_shadertype vertex/fragment enum shader type
    /// @return string_view to const char* name
    constexpr std::string_view getName(const ShaderType cv_shadertype)
    {
        const char* names[] = {
            "SimpleColor",
            "Count"
        };
        return names[static_cast<int>(cv_shadertype)];
    }

    /// @brief get name of shader
    /// @param cv_shadertype compute enum shader type
    /// @return string_view to const char* name
    constexpr std::string_view getName(const ShaderTypeCompute cv_shadertype)
    {
        const char* names[] = {
            "Count"
        };
        return names[static_cast<int>(cv_shadertype)];
    }
}

#endif //LABYRINTH_PENDING_ENGINE_GRAPHICS_TYPES_HPP