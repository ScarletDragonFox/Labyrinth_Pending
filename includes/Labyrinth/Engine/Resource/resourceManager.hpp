#ifndef LABYRINTH_PENDING_ENGINE_RESOURCE_RESOURCEMANAGER_HPP
#define LABYRINTH_PENDING_ENGINE_RESOURCE_RESOURCEMANAGER_HPP
#include <memory>

namespace lp
{
    namespace res
    {
        class ShaderManager;
    };

    class ResourceManager
    {
        public:
        /// @brief call initialize() on all its member classes
        /// @return true if error, false otherwise
        bool initialize();

        /// @brief unloads all resources (called at shutdwon only)
        void destroy();

        /// @brief get a const reference to the shader manager
        /// @return reference to shader manager
        /// @warning this requires #include-ing <Labyrinth/Engine/Resource/shaderManager.hpp>, otherwise compile error
        const lp::res::ShaderManager& getShaderManager() const { return *mShader; }

        private:
        /// @brief Shader Manager storage as a pointer to opaque, so as to not include its header file here
        std::unique_ptr<lp::res::ShaderManager> mShader;
    };
}

#endif //LABYRINTH_PENDING_ENGINE_RESOURCE_RESOURCEMANAGER_HPP