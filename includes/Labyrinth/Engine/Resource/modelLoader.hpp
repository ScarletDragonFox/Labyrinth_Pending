#ifndef LABYRINTH_PENDING_ENGINE_RESOURCE_MODELLOADER_HPP
#define LABYRINTH_PENDING_ENGINE_RESOURCE_MODELLOADER_HPP

#include <memory>
#include <future>
#include <list>
#include <vector>
#include <unordered_map>
#include <mutex> //std::mutex, std::scoped_lock
#include <queue>
#include <latch>
#include <filesystem>

#include <glad/gl.h>
#include <string>
#include <string_view>

#include "Labyrinth/Engine/Graphics/textures.hpp"

#include "Labyrinth/Engine/Resource/loadedModel.hpp" //defines lp::res::Model_t

#include "Labyrinth/Engine/Resource/modelLifetimeSystem.hpp"

/// @brief forward declaration of aiMesh
struct aiMesh;

namespace lp::res
{
    /// @brief the Vertex structure that all models loaded by @ref ModelLoader use
    struct VertexFull
    {
        glm::vec3 mPosition = {};
        glm::vec2 mTextureCoords = {};
        glm::vec3 mNormal = {};
        glm::vec3 mTangent = {};
        // glm::vec3 mBitangent = {};
    };

    //TODO: find way to delete stray threads of std::async on destructor

    /// @brief Model loader.
    /// 
    /// Loads & unloads models+textures asynchronously
    class ModelLoader
    {
        public:

        /// @brief initialize the model loader.
        ///
        /// Sets up Assimp logging mainly.
        void initialize();

        /// @brief Runs 'Tasks' up to cv_maxTime seconds
        /// @param cv_maxTime max time to spend loading data into OpenGL
        void update(const double cv_maxTime);

        /// @brief Return a previously loaded model via const pointer, or nullptr if not loaded
        /// @param cv_id id of the model to load
        /// @return const pointer if loaded, nullptr if not loaded
        LoadedModel const* getLoadedModel(const ModelID_t cv_id)const;

        /// @brief get a shared_ptr to the model
        /// @param cv_id id of the model to load
        /// @return ptr or nullptr if not loaded
        std::shared_ptr<LoadedModel> getModelRef(const ModelID_t cv_id);

        /// @brief schedule to load a model
        /// @param cv_name name/path of the model to load
        /// @return id of the model
        ModelID_t scheduleLoad(const std::string_view cv_name);

        /// @brief checks if the given id is valid
        /// @param cv_id id to check
        /// @return true if the model with that id is loaded, false otherwise
        bool isValid(const ModelID_t cv_id) const;

        /// @brief unloads a model from memory
        /// @param cv_name id of the model
        /// @return true if error, false if succeded
        bool unload(const ModelID_t cv_id);

        private:

        /// @brief data used by taskMesh()
        struct TaskMeshData
        {
            /// @brief mesh (pointer) to output to.
            ///
            /// Should never be nullptr!
            LoadedModel::Mesh* mMesh = nullptr;
            /// @brief container of input verticies
            std::vector<lp::res::VertexFull> mVerticies;
            /// @brief container of input indicies
            std::vector<unsigned int> mIndicies;
            /// @brief latch to count_down() after mesh is loaded
            std::shared_ptr<std::latch> mLatchPtr;
        };

        /// @brief data used by taskTexture
        struct TaskTextureData
        {
            /// @brief file this texture comes from
            std::filesystem::directory_entry mFile = {};
            /// @brief Texure (pointer) to output to.
            ///
            /// Should never be nullptr!
            lp::gl::Texture* mTex = nullptr;
            /// @brief format of the texture to load
            lp::gl::Format mTexFormat = lp::gl::Format::R8;
            /// @brief size of the texture
            glm::uvec2 mTexSize = {};
            /// @brief mipmap count of the texture to load
            std::uint_fast32_t mMipmapCount = 0;
            /// @brief should this texture have mipmaps?
            bool noMipmaps = false;
            /// @brief storage of raw image data with mTexFormat Format
            ///
            /// dont't forget to stbi_image_free() after use!
            void* mTextureData = nullptr;
            /// @brief latch to count_down() after texture is loaded
            std::shared_ptr<std::latch> mLatchPtr;
        };

        /// @brief temp structure soo the future returns both a ptr & an id
        struct LambdaReturnStructure
        {
            std::shared_ptr<lp::res::LoadedModel> mPtr;
            lp::res::ModelID_t id = lp::res::const_id_model_invalid;
        };

        /// @brief loads a single mesh into OpenGL
        /// @param data input/output data
        void taskMesh(TaskMeshData& data);

        /// @brief loads a single texture into OpenGL & free()'s previously allocated memory
        /// @param data input/output data
        void taskTexture(TaskTextureData& data);

        /// @brief internal function load a texture
        /// @param data output of the function
        /// @param path path/name of image
        /// @param directory directory the path/name is in
        /// @return true on error, false otherwise
        static bool loadTexture(TaskTextureData& data, const char* path, const std::filesystem::path directory);

        /// @brief process & convert a aiMesh to TaskMeshData
        /// @param mesh assimp aiMesh to convert
        /// @param output where to put the results
        void int_processMesh(aiMesh* mesh, lp::res::ModelLoader::TaskMeshData& v_output);

        /// @brief queue of Mesh-load tasks to finish.
        ///
        /// Used by multiple threads, use mTasksMeshLoadMutex when push-ing or pop-ing
        std::queue<ModelLoader::TaskMeshData> mTasksMeshLoad;
        /// @brief mutex for the mTasksMeshLoad container.
        std::mutex mTasksMeshLoadMutex;
        
        /// @brief queue of Texture-load tasks to finish.
        ///
        /// Used by multiple threads, use mTasksTextureLoadMutex when push-ing or pop-ing
        std::queue<ModelLoader::TaskTextureData> mTasksTextureLoad;
        /// @brief mutex for the mTasksTextureLoad container.
        std::mutex mTasksTextureLoadMutex;

        /// @brief storage for all loaded models
        std::unordered_map<ModelID_t, std::shared_ptr<LoadedModel>> mModels;

        /// @brief Map of Model ID -> filename/path
        std::unordered_map<ModelID_t, std::string> mModelNames;

        /// @brief storage for all loading models
        std::list<std::future<LambdaReturnStructure>> mLoaders;

        /// @brief id of last loaded model
        lp::res::ModelID_t mLastModelID = lp::res::const_id_model_invalid;

        /// @brief pointer loaded by g_engine.getECS()
        std::shared_ptr<lp::res::SystemModelLifetime> mSystemModelLifetime;
    };
}

#endif //LABYRINTH_PENDING_ENGINE_RESOURCE_MODELLOADER_HPP