#ifndef LABYRINTH_PENDING_ENGINE_GRAPHICS_GRAPHICSPREPARESYSTEM_HPP
#define LABYRINTH_PENDING_ENGINE_GRAPHICS_GRAPHICSPREPARESYSTEM_HPP

#include <glad/gl.h>

#include <Labyrinth/Engine/ECS/baseSystem.hpp>
#include <Labyrinth/Engine/ComponentLight.hpp>

#include <unordered_map>

#include <Labyrinth/Engine/Graphics/processedScene.hpp>

namespace lp::gl
{
    /// @brief forward declaration of Bullet3Debug
    class Bullet3Debug;

    /// @brief semi-internal class used by GraphicsPrepareSystem.
    ///
    /// This recives all Entities with a ComponentModel + ComponentPhysics
    class GPS_Model_w_Physics: public lp::ecs::System
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

    /// @brief semi-internal class used by GraphicsPrepareSystem.
    ///
    /// This recives all Entities with a ComponentModel + ComponentPosition
    class GPS_Model_w_Position: public lp::ecs::System
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


    class GraphicsPrepareSystem
    {
        public:
        /// @brief default constructor. Registeres the GPS_Model_w_Physics & GPS_Model_w_Position Systems.
        GraphicsPrepareSystem();

        /// @brief shoves all data into the ProcessedScene
        /// @param output output. clears all data that was there previously
        void process(ProcessedScene& output, const Bullet3Debug& bulletDebug);

        private:

        void addModel(ProcessedScene& output, const lp::res::ModelID_t id, std::shared_ptr<lp::res::LoadedModel>& ptr, const glm::mat4& matrix);

        std::shared_ptr<GPS_Model_w_Physics> mSystemPhysics;
        std::shared_ptr<GPS_Model_w_Position> mSystemPositions;
    };

        //TODOs:
        // - constexpr/consteval ECS
        // - make component ids constant & just put them all + their forward declarations in a file.
        //     constexpr const_Signature_ModelComponent = 1; etc...
        // - create a non-physics position component:
        //   + normally, a model Matrix will be obtained straight from bullet
        //   + no exceptions for static/dynamic/kinematic
        //   + but we NEED a way to still have that info outside of Bullet
        //   + will be rarely used but make a ComponentPosition
        //   + Create a system with a ComponentPosition & CompoentPhysics as its Signature
        //   + so it can crash because these should NEVER be together!!!!!
        //   + the alternative being having them both be the same, but as a variant
        // - do the physics component:
        //   + the whole notification thing
        //   + try to fix that horribly named function while you're at it
        //   + the Resource Manager owns the btCollisionShape's
        //   + we don't need to be able to access them, so just use an id?
        //   + EVERY physics component is a unique object in bullets world
        //   + think about switching levels (unloading/loading from bullets world)
        //   + a btRigidBody doesn't NEED to be added to any world
        //   + btRigidBody -> World - a btRigidBody cannot belong to multiple worlds at the same time!!
        // - levels:
        //   + 'holding cell' / queue of Components?
        //   + could try a copy of CoreECS, but would break some things at replacement (like player)
        //   + a level BEFORE loading is the entire structure of entities + components
        //   + AFTERWARDS it is just a list of Entities to save on mem (maybe also a list of used [unloaded] assets)
        //   + try loading in a separate thread
        //   + freeze game as old -> unloaded & new -> put in place
        //   + THIS WILL NOT UNLOAD ANY ASSETS! removes hem from Entities but not the ResourceManager
        //   + When loading new things is completed do a garbage collection run & unload unused assets
        //   + so we can reuse assets between levels


} // namespace lp::gl


#endif //LABYRINTH_PENDING_ENGINE_GRAPHICS_GRAPHICSPREPARESYSTEM_HPP