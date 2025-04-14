#include "Labyrinth/Engine/Resource/shaderManager.hpp"

#include <filesystem>
#include <iostream>
#include <string_view>
#include <fstream>






namespace
{
    constexpr std::string_view l_const_includeIdentifier = "#include ";

    /// @brief code to deal with #include-ing
    /// @param cv_path string view to path of shader
    /// @return code of function with #include's unrolled, or empty
    std::string getCodeAndHandleIncludes(const std::string_view cv_path, bool isNotFinal = false)
    {
        std::string fullSourceCode = "";
        std::ifstream file(cv_path.data());
        
        if (!file.is_open())
        {
            std::cerr << "ERROR: could not open the shader at: " << cv_path << "\n";
            return fullSourceCode;
        }

        std::string line;
        while (std::getline(file, line))
        {
            if (line.find(l_const_includeIdentifier) != std::string::npos)
            {
                line.erase(0, l_const_includeIdentifier.size());

                if (line[0] == '\"' || line[0] == '\'')
                {
                    line = line.substr(1, line.size() - 2);
                }
                
                line.insert(0, cv_path.substr(0, cv_path.find_last_of("/\\") + 1));
                fullSourceCode += getCodeAndHandleIncludes(line, true);
                continue;
            }

            fullSourceCode += line + '\n';
        }
        if (!isNotFinal)
            fullSourceCode += '\0';

        file.close();

        return fullSourceCode;
    }

    /// @brief internal function to check for & display shader compilation errors 
    /// @param shader id of OpenGLs shader (or OpenGLs shader program)
    /// @param type "PROGRAM" if entire program, otherwise use "VERTEX", "FRAGMENT" or "COMPUTE"
    /// @return true on success, false on error
    /// @note stolen from https://learnopengl.com. Should probably change that ...
    /// @author JoeyDeVries
    static bool checkCompileErrors(const GLuint shader, const std::string_view type)
    {

        GLint success;

        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                GLint length = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
                if (length <= 0 || length > 10240) length = 1024;
                GLchar* infoLog = new GLchar[length];
                glGetShaderInfoLog(shader, length, NULL, infoLog);
                std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
                delete[] infoLog;
                return false;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                GLint length = 0;
                glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &length);
                if (length <= 0 || length > 10240) length = 1024;
                GLchar* infoLog = new GLchar[length];
                glGetProgramInfoLog(shader, length, NULL, infoLog);
                std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
                delete[] infoLog;
                return false;
            }
        }

        return true;
    }

    //RegularShader shader program filepaths:

    /// @brief vertex shader of SimpleColor
    const char* const l_shaderpath_SimpleColor_vs = "shaders/simpleColor.vs.glsl";
    /// @brief fragment shader of SimpleColor
    const char* const l_shaderpath_SimpleColor_fs = "shaders/simpleColor.fs.glsl";
    
    //ComputeShader shader program filepaths:

    /// @brief compute shader of name
    const char* const l_shaderpath_name_cs = "";
}

namespace lp::res
{
    bool ShaderManager::initialize()
    {
        {
            mRegularShaders[static_cast<int>(lp::gl::ShaderType::SimpleColor)].mVertexPath = l_shaderpath_SimpleColor_vs;
            mRegularShaders[static_cast<int>(lp::gl::ShaderType::SimpleColor)].mFragmentPath = l_shaderpath_SimpleColor_fs;

            //mRegularShaders[static_cast<int>(lp::gl::ShaderType::SimpleColor)].mFragmentPath = l_shaderpath_SimpleColor_vs;
            //mRegularShaders[static_cast<int>(lp::gl::ShaderType::SimpleColor)].mFragmentPath = l_shaderpath_SimpleColor_fs;
        }

        {
            //this->mComputeShaders[static_cast<int>(lp::gl::ShaderTypeCompute::Count)].mComputePath = l_shaderpath_name_cs;
        }

        namespace fs = std::filesystem;
        bool error_happened = false;

        for(const auto&i: this->mComputeShaders)
        {
            if(!fs::exists(i.mComputePath))
            {
                std::cerr << "ERROR: \"" << i.mComputePath << "\" is not a valid Compute Shader Path!\n";
                error_happened = true;
            }
        }
        
        for(const auto&i: this->mRegularShaders)
        {
            if(!fs::exists(i.mVertexPath))
            {
                std::cerr << "ERROR: \"" << i.mVertexPath << "\" is not a valid Vertex Shader Path!\n";
                error_happened = true;
            }
            if(!fs::exists(i.mFragmentPath))
            {
                std::cerr << "ERROR: \"" << i.mFragmentPath << "\" is not a valid Fragment Shader Path!\n";
                error_happened = true;
            }
        }

        if(error_happened)
        {
            return true;
        }

        std::cout << "inside shader Manager!\n";

        this->reloadAllShaders();

        return false; //true if error
    }

