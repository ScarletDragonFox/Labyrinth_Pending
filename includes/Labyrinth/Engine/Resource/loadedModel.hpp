#ifndef LABYRINTH_PENDING_ENGINE_RESOURCE_LOADEDMODEL_HPP
#define LABYRINTH_PENDING_ENGINE_RESOURCE_LOADEDMODEL_HPP

#include <glad/gl.h>
#include "Labyrinth/Engine/Graphics/textures.hpp"
#include <filesystem>

namespace lp::res
{
    /// @brief id type used for models
    using ModelID_t = std::uint32_t;

    /// @brief invalid ModelID_t
    constexpr ModelID_t const_id_model_invalid = 0;

    /// @brief represents a loaded model
    struct LoadedModel
    {
        /// @brief typedef of material id
        using MaterialID_t = unsigned int;

        /// @brief Simple Mesh.
        ///
        /// Has the minimum ammount of data possible to render the mesh
        struct Mesh
        {
            /// @brief the Vertex Buffer Object storing all Verticies OpenGL-side
            GLuint mVBO = 0;
            /// @brief the Element Buffer Object storing all Indicies OpenGL-side
            GLuint mEBO = 0;
            /// @brief how many Elements the EBO has
            GLuint mDrawCount = 0;
            /// @brief is of used material.
            ///
            /// Used as index into mMaterials
            MaterialID_t mMaterialID = 0;
        };
        /// @brief Simple Material
        ///
        /// Represents a material (a series of textures to load)
        struct Material
        {
            /// @brief a BaseColor/Albedo Texture of the material
            lp::gl::Texture mColor;

            /// @brief a specular reflaection color of this material
            lp::gl::Texture mSpecular;

            /// @brief shininess of the material. 16.0 by default
            float mShininess = 16.0f;
        };

        std::vector<LoadedModel::Mesh> mMeshes;
        std::vector<LoadedModel::Material> mMaterials;
        std::filesystem::path mFile = {};
    };
}

#endif //LABYRINTH_PENDING_ENGINE_RESOURCE_LOADEDMODEL_HPP