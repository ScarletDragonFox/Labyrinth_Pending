#include "Labyrinth/Engine/Resource/modelLoader.hpp"

#include "Labyrinth/Helpers/compilerErrors.hpp"

#include <chrono> 
#include <iostream> //std::cerr

LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH()

#include <stb_image.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/postprocess.h>

LP_PRAGMA_DISABLE_ALL_WARNINGS_POP()

namespace
{
    /// @brief internal function to load, process & load the node hierarchy + its meshes
    /// @param node input node
    /// @param scene scene that contains this all
    /// @param output output mesh vector
    void int_processNodeRecursiveFlatten(aiNode* node, const aiScene* scene, std::vector<aiMesh*>& output);

    /// @brief process & convert a aiMesh to LoadingModel::TaskMeshData
    /// @param mesh assimp aiMesh to convert
    /// @param output where to put the results
    void int_processMesh(aiMesh* mesh, lp::res::ModelLoader::LoadingModel::TaskMeshData& v_output);

    constexpr inline glm::vec3 int_convert_Assimp(aiVector3f& vec)
    {
        return {vec.x, vec.y, vec.z};
    }
}

namespace lp::res
{
    void ModelLoader::initialize()
    {
        //TODO: below is an example of logging
        // - multithreading????
        //https://github.com/assimp/assimp/blob/master/samples/SimpleTexturedOpenGL/SimpleTexturedOpenGL/src/model_loading.cpp
    }

    void ModelLoader::update(const double cv_maxTime)
    {
        if(mLoaders.size() == 0) return; //early return if no models are being loaded

        using schr = std::chrono::high_resolution_clock;
        std::chrono::time_point<schr> m_StartTime = schr::now(); // effectively just system_clock, but meh
        using FpSeconds = std::chrono::duration<double, std::chrono::seconds::period>;
        static_assert(std::chrono::treat_as_floating_point<FpSeconds::rep>::value, "Rep required to be floating point");

        for(auto it = mLoaders.begin(); it != mLoaders.end();)
        {
            using namespace std::chrono_literals;
            if((*it).valid() && (*it).wait_for(1ns) == std::future_status::ready)
            {
                std::unique_ptr<LoadedModel> mdl = it->get();
                mModels[mdl->mFile.string()] = std::move(mdl);
                it = mLoaders.erase(it);
            } else {
                ++it;
            }
        }

        while(true)
        {
            LoadingModel::TaskTextureData data;
            {
                std::scoped_lock<std::mutex> LOK(this->mTasksTextureLoadMutex);
                if(mTasksTextureLoad.empty()) break;
                std::swap(data, mTasksTextureLoad.front());
                mTasksTextureLoad.pop();
            }

            this->taskTexture(data);
            const auto f_secs = FpSeconds(schr::now() - m_StartTime).count();
            if(f_secs > cv_maxTime) break;
        }

        //return if timer reached
        if(FpSeconds(schr::now() - m_StartTime).count() > cv_maxTime) return; 

        while(true)
        {
            LoadingModel::TaskMeshData data;
            {
                std::scoped_lock<std::mutex> LOK(this->mTasksMeshLoadMutex);
                if(mTasksMeshLoad.empty()) break;
                std::swap(data, mTasksMeshLoad.front());
                mTasksMeshLoad.pop();
            }

            this->taskMesh(data);
            const auto f_secs = FpSeconds(schr::now() - m_StartTime).count();
            if(f_secs > cv_maxTime) break;
        }

    }

    LoadedModel const* ModelLoader::getLoadedModel(const std::string_view cv_name)const
    {
        const std::string copy = std::string(cv_name);
        if(mModels.contains(copy)) //unfortunately, we need a copy here :(
        {
            return mModels.at(copy).get();
        } else{
            return nullptr;
        }
    }

    bool ModelLoader::unload(const std::string_view cv_name)
    {
        const std::string copy = std::string(cv_name);
        if(!mModels.contains(copy))
        {
            return true;
        }
        mModels.erase(copy);
        return false;
    }

