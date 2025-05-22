#ifndef LABYRINTH_PENDING_ENGINE_ECS_COMPONENTMODEL_HPP
#define LABYRINTH_PENDING_ENGINE_ECS_COMPONENTMODEL_HPP
#include <cstdint>
#include <glm/vec3.hpp>
#include <memory>

#include "Labyrinth/Engine/Resource/loadedModel.hpp"

namespace lp::ecs
{
    /// @brief Component that stores a model.
    struct ComponentModel
    {
        /// @brief ptr to model. Is nullptr if the model is not loaded.
        ///
        /// Copying this around will probably break things, so avoid that.
        std::shared_ptr<lp::res::LoadedModel> mModel;

        /// @brief id of the model that this represents.
        ///
        /// Used by lp::res::SystemModelLifetime for updating mModel.
        lp::res::ModelID_t mID = lp::res::const_id_model_invalid;
    };

}

#endif //LABYRINTH_PENDING_ENGINE_ECS_COMPONENTMODEL_HPP