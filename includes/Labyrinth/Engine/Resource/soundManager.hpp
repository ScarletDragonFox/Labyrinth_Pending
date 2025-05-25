#ifndef LABYRINTH_PENDING_ENGINE_RESOURCE_SOUNDMANAGER_HPP
#define LABYRINTH_PENDING_ENGINE_RESOURCE_SOUNDMANAGER_HPP

#include <soloud.h>

#include "Labyrinth/Engine/ECS/baseSystem.hpp"

#include <string>
#include <string_view>
#include <unordered_map>

namespace lp::res
{
    /// @brief semi-internal class used by SoundManager.
    ///
    /// This recives all Entities with a ComponentSoundSource + ComponentPhysics
    class SoundManager_AuxillaryPhysics: public lp::ecs::System
    {
        public:

        /// @brief get the dirty flag
        /// @return the dirty flag
        bool getDirty() const { return this->mDirty; }

        /// @brief set the dirty flag
        /// @param cv_Dirty the dirty flag
        void setDirty(const bool cv_Dirty) { this->mDirty = cv_Dirty; }

        /// @brief get the set of Entities
        /// @return const reference to set
        std::set<lp::ecs::Entity> const & getSet() const{ return this->mEntities; }
    };

    /// @brief semi-internal class used by SoundManager.
    ///
    /// This recives all Entities with a ComponentSoundSource + ComponentPosition
    class SoundManager_AuxillaryPosition: public lp::ecs::System
    {
        public:
        
        /// @brief get the dirty flag
        /// @return the dirty flag
        bool getDirty() const { return this->mDirty; }

        /// @brief set the dirty flag
        /// @param cv_Dirty the dirty flag
        void setDirty(const bool cv_Dirty) { this->mDirty = cv_Dirty; }

        /// @brief get the set of Entities
        /// @return const reference to set
        std::set<lp::ecs::Entity> const & getSet() const{ return this->mEntities; }
    };


    class SoundManager: lp::ecs::System
    {
        public:
        /// @brief removes old/stopped sound source components
        void update();

        /// @brief check if an id (string) is loaded
        /// @param soundNameID string-id of the sound
        /// @return true if loaded, false otherwise
        /// @note mostly used internally
        bool isLoaded(const std::string_view soundNameID);
        
        /// @brief play a 3d sound attaching it to an entity
        /// @param cv_entity id of the entity
        /// @param soundNameID string-id of the sound
        /// @warning Fails silently!
        void playSoundEntity(const lp::ecs::Entity cv_entity, const std::string_view soundNameID);

        /// @brief stop all sounds
        void stopAll();

        /// @brief resume all stopped sounds
        /// @todo TODO: untested!
        void resumeAll();

        private:
        /// @brief Continer struct for AudioSources & data
        struct AudioData
        {
            /// @brief pointer to the audio source
            SoLoud::AudioSource* source = nullptr;
        };
        
        /// @brief map of string id/name-> sound/audio
        std::unordered_map<std::string, AudioData> mAudios;

        /// @brief if true all sounds & processing is stopped. Used internally, do not set!
        bool mGlobalStop = false;
    };
}

#endif //LABYRINTH_PENDING_ENGINE_RESOURCE_SOUNDMANAGER_HPP