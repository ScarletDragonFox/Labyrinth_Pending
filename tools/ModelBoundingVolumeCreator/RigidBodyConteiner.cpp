#include "RigidBodyConteiner.hpp"

#include <imgui.h>
#include <glm/glm.hpp>

namespace lpt
{
    RigidBodyContainerSingular::RigidBodyContainerSingular(std::shared_ptr<btDynamicsWorld> vWorld, lpt::BulletShape& vShape)
    {
        mWorld = vWorld;
        mBodyMotionState = std::make_shared<btDefaultMotionState>();
        
        mShape = std::move(vShape);

        btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, mBodyMotionState.get(), (btCollisionShape*)mShape.getRaw(), btVector3(0,0,0));
        mBody =  std::make_shared<btRigidBody>(groundRigidBodyCI);
        mBody->setFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
        vWorld->addRigidBody(mBody.get());
    }

    const char* RigidBodyContainerSingular::getName() const
    {
        if((btCollisionShape*)mShape.getRaw() == nullptr) return "ERROR!";
        return ((btCollisionShape*)mShape.getRaw())->getName();
    }

    void RigidBodyContainerSingular::ui()
    {
        ImGui::Text("Name: \"%s\"", this->getName());
            bool updated = false;
            btTransform tranz;
            mBodyMotionState->getWorldTransform(tranz);
            const btVector3 btPos = tranz.getOrigin();
            glm::vec3 pos = glm::vec3(btPos.getX(), btPos.getY(), btPos.getZ());
            if(ImGui::SliderFloat3("position", &pos.x, -100.0, 100.0, "%.3f", ImGuiSliderFlags_NoRoundToFormat))
            {
                tranz.setOrigin(btVector3(pos.x, pos.y, pos.z));
                updated = true;
            }
            
            btCollisionShape* shapePtr = ((btCollisionShape*)mShape.getRaw());
            if(shapePtr != nullptr){
                const btVector3 shIN = shapePtr->getLocalScaling();
                glm::vec3 scale = {shIN.getX(), shIN.getY(), shIN.getZ()};
                if(ImGui::SliderFloat3("scale", &scale.x, 0.01, 10.0)){
                    shapePtr->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
                }
            }

            const btQuaternion btRot = tranz.getRotation();
            glm::vec3 rot = {};
            btRot.getEulerZYX(rot.z, rot.y, rot.x);
            rot.x = glm::degrees(rot.x);
            rot.y = glm::degrees(rot.y);
            rot.z = glm::degrees(rot.z);
            if(ImGui::SliderFloat3("rotation", &rot.x, -180, 180, "%.2f", ImGuiSliderFlags_NoRoundToFormat))
            {
                tranz.setRotation(btQuaternion(glm::radians(rot.z), glm::radians(rot.y), glm::radians(rot.x)));
                updated = true;
            }
            ImGui::SameLine();
            if(ImGui::Button("Reset"))
            {
                tranz.setRotation(btQuaternion(0.0, 0.0, 0.0));
                updated = true;
            }
            if(updated)
            {
                mBodyMotionState->setWorldTransform(tranz);
                mBody->setMotionState(mBodyMotionState.get());
            }
    }

    void RigidBodyContainerSingular::changeCollision(lpt::BulletShape& vShape)
    {
        mBody->setCollisionShape(static_cast<btCollisionShape*>(vShape.getRaw()));
        mBody->activate();
        mShape = std::move(vShape);
    }
    RigidBodyContainerSingular::~RigidBodyContainerSingular()
    {
        mWorld->removeRigidBody(mBody.get());
    }


    RigidBosyContainerCreationClassThing::RigidBosyContainerCreationClassThing(std::shared_ptr<btDynamicsWorld> vWorld)
    {
        mWorld = vWorld;
    }

    void RigidBosyContainerCreationClassThing::drawUI()
    {
        if(ImGui::Button("Create new"))
        {
            mModalLastSelected = lpt::BulletShapeType::SphereShape;
            mModalFloatVariable[0] = 1.0f;
            mModalFloatVariable[1] = 1.0f;
            mModalFloatVariable[2] = 1.0f;
            ImGui::OpenPopup("Create New Shape - Modal");
        }
        bool doDelete = false;
        std::size_t iref_deleta = 0;
        std::size_t iref = 0;
        for(auto& i : mObjects)
        {
            std::string name = i->getName();
            name += "###" + std::to_string(iref);
            if(ImGui::TreeNode(name.c_str()))
            {
                if(ImGui::Button("Kill"))
                {
                    iref_deleta = iref; 
                    doDelete = true;
                }
                i->ui();
                ImGui::TreePop();
            }
            ++iref;
        }
        if(doDelete)
        {
            if(iref_deleta < mObjects.size())
            {
                mObjects.erase(mObjects.begin() + iref_deleta);
            }
        }
        drawModalPopup();
    }
    
    void RigidBosyContainerCreationClassThing::drawModalPopup()
    {
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if(ImGui::BeginPopupModal("Create New Shape - Modal", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            const char* value_preview = lpt::getName(mModalLastSelected);
            if(ImGui::BeginCombo("btCollisionShape", value_preview))
            {
                for(std::size_t i = 0; i < static_cast<std::size_t>(lpt::BulletShapeType::Size); ++i)
                {
                    const lpt::BulletShapeType type = static_cast<lpt::BulletShapeType>(i);
                    const bool is_selected = mModalLastSelected == type;
                    if(ImGui::Selectable(lpt::getName(type), is_selected))
                    {
                        mModalLastSelected = type;
                    }
                }
                ImGui::EndCombo();
            }

            switch(mModalLastSelected)
            {
                case lpt::BulletShapeType::CapsuleShapeY:
                [[fallthrough]];
                case lpt::BulletShapeType::CapsuleShapeX:
                [[fallthrough]];
                case lpt::BulletShapeType::CapsuleShapeZ:
                [[fallthrough]];
                case lpt::BulletShapeType::ConeShapeY:
                [[fallthrough]];
                case lpt::BulletShapeType::ConeShapeX:
                [[fallthrough]];
                case lpt::BulletShapeType::ConeShapeZ:
                {
                    ImGui::SliderFloat("radius", &mModalFloatVariable[0], 0.001f, 100.0f);
                    ImGui::SliderFloat("height", &mModalFloatVariable[1], 0.001f, 100.0f);
                    break;
                }

                case lpt::BulletShapeType::CylinderShapeY:
                [[fallthrough]];
                case lpt::BulletShapeType::CylinderShapeX:
                [[fallthrough]];
                case lpt::BulletShapeType::CylinderShapeZ:
                [[fallthrough]];
                case lpt::BulletShapeType::BoxShape:
                {
                    ImGui::SliderFloat3("boxHalfExtents", &mModalFloatVariable[0], 0.001f, 100.0f);
                    break;
                }
                case lpt::BulletShapeType::SphereShape:
                {
                    ImGui::SliderFloat("radius", &mModalFloatVariable[0], 0.001f, 100.0f);
                    break;
                }
                // {
                //     ImGui::Text("ConvexHullShape settings here");
                //     ImGui::Text("ConvexHullShape model load here?");
                //     ImGui::Text("maybe even model optimisation??");
                //     break;
                // }
                case lpt::BulletShapeType::ConvexHullShape:
                [[fallthrough]];
                case lpt::BulletShapeType::CompoundShape:
                [[fallthrough]];
                case lpt::BulletShapeType::TriangleMeshShape:
                [[fallthrough]];
                case lpt::BulletShapeType::StaticPlaneShape:
                [[fallthrough]];
                case lpt::BulletShapeType::Size:
                [[fallthrough]];
                default:
                    ImGui::Text("Unsupported value chosen. Quo vadis?");
                    break;
            }

            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                lpt::BulletShape shape;
                switch(mModalLastSelected)
                {
                    case lpt::BulletShapeType::CapsuleShapeY:
                        shape = shape.create<btCapsuleShape>(mModalFloatVariable[0], mModalFloatVariable[1]);
                        break;
                    case lpt::BulletShapeType::CapsuleShapeX:
                        shape = shape.create<btCapsuleShapeX>(mModalFloatVariable[0], mModalFloatVariable[1]);
                        break;
                    case lpt::BulletShapeType::CapsuleShapeZ:
                        shape = shape.create<btCapsuleShapeZ>(mModalFloatVariable[0], mModalFloatVariable[1]);
                        break;
                    case lpt::BulletShapeType::ConeShapeY:
                        shape = shape.create<btConeShape>(mModalFloatVariable[0], mModalFloatVariable[1]);
                        break;
                    case lpt::BulletShapeType::ConeShapeX:
                        shape = shape.create<btConeShapeX>(mModalFloatVariable[0], mModalFloatVariable[1]);
                        break;
                    case lpt::BulletShapeType::ConeShapeZ:
                        shape = shape.create<btConeShapeZ>(mModalFloatVariable[0], mModalFloatVariable[1]);
                        break;

                    case lpt::BulletShapeType::CylinderShapeY:
                        shape = shape.create<btCylinderShape>(btVector3(mModalFloatVariable[0], mModalFloatVariable[1], mModalFloatVariable[2]));
                        break;
                    case lpt::BulletShapeType::CylinderShapeX:
                        shape = shape.create<btCylinderShapeX>(btVector3(mModalFloatVariable[0], mModalFloatVariable[1], mModalFloatVariable[2]));
                        break;
                    case lpt::BulletShapeType::CylinderShapeZ:
                        shape = shape.create<btCylinderShapeZ>(btVector3(mModalFloatVariable[0], mModalFloatVariable[1], mModalFloatVariable[2]));
                        break;
                    case lpt::BulletShapeType::BoxShape:
                        shape = shape.create<btBoxShape>(btVector3(mModalFloatVariable[0], mModalFloatVariable[1], mModalFloatVariable[2]));
                        break;
                    case lpt::BulletShapeType::SphereShape:
                        shape = shape.create<btSphereShape>(mModalFloatVariable[0]);
                        break;

                    case lpt::BulletShapeType::ConvexHullShape:
                    [[fallthrough]];
                    case lpt::BulletShapeType::CompoundShape:
                    [[fallthrough]];
                    case lpt::BulletShapeType::TriangleMeshShape:
                    [[fallthrough]];
                    case lpt::BulletShapeType::StaticPlaneShape:
                    [[fallthrough]];
                    case lpt::BulletShapeType::Size:
                    [[fallthrough]];
                    default:
                        goto GOTO_INSTR_SKIP;
                        break;
                }
                ImGui::CloseCurrentPopup();
                {
                    mObjects.push_back(std::make_unique<RigidBodyContainerSingular>(mWorld, shape));
                }
                GOTO_INSTR_SKIP:;
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) 
            { 
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
}