    GLuint ShaderManager::getShaderID(const lp::gl::ShaderType cv_shaderT)const
    {
        return this->mRegularShaders[static_cast<int>(cv_shaderT)].mProgramID;
    }

    GLuint ShaderManager::getShaderID(const lp::gl::ShaderTypeCompute cv_shaderT)const
    {
        return this->mComputeShaders[static_cast<int>(cv_shaderT)].mProgramID;
    }

    void ShaderManager::reloadAllShaders()
    {
        for(auto&i: this->mComputeShaders)
        {
            GLuint newProgram = this->loadShader(i.mComputePath.c_str());
            if(newProgram != 0)
            {
                if(i.mProgramID != 0) glDeleteProgram(i.mProgramID);
                i.mProgramID = newProgram;
                
            } else 
            {
                std ::cerr << "OH NOES!!!\n";
            }
        }

        for(auto&i: this->mRegularShaders)
        {
            GLuint newProgram = this->loadShader(i.mVertexPath.c_str(), i.mFragmentPath.c_str());
            if(newProgram != 0)
            {
                if(i.mProgramID != 0) glDeleteProgram(i.mProgramID);
                i.mProgramID = newProgram;
                std ::cout << "ProgramID = " << newProgram << "\n";
            } else
            {
                std ::cerr << "OH NOES!!!\n";
            }
        }
    }

    void ShaderManager::destroy()
    {
        for(auto&i: this->mComputeShaders)
        {
            glDeleteProgram(i.mProgramID); 
            i.mProgramID = 0;
        }

        for(auto&i: this->mRegularShaders)
        {
            glDeleteProgram(i.mProgramID); 
            i.mProgramID = 0;
        }
    }

    GLuint ShaderManager::loadShader(const char* vertexShaderPath, const char* fragmentShaderPath)
    {
        const std::string vsCode = getCodeAndHandleIncludes(vertexShaderPath);
        const std::string fsCode = getCodeAndHandleIncludes(fragmentShaderPath);
        if(vsCode.length() == 0 || fsCode.length() == 0) return 0; //return 0 (error) if shader code strings empty

        GLuint vertexID = 0, fragmentID = 0, ProgramID = 0;

        const char* vShaderCode = vsCode.c_str();
        const char* fShaderCode = fsCode.c_str();

        // vertex shader
        vertexID = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexID, 1, &vShaderCode, NULL);
        glCompileShader(vertexID);
        checkCompileErrors(vertexID, "VERTEX");

        // fragment Shader
        fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentID, 1, &fShaderCode, NULL);
        glCompileShader(fragmentID);
        checkCompileErrors(fragmentID, "FRAGMENT");

        ProgramID = glCreateProgram();
        glAttachShader(ProgramID, vertexID);
        glAttachShader(ProgramID, fragmentID);
        glLinkProgram(ProgramID);
        if (!checkCompileErrors(ProgramID, "PROGRAM"))
        {
            return 0;
        }
        glDeleteShader(vertexID);
        glDeleteShader(fragmentID);
        return ProgramID;
    }
    
    GLuint ShaderManager::loadShader(const char* computeShaderPath)
    {
        const std::string csCode = getCodeAndHandleIncludes(computeShaderPath);
        if(csCode.length() == 0) return 0; //return 0 (error) if shader code strings empty

        GLuint computeID = 0, ProgramID = 0;

        const char* cShaderCode = csCode.c_str();

        computeID = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(computeID, 1, &cShaderCode, NULL);
        glCompileShader(computeID);
        checkCompileErrors(computeID, "COMPUTE");

        ProgramID = glCreateProgram();
        glAttachShader(ProgramID, computeID);
        glLinkProgram(ProgramID);
        if (!checkCompileErrors(ProgramID, "PROGRAM"))
        {
            return 0;
        }
        glDeleteShader(computeID);
        return ProgramID;
    }
}