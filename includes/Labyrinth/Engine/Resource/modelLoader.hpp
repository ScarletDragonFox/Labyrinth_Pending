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

#include "Labyrinth/Engine/Resource/loadedModel.hpp"

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

    //TODO: strings need to be hashed every time
    // use a typdef'd int for ids
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
        /// @param cv_name 'name' ("path") of the model to get
        /// @return const pointer if loaded, nullptr if not loaded
        LoadedModel const* getLoadedModel(const std::string_view cv_name)const;

        /// @brief schedule to load a model
        /// @param cv_name name/path of the model to load
        void scheduleLoad(const std::string_view cv_name);

        /// @brief unloads a model from memory
        /// @param cv_name name/path of model
        /// @return true if error, false if succeded
        bool unload(const std::string_view cv_name);

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
        std::unordered_map<std::string, std::unique_ptr<LoadedModel>> mModels;

        /// @brief storage for all loading models
        std::list<std::future<std::unique_ptr<LoadedModel>>> mLoaders;
    };
}

#endif //LABYRINTH_PENDING_ENGINE_RESOURCE_MODELLOADER_HPP