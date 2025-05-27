#include "MeshContainer.hpp"

#include "Labyrinth/Helpers/compilerErrors.hpp"
#include <filesystem>
#include <iostream>

LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH()
#include <imgui.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/postprocess.h>

LP_PRAGMA_DISABLE_ALL_WARNINGS_POP()

namespace
{
    void int_processNodeRecursiveFlatten(aiNode* node, const aiScene* scene, std::vector<aiMesh*>& output);
}

namespace lpt
{
    void MeshPhysics::ui()
    {
        ImGui::Text("Verticies Count = %u", (unsigned int)mPositions.size());
        ImGui::Text("Indicies Count = %u", (unsigned int)mIndicies.size());
    }

    MeshPhysics* MeshContainer::getMesh(const MeshDragDropID& cv_id)
    {
        if(!mMeshes.contains(cv_id.mModel)) return nullptr;
        auto& ref = mMeshes[cv_id.mModel];
        if(!ref.contains(cv_id.mID)) return nullptr;
        return &(ref[cv_id.mID]);
    }

    void MeshContainer::ui(bool* opened)
    {
        if(ImGui::Begin("MeshContainer Window", opened))
        {
            ImGui::InputTextWithHint("path", "path to model/meshes", mIMGUI_path, 256);
            ImGui::SameLine();
            if(ImGui::Button("Load"))
            {
                this->loadMeshes(mIMGUI_path);
            }
            for(auto&iMdl: this->mMeshes)
            {
                //meshes need to be a 'selectable' so we can drag & drop them as references:
                // - make it so we can add/remove a category
                // - make meshes moveable betwen categories
                // - whil still keeping
                if(ImGui::TreeNode(iMdl.first.c_str())) 
                {
                    if(ImGui::BeginDragDropTarget())
                    {
                        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MESH_TYPE");
                        if(payload != nullptr && payload->DataSize == sizeof(MeshDragDropID))
                        {
                            MeshDragDropID* ptr = (MeshDragDropID*)payload->Data;
                            if(ptr->mModel != iMdl.first)
                            {
                                iMdl.second[ptr->mID] = this->mMeshes[ptr->mModel][ptr->mID];
                                this->mMeshes[ptr->mModel].erase(ptr->mID);
                                //delete ptr; // OR HERE ??? //might be a memory leak? ImGui seems to be copying the data? and delete'ing it crashes, so leave it!
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }

                    std::size_t selected = 2'000'000'000;
                    std::size_t iref = 0;
                    for(auto&i: iMdl.second)
                    {
                        std::string name = i.second.mName + std::to_string(i.first);
                        if(ImGui::TreeNode(name.c_str()))
                        {
                            if(ImGui::Button("Unload"))
                            {
                                selected = iref;
                            }
                            ImGui::Text("id = %u", i.first);

                            i.second.ui();
                            ImGui::TreePop();
                        }
                        if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                            {
                                MeshDragDropID* dataa = new MeshDragDropID();
                                dataa->mModel.assign(iMdl.first);
                                dataa->mID = i.first;
                                ImGui::SetDragDropPayload("MESH_TYPE", dataa, sizeof(MeshDragDropID));
                                ImGui::EndDragDropSource();
                            }
                        ++iref;
                    }
                    if(selected != 2'000'000'000)
                    {
                        auto iter = iMdl.second.begin();
                        std::advance(iter, selected);
                        iMdl.second.erase(iter->first);
                    }
                    ImGui::TreePop();
                }
            }
        }
        ImGui::End();
    }

    void MeshContainer::loadMeshes(const std::string_view cv_path)
    {
        const std::filesystem::path c_path(cv_path);
        if(!std::filesystem::is_regular_file(c_path))
        {
            std::cerr << "ERROR::ASSIMP:: " << c_path << " is not a file!" << std::endl;
                return;
        }
        const std::string directory = c_path.parent_path().string();

        constexpr unsigned int Flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_RemoveComponent;
        //aiProcess_PreTransformVertices; //fixes the no transform for meshes problem!!!
        Assimp::Importer importer;
        // ignore / do not import all components bar aiComponent_MESHES, as we don't use them
        constexpr int ignoredComponents = aiComponent_NORMALS | aiComponent_TANGENTS_AND_BITANGENTS | aiComponent_COLORS 
            | aiComponent_TEXCOORDS | aiComponent_BONEWEIGHTS | aiComponent_ANIMATIONS | aiComponent_TEXTURES 
            | aiComponent_LIGHTS | aiComponent_CAMERAS | aiComponent_MATERIALS;
        importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, ignoredComponents);
        
        const aiScene* scene = importer.ReadFile(cv_path.data(), Flags);

        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }

        std::vector <aiMesh*> assimp_meshes;
        assimp_meshes.reserve(scene->mNumMeshes);
        int_processNodeRecursiveFlatten(scene->mRootNode, scene, assimp_meshes);

        std::vector<MeshPhysics> meshes;
        meshes.reserve(scene->mNumMeshes);

        std::size_t iref = 0;
        for(aiMesh* am: assimp_meshes)
        {
            MeshPhysics mp;
            mp.mPositions.reserve(am->mNumVertices);
            mp.mIndicies.reserve(std::size_t(am->mNumFaces) * 3);

            for (unsigned int i = 0; i < am->mNumVertices; i++)
            {
                mp.mPositions.push_back(glm::vec3(am->mVertices[i].x, am->mVertices[i].y, am->mVertices[i].z));
            }

            for (unsigned int j = 0; j < am->mNumFaces; j++)
            {
                aiFace face = am->mFaces[j];
                if (face.mNumIndices != 3) 
                {
                    std::cerr << "The number of verticies in a face is not 3\n"; 
                    continue;
                }
                mp.mIndicies.push_back(face.mIndices[0]);
                mp.mIndicies.push_back(face.mIndices[1]);
                mp.mIndicies.push_back(face.mIndices[2]);
            }

            if(am->mName.length == 0)
            {
                mp.mName = "Unnamed_Mesh_";
                mp.mName += std::to_string(iref);
            } else
            {
                mp.mName = am->mName.C_Str();
            }
            meshes.push_back(mp);
            ++iref;
        }

        // process std::vector<MeshPhysics> meshes; if necessary
        std::string modelName = c_path.filename().string() + std::to_string(mNextModelID); ++mNextModelID;
        mMeshes[modelName] = std::unordered_map<MeshID_t, MeshPhysics>();
        for(MeshPhysics& i: meshes)
        {
            (mMeshes[modelName])[mNextID] = i;
            ++mNextID;
        }
    }
}

namespace
{
    void int_processNodeRecursiveFlatten(aiNode* node, const aiScene* scene, std::vector<aiMesh*>& output)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            output.push_back(scene->mMeshes[node->mMeshes[i]]);
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            int_processNodeRecursiveFlatten(node->mChildren[i], scene, output);
        }
    }
}