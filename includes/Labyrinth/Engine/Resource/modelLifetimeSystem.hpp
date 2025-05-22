#ifndef LABYRINTH_PENDING_ENGINE_RESOURCE_MODELLIFETIMESYSTEM_HPP
#define LABYRINTH_PENDING_ENGINE_RESOURCE_MODELLIFETIMESYSTEM_HPP

#include "Labyrinth/Engine/ECS/componentModel.hpp"
#include "Labyrinth/Engine/ECS/baseSystem.hpp"

namespace lp::res
{
    /// @brief auxillary class for handling loading/unloading of live data from ComponentModel's
    ///
    /// Lives within the ModelLoader
    class SystemModelLifetime: public lp::ecs::System
    {
        public:
        /// @brief updates the ComponentModels' model shared_ptrs when their id matches
        ///
        /// In simpler terms this function updates the .mModel variable of all ComponentModels IF their mID == cv_id
        /// @note called when a model finished loading
        /// @param cv_id id of the loaded model
        /// @param v_ptr the pointer to the newly loaded model
        void modelLoaded(const lp::res::ModelID_t cv_id, std::shared_ptr<lp::res::LoadedModel> v_ptr);

        /// @brief same thing as modelLoaded, except this sets .mModel to nullptr, removing the reference
        /// @param cv_id id of the model-to-be-unloaded
        void modelUnloaded(const lp::res::ModelID_t cv_id);
    };
}

#endif //LABYRINTH_PENDING_ENGINE_RESOURCE_MODELLIFETIMESYSTEM_HPP