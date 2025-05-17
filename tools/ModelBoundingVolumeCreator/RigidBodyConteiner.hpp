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
        RigidBodyContainerSingular(RigidBodyContainerSingular&) = delete;
        RigidBodyContainerSingular(RigidBodyContainerSingular&&) = default;
        const char* getName() const;
        void ui();
        void changeCollision(lpt::BulletShape& vShape);
        ~RigidBodyContainerSingular();

        private:
        lpt::BulletShape mShape;
        std::shared_ptr<btRigidBody> mBody;
        std::shared_ptr<btDefaultMotionState> mBodyMotionState;
        std::shared_ptr<btDynamicsWorld> mWorld;
    };

    class RigidBosyContainerCreationClassThing
    {
        public:
        RigidBosyContainerCreationClassThing(std::shared_ptr<btDynamicsWorld> vWorld);
        void drawUI();
        private:
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