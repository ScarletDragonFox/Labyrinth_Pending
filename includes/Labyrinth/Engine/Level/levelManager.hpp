#ifndef LABYRINTH_PENDING_ENGINE_LEVEL_LEVELMANAGER_HPP
#define LABYRINTH_PENDING_ENGINE_LEVEL_LEVELMANAGER_HPP

#include <Labyrinth/Engine/ECS/coreECS.hpp>
#include <Labyrinth/Engine/Resource/loadedModel.hpp>
#include <Labyrinth/Engine/Physics/physicsWorld.hpp>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>

namespace lp
{
    /// @brief class that loads & unloads levels
    class LevelManager
    {
        /// @brief private storage of loaded data for unloading
        struct Level
        {
            std::vector<lp::ecs::Entity> mLights;
            std::vector<lp::res::ModelID_t> mModels;
            std::vector<lp::ecs::Entity> mModelsEntities;
            std::vector<std::string> mSounds;
            std::vector<lp::ph::ColliderID_t> mCollision;
            /// @brief origin id of all the mCollision-s
            ///
            /// Used to know what ComponentPhysics to kill when unloading a level
            /// @see ComponentPhysics::mOrigin
            unsigned int mCollisionOriginID = 100'001; 
        };
        public:
        /// @brief load a level
        /// @param name name/path of the level.json file
        void loadLevel(std::string_view name);

        /// @brief unloads the specified level
        /// @param name name/path of the level.json file
        void unloadLevel(std::string_view name);

        private:
        /// @brief 
        std::unordered_map<std::string, Level> mLevels;

        unsigned int mNextCollisionOriginID = 100'001;
    };
} // lp

#endif //