    void ModelLoader::scheduleLoad(const std::string_view cv_name)
    {
        if(mModels.contains(std::string(cv_name))) return; //early return on model already being loaded

        const auto loader_lambda = [this](const std::string_view cv_filename) -> std::unique_ptr<LoadedModel>
        {
            const std::filesystem::path c_path(cv_filename);
            if(!std::filesystem::is_regular_file(c_path))
            {
                return nullptr;
            }
            const std::string directory = c_path.parent_path().string();


            std::unique_ptr<LoadedModel> resultModelActualStorage = std::make_unique<LoadedModel>();

            LoadedModel& outputModel = *resultModelActualStorage;
            outputModel.mFile = c_path;

            constexpr unsigned int Flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices;// | aiProcess_GenUVCoords | aiProcess_SortByPType;
            Assimp::Importer importer;
            
            const aiScene* scene = importer.ReadFile(cv_filename.data(), Flags);
            std::cerr << "After importer.ReadFile()\n";
            // check for errors
            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
            {
                std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
                return nullptr;
            }

            std::shared_ptr<std::latch> vLatchTextures = std::make_shared<std::latch>(static_cast<std::ptrdiff_t>(scene->mNumMaterials));

            outputModel.mMaterials.resize(scene->mNumMaterials);

            std::cerr << "Before Txtures!\n";

            ///process textures
            for (unsigned int i = 0; i < scene->mNumMaterials; i++)
            {
                aiMaterial* material = scene->mMaterials[i];
                LoadingModel::TaskTextureData data;

                data.mTex = &(outputModel.mMaterials[i].mColor);
                data.mLatchPtr = vLatchTextures;

                bool colorNOTLoaded = true;

                if(colorNOTLoaded)
                    if (material->GetTextureCount(aiTextureType_BASE_COLOR) > 0)
                    {
                        aiString str;
                        material->GetTexture(aiTextureType_BASE_COLOR, 0, &str);
                        if(loadTexture(data, str.C_Str(), directory) == false)
                        {
                            //texture loaded correctly
                            colorNOTLoaded = false;
                        }
                    }
                if(colorNOTLoaded)
                    if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
                    {
                        aiString str;
                        material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
                        if(loadTexture(data, str.C_Str(), directory) == false)
                        {
                            //texture loaded correctly
                            colorNOTLoaded = false;
                        }
                    }
                if(colorNOTLoaded)
                {
                    aiColor4D temp_color;
                    
                    if (AI_SUCCESS != aiGetMaterialColor(material, AI_MATKEY_BASE_COLOR, &temp_color))
                    {
                        if (AI_SUCCESS != aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &temp_color))
                        {
                            temp_color = aiColor4D(1.f, 1.f, 1.f, 1.f);
                        }
                    }
                    float* dt = new float[4];
                    dt[0] = temp_color.r;
                    dt[1] = temp_color.g;
                    dt[2] = temp_color.b;
                    dt[3] = temp_color.a;
                    data.mTextureData = dt;
                    data.mMipmapCount = 0;
                    data.noMipmaps = true;
                    data.mTexSize = { 1, 1};
                    data.mTexFormat = lp::gl::Format::RGBA32F;
                }
                {
                    std::scoped_lock<std::mutex> LOK(this->mTasksTextureLoadMutex);
                    mTasksTextureLoad.push(data);
                }
            }
            
            std::cerr << "Before Node Flattening!\n";


            std::vector <aiMesh*> assimp_meshes;
            assimp_meshes.reserve(scene->mNumMeshes);

            int_processNodeRecursiveFlatten(scene->mRootNode, scene, assimp_meshes);

            outputModel.mMeshes.resize(assimp_meshes.size());

            std::sort(assimp_meshes.begin(), assimp_meshes.end(), [](const aiMesh* a, aiMesh* b) -> bool
            {
                return a->mMaterialIndex < b->mMaterialIndex; //sort all meshes ascending based on material, to limit material changes during render
            });

            std::shared_ptr<std::latch> vLatchMeshes = std::make_shared<std::latch>(static_cast<std::ptrdiff_t>(assimp_meshes.size()));

            std::cerr << "Before Mesh Processing!\n";

            for(std::size_t i = 0; i < assimp_meshes.size(); i++)
            {
                lp::res::ModelLoader::LoadingModel::TaskMeshData data;
                data.mLatchPtr = vLatchMeshes;
                data.mMesh = &(outputModel.mMeshes[i]);
                std::cerr << "processing mesh #" << i << " name = \"" <<assimp_meshes[i]->mName.C_Str() << "\"\n";
                int_processMesh(assimp_meshes[i], data);
                std::cerr << "finished!\n";
                {
                    std::scoped_lock<std::mutex> LOK(this->mTasksMeshLoadMutex);
                    mTasksMeshLoad.push(data);
                }
            }

            //wait for all tasks to finish before returning

            std::cerr << "Model \"" << c_path.filename().string() << "\" is waiting for tasks to finish!\n";

            std::cerr << "Model has " << scene->mNumMaterials << " materials & " << scene->mNumMeshes << " meshes!\n";
            std::cerr << "Model had " << assimp_meshes.size() << " meshes!\n"; 

            vLatchTextures->wait();
            vLatchMeshes->wait();
            
            if(vLatchTextures.use_count() > 1 || vLatchMeshes.use_count() > 1)
            {
                std::cerr << "A horrific error has occurred!!!!\n";
                std::cerr << "You should shut down this game, a model refused to be loaded and memory leaked all over!\n";
            }

            std::cerr << "Model \"" << c_path.filename().string() << "\" has finished loading!\n";

            return resultModelActualStorage;
        };

