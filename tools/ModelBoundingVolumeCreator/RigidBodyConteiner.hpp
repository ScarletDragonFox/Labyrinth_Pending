#ifndef CDF_TOOL_RIGIDBODYCONTAINER_HPP
#define CDF_TOOL_RIGIDBODYCONTAINER_HPP

#include <memory>
#include <vector>
#include "Labyrinth/Helpers/compilerErrors.hpp"

LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH();
#include "BulletShape.hpp"
#include <bullet/btBulletDynamicsCommon.h> //put smaller headers here?
LP_PRAGMA_DISABLE_ALL_WARNINGS_POP();

namespace lpt
{
    class RigidBodyContainerSingular
    {
        public:
        RigidBodyContainerSingular(std::shared_ptr<btDynamicsWorld> vWorld, lpt::BulletShape& vShape);
        RigidBodyContainerSingular(std::shared_ptr<btDynamicsWorld> vWorld, btRigidBody* body);
        RigidBodyContainerSingular(RigidBodyContainerSingular&) = delete;
        RigidBodyContainerSingular(RigidBodyContainerSingular&&) = default;
        /// @brief get 'name' of this body
        /// @return c-style string name
        const char* getNameInputted() const;
        /// @brief do the ImGUI ui
        void ui();
        /// @brief change the shape of the object
        /// @param vShape new shape
        void changeCollision(lpt::BulletShape& vShape);
        /// @brief destructor
        ~RigidBodyContainerSingular();

        private:
        /// @brief internal function to get name of CollsisionShape from bullet
        /// @return c-style string name
        const char* getNameBullet() const;

        lpt::BulletShape mShape;
        std::shared_ptr<btRigidBody> mBody;
        std::shared_ptr<btDefaultMotionState> mBodyMotionState;
        std::shared_ptr<btDynamicsWorld> mWorld;
        char mNameBuffer[51] = {};
    };

    class RigidBosyContainerCreationClassThing
    {
        public:
        RigidBosyContainerCreationClassThing(std::shared_ptr<btDynamicsWorld> vWorld);

        /// @brief kills all 'child' shapes 
        void killAllChildren();

        /// @brief add a new child to the Container
        /// @param body pointer to body
        /// @warning This does not add the body to the physics world!
        void addNewChild(btRigidBody* body);

        /// @brief do the ImGUI ui
        void drawUI();
        private:
        /// @brief internal func to draw a modal popup with ImGUI
        void drawModalPopup();
        std::vector<std::unique_ptr<RigidBodyContainerSingular>> mObjects;
        lpt::BulletShapeType mModalLastSelected = lpt::BulletShapeType::SphereShape;
        std::shared_ptr<btDynamicsWorld> mWorld;
        // 0 - radius ; boxHalfExtents.X ; radius
        // 1 -        ; boxHalfExtents.Y ; height
        // 2 -        ; boxHalfExtents.Z ;
        float mModalFloatVariable[3] = {};
    };
}

#endif //CDF_TOOL_RIGIDBODYCONTAINER_HPP