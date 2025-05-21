#ifndef LABYRINTH_PENDING_ENGINE_GRAPHICS_REGULARSHADER_HPP
#define LABYRINTH_PENDING_ENGINE_GRAPHICS_REGULARSHADER_HPP

#include "Labyrinth/Engine/Graphics/types.hpp"

#include <glad/gl.h>
#include "Labyrinth/Helpers/compilerErrors.hpp"
LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH();
#include <glm/glm.hpp>
LP_PRAGMA_DISABLE_ALL_WARNINGS_POP();

namespace lp::gl
{
    /// @brief a class representing a shader, loaded from global shader repositiory in engine.shaderManager
    class RegularShader
    {
    public:
        /// @brief loads the shader
        /// @param ST value obtained from the engine::resourceManager::shaderManager
        void LoadShader(const ShaderType ST);

        /// @brief get the currently loaded (by LoadShader()) id of shader program
        /// @return id of shader program (or 0 if no program was loaded)
        GLuint GetShaderID()const { return this->mProgramID; }

        /// @brief 'use the shader' (set as the currently used program in OpenGL)
        inline void Use(void) { glUseProgram(mProgramID); }

        /// @brief set the uniform of shader loaded by LoadShader()
        /// @param ID id (location) of the uniform variable
        /// @param value type and value of the uniform to be set
        inline void SetUniform(const GLint ID, const int value)
        {
            glProgramUniform1i(mProgramID, ID, value);
        }

        /// @brief set the uniform of shader loaded by LoadShader()
        /// @param ID id (location) of the uniform variable
        /// @param value type and value of the uniform to be set
        inline void SetUniform(const GLint ID, const unsigned int value)
        {
            glProgramUniform1ui(mProgramID, ID, value);
        }

        /// @brief set the uniform of shader loaded by LoadShader()
        /// @param ID id (location) of the uniform variable
        /// @param value type and value of the uniform to be set
        inline void SetUniform(const GLint ID, const float value)
        {
            glProgramUniform1f(mProgramID, ID, value);
        }

        /// @brief set the uniform of shader loaded by LoadShader()
        /// @param ID id (location) of the uniform variable
        /// @param value type and value of the uniform to be set
        inline void SetUniform(const GLint ID, const glm::vec2 value)
        {
            glProgramUniform2fv(mProgramID, ID, 1, &value[0]);
        }

        /// @brief set the uniform of shader loaded by LoadShader()
        /// @param ID id (location) of the uniform variable
        /// @param value type and value of the uniform to be set
        inline void SetUniform(const GLint ID, const glm::vec3 value)
        {
            glProgramUniform3fv(mProgramID, ID, 1, &value[0]);
        }

        /// @brief set the uniform of shader loaded by LoadShader()
        /// @param ID id (location) of the uniform variable
        /// @param value type and value of the uniform to be set
        inline void SetUniform(const GLint ID, const glm::vec4 value)
        {
            glProgramUniform4fv(mProgramID, ID, 1, &value[0]);
        }

        /// @brief set the uniform of shader loaded by LoadShader()
        /// @param ID id (location) of the uniform variable
        /// @param value type and value of the uniform to be set
        inline void SetUniform(const GLint ID, const glm::mat2 value)
        {
            glProgramUniformMatrix2fv(mProgramID, ID, 1, GL_FALSE, &value[0][0]);
        }

        /// @brief set the uniform of shader loaded by LoadShader()
        /// @param ID id (location) of the uniform variable
        /// @param value type and value of the uniform to be set
        inline void SetUniform(const GLint ID, const glm::mat3 value)
        {
            glProgramUniformMatrix3fv(mProgramID, ID, 1, GL_FALSE, &value[0][0]);
        }

        /// @brief set the uniform of shader loaded by LoadShader()
        /// @param ID id (location) of the uniform variable
        /// @param value type and value of the uniform to be set
        inline void SetUniform(const GLint ID, const glm::mat4 value)
        {
            glProgramUniformMatrix4fv(mProgramID, ID, 1, GL_FALSE, &value[0][0]);
        }
    private:
        GLuint mProgramID = 0; //id of the shader program
    };
}

#endif //LABYRINTH_PENDING_ENGINE_GRAPHICS_REGULARSHADER_HPP