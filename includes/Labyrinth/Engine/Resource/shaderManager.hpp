#ifndef LABYRINTH_PENDING_ENGINE_RESOURCE_SHADERMANAGER_HPP
#define LABYRINTH_PENDING_ENGINE_RESOURCE_SHADERMANAGER_HPP

#include "Labyrinth/Engine/Graphics/types.hpp"

#include <glad/gl.h>
#include <string>
#include <array>
#include <unordered_map>

namespace lp::res
{
    
    /// @brief Shader Manager class that:
    ///
    /// - compiles shaders vertex/fragment AND compute ones
    ///
    /// - can recompile all shaders when reloadAllShaders() is called
    /// @link https://www.khronos.org/opengl/wiki/Shader_Compilation
    class ShaderManager
    {
        public:

        /// @brief Initializes the class & loads all the shaders
        ///
        /// Exists in place of a constructor, because it could error
        ///
        /// And I don't want to have to catch an Exception
        /// @return true if error, false otherwise
        bool initialize();

        /// @brief get OpenGLs shader ID from our ShaderType
        /// @param cv_shaderT vertex/fragment shader enum type
        /// @return OpenGLs ID of querried program ('shader')
        GLuint getShaderID(const lp::gl::ShaderType cv_shaderT)const;

        /// @brief get OpenGLs shader ID from our ShaderType
        /// @param cv_shaderT compute shader enum type
        /// @return OpenGLs ID of querried program ('shader')
        GLuint getShaderID(const lp::gl::ShaderTypeCompute cv_shaderT)const;

        /// @brief Reloads all the shaders
        ///
        /// If a shader failed to compile, previous version will be used
        void reloadAllShaders();

        /// @brief Reloads the specified shader
        /// @param cv_shaderT vertex/fragment shader to reload
        void reloadShader(const lp::gl::ShaderType cv_shaderT);

        /// @brief Reloads the specified shader
        /// @param cv_shaderT compute shader to reload
        void reloadShader(const lp::gl::ShaderTypeCompute cv_shaderT);

        /// @brief unloads all OpenGL shaders
        void destroy();

        /// @brief public map of define -> value
        ///
        /// translates to: #define key value
        ///
        /// with key + value taken from this map
        std::unordered_map<std::string, std::string> mShaderDefines;

        private:

        /// @brief struct that stores data for a RegularShader (like its ID)
        ///
        /// Stores the path to the vertex + fragment shaders
        struct VFShData
        {
            std::string mVertexPath; //path to vertex shader of this 'program'
            std::string mFragmentPath; //path to fragment shader of this 'program'
            GLuint mProgramID = 0; //OpenGLs ID of this program (which we call a shader)
            //4 bytes of padding
        };

        /// @brief struct that stores data for a ComputeShader (like its ID)
        ///
        /// Stores the path to the vertex + fragment shaders
        struct CShData
        {
            std::string mComputePath; //path to compute shader of this 'program'
            GLuint mProgramID = 0; //OpenGLs ID of this program (which we call a shader)
        };

        /// @brief internal function to load a shader program from a filepath
        /// @param vertexShaderPath path to vertex shader file
        /// @param fragmentShaderPath path to fragment shader file
        /// @param shader_defines view to string result of processDefines()
        /// @return id of Shader, or 0 if error
        GLuint loadShader(const char* vertexShaderPath, const char* fragmentShaderPath, const std::string_view shader_defines);

        /// @brief internal function to load a shader program from a filepath
        /// @param computeShaderPath path to compute shader file
        /// @param shader_defines view to string result of processDefines()
        /// @return id of Shader, or 0 if error
        GLuint loadShader(const char* computeShaderPath, const std::string_view shader_defines);

        /// @brief array storing vertex/fragment shader program data
        std::array<VFShData, static_cast<int>(lp::gl::ShaderType::Count)> mRegularShaders;
        /// @brief array storing compute shader program data
        std::array<CShData, static_cast<int>(lp::gl::ShaderTypeCompute::Count)> mComputeShaders;

        /// @brief internal function to transform mShaderDefines -> processed string
        std::string processDefines()const;

        /// @brief internal function that insertes the output of processDefines() AFTER the #version string
        /// @param r_out the 'code' of the shader
        /// @param cv_defines the string to insert, the output of processDefines()
        void insertDefines(std::string& r_out, const std::string_view cv_defines)const;
    };
}

#endif //LABYRINTH_PENDING_ENGINE_RESOURCE_SHADERMANAGER_HPP