        mLoaders.push_back(std::async(loader_lambda, cv_name));
    }

    bool ModelLoader::loadTexture(LoadingModel::TaskTextureData& data, const char* path, const std::filesystem::path directory)
    {
        const std::filesystem::path ppaatthh = directory/path; // append filename to directory
        data.mFile = std::filesystem::directory_entry(ppaatthh);
        
        int vX = 0, vY = 0, vC = 1;

        data.mTextureData = stbi_load(ppaatthh.string().c_str(), &vX, &vY, &vC, 0);

        if(data.mTex == nullptr)
        {   
            return true;
        }
        data.mTexSize = {static_cast<unsigned int>(vX), static_cast<unsigned int>(vY)};
        data.noMipmaps = false;
        data.mMipmapCount = lp::gl::Texture::getMaxMipmapCount(data.mTexSize);

        switch (vC)
        {
        case 1:
            data.mTexFormat = lp::gl::Format::R8;
            break;
        case 2:
            data.mTexFormat = lp::gl::Format::RG8;
            break;
        case 3:
            data.mTexFormat = lp::gl::Format::RGB8;
            break;
        case 4:
            data.mTexFormat = lp::gl::Format::RGBA8;
            break;
        default:
            //std::cerr << "ERROR::ASSIMP::TEXTURE_INFO: Non-Standard component number - " << nrComponents << "\n";
            return true;
        }

        return false;
    }


    void ModelLoader::taskMesh(LoadingModel::TaskMeshData& data)
    {
        GLuint VBO = 0;
        GLuint EBO = 0;
        
        glCreateBuffers(1, &VBO);
        glCreateBuffers(1, &EBO);

        glNamedBufferStorage(VBO, sizeof(ModelLoader::LoadingModel::Vertex) * data.mVerticies.size(), data.mVerticies.data(), 0);
        glNamedBufferStorage(EBO, sizeof(unsigned int) * data.mIndicies.size(), data.mIndicies.data(), 0);

        data.mMesh->mEBO = EBO;
        data.mMesh->mVBO = VBO;

        data.mLatchPtr->count_down();
    }
    void ModelLoader::taskTexture(LoadingModel::TaskTextureData& data)
    {
        data.mTex->create(data.mTexFormat, data.mTexSize, data.mTextureData, data.mMipmapCount, data.noMipmaps);
        stbi_image_free(data.mTextureData);
        data.mLatchPtr->count_down();
    }

}

namespace
{
    void int_processNodeRecursiveFlatten(aiNode* node, const aiScene* scene, std::vector<aiMesh*>& output)
    {
        //reserve size in vector for new mesh pointers for output
        output.reserve(output.size() + node->mNumMeshes);

        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            output.push_back(scene->mMeshes[node->mMeshes[i]]);
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            int_processNodeRecursiveFlatten(node->mChildren[i], scene, output);
        }
    }

    void int_processMesh(aiMesh* mesh, lp::res::ModelLoader::LoadingModel::TaskMeshData& v_output)
    {
        v_output.mVerticies.reserve(mesh->mNumVertices);
        v_output.mIndicies.reserve(std::size_t(mesh->mNumFaces) * 3); //TODO: why is this called verticies??? It works, but ...
        v_output.mMesh->mMaterialID = mesh->mMaterialIndex;
        v_output.mMesh->mDrawCount = std::size_t(mesh->mNumFaces) * 3;

        std::cerr << "matIndex = " << mesh->mMaterialIndex << "\n";
        std::cerr << "mDrawCount = " << mesh->mNumVertices << "\n";
        std::cerr << "faces =  = " << mesh->mNumFaces << " * 3\n";

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            
            lp::res::ModelLoader::LoadingModel::Vertex vertex;
            //create function to convert assimp -> glm
            vertex.mPosition = int_convert_Assimp(mesh->mVertices[i]);
            if (mesh->HasNormals())
            {
                vertex.mNormal = int_convert_Assimp(mesh->mNormals[i]);
            }

            if (mesh->HasTextureCoords(0))
            {
                const glm::vec3 intermediary = int_convert_Assimp(mesh->mTextureCoords[0][i]);
                vertex.mTextureCoords = {intermediary.x, intermediary.y};
            }

            if (mesh->HasTangentsAndBitangents())
            {
                vertex.mTangent = int_convert_Assimp(mesh->mTangents[i]);
            }

            v_output.mVerticies.push_back(vertex);
        }

        for (unsigned int j = 0; j < mesh->mNumFaces; j++)
            {
                aiFace face = mesh->mFaces[j];
                if (face.mNumIndices != 3) 
                {
                    std::cerr << "The number of verticies in a face is not 3\n"; 
                    continue;
                }
                // retrieve all indices of the face and store them in the indices vector
                v_output.mIndicies.push_back(face.mIndices[0]);
                v_output.mIndicies.push_back(face.mIndices[1]);
                v_output.mIndicies.push_back(face.mIndices[2]);

            }
    }
}