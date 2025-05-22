#ifndef LABYRINTH_PENDING_ENGINE_RESOURCE_RESOURCEMANAGER_HPP
#define LABYRINTH_PENDING_ENGINE_RESOURCE_RESOURCEMANAGER_HPP
#include <memory>

#include <glm/mat4x4.hpp>
#include <string>
#include <string_view>
#include <unordered_map>

#include "Labyrinth/Engine/Resource/modelLoader.hpp"
#include "Labyrinth/Engine/Resource/loadedModel.hpp"

namespace lp
{
    namespace res
    {
        /// @brief forward declaration of ShaderManager class
        class ShaderManager;
    };

    /// @brief Component that stores 'Models'
    ///
    /// @todo TODO: make this aware of owning of a model if & informing the Model Manager when given/destroyed
    struct ModelComponent
    {
        /// @brief id of model
        res::ModelID_t mModelID = res::const_id_model_invalid;
        glm::mat4 mModel = glm::mat4(1.0f); //temppppppppppppppp
    };

    class ResourceManager
    {
        public:
        /// @brief call initialize() on all its member classes
        /// @return true if error, false otherwise
        bool initialize();

        /// @brief unloads all resources (called at shutdwon only)
        void destroy();

        /// @brief get a reference to the shader manager
        /// @return reference to shader manager
        /// @warning this requires #include-ing <Labyrinth/Engine/Resource/shaderManager.hpp>, otherwise compile error
        lp::res::ShaderManager& getShaderManager() const { return *mShader; }

        /// @brief get a reference to the model loader. Avoid using!
        /// @return reference to the model loader
        lp::res::ModelLoader& getModelLoaderRef() { return mModel; }

        /// @brief Load a model
        /// @param cv_path path to the model
        /// @return id (may be invalid check with isValidModel()!)
        lp::res::ModelID_t loadModel(const std::string_view cv_path);

        /// @brief Return a previously loaded model via const pointer, or nullptr if not loaded
        /// @param cv_id id of the model to get
        /// @return const pointer if loaded, nullptr if not loaded
        /// @see isValidModel()
        lp::res::LoadedModel const* getLoadedModel(const lp::res::ModelID_t cv_id)const;

        /// @brief checks if given model id refers to a model currently loaded
        /// @param cv_id id of model to check
        /// @return true if valid, false otherwise
        bool isValidModel(const lp::res::ModelID_t cv_id) const;

        private:
        /// @brief Shader Manager storage as a pointer to opaque, so as to not include its header file here
        std::unique_ptr<lp::res::ShaderManager> mShader;
        /// @brief model loader & container of loaded/loading models with strings to access
        lp::res::ModelLoader mModel;
    };
}

#endif //LABYRINTH_PENDING_ENGINE_RESOURCE_RESOURCEMANAGER_HPP