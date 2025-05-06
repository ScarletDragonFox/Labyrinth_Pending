#include "BulletShape.hpp"

#include <array>
#include <iostream>

LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH();
#include <bullet/BulletCollision/CollisionShapes/btCompoundShape.h>
#include <bullet/btBulletDynamicsCommon.h>
LP_PRAGMA_DISABLE_ALL_WARNINGS_POP();

namespace lpt
{
    constexpr const char* getName(const BulletShapeType type)
    {
        switch(type)
            {
                case BulletShapeType::CompoundShape: return "btCompoundShape";
                case BulletShapeType::CapsuleShapeY: return "btCapsuleShape";
                case BulletShapeType::CapsuleShapeX: return "btCapsuleShapeX";
                case BulletShapeType::CapsuleShapeZ: return "btCapsuleShapeZ";
                case BulletShapeType::ConeShapeY: return "btConeShape";
                case BulletShapeType::ConeShapeX: return "btConeShapeX";
                case BulletShapeType::ConeShapeZ: return "btConeShapeZ";
                case BulletShapeType::CylinderShapeY: return "btCylinderShape";
                case BulletShapeType::CylinderShapeX: return "btCylinderShapeX";
                case BulletShapeType::CylinderShapeZ: return "btCylinderShapeZ";
                case BulletShapeType::BoxShape: return "btBoxShape";
                case BulletShapeType::SphereShape: return "btSphereShape";
                case BulletShapeType::ConvexHullShape: return "btConvexHullShape";
                //STATIC:
                case BulletShapeType::StaticPlaneShape: //infinite plane
                    return "btStaticPlaneShape";  
                case BulletShapeType::TriangleMeshShape: //mesh of triangles all with 1 material
                    return "btBvhTriangleMeshShape";
                case BulletShapeType::Size:
                [[fallthrough]];
                default:
                    return "InvalidShape";
            }
    }


    BulletShape::~BulletShape()
    {
        erase();
    }
    
    void BulletShape::erase()
    {
        if(mData)
        {
            switch(mType)
            {
                case BulletShapeType::CompoundShape:
                    delete (btCompoundShape*)mData;
                    break;
                case BulletShapeType::CapsuleShapeY:
                    delete (btCapsuleShape*)mData;
                    break;
                case BulletShapeType::CapsuleShapeX:
                    delete (btCapsuleShapeX*)mData;
                    break;
                case BulletShapeType::CapsuleShapeZ:
                    delete (btCapsuleShapeZ*)mData;
                    break;
                case BulletShapeType::ConeShapeY:
                    delete (btConeShape*)mData;
                    break;
                case BulletShapeType::ConeShapeX:
                    delete (btConeShapeX*)mData;
                    break;
                case BulletShapeType::ConeShapeZ:
                    delete (btConeShapeZ*)mData;
                    break;
                case BulletShapeType::CylinderShapeY:
                    delete (btCylinderShape*)mData;
                    break;
                case BulletShapeType::CylinderShapeX:
                    delete (btCylinderShapeX*)mData;
                    break;
                case BulletShapeType::CylinderShapeZ:
                    delete (btCylinderShapeZ*)mData;
                    break;
                case BulletShapeType::BoxShape:
                    delete (btBoxShape*)mData;
                    break;
                case BulletShapeType::SphereShape:
                    delete (btSphereShape*)mData;
                    break;
                case BulletShapeType::ConvexHullShape:
                    delete (btConvexHullShape*)mData;
                    break;
                //STATIC:
                case BulletShapeType::StaticPlaneShape: //infinite plane
                    delete (btStaticPlaneShape*)mData;
                    break;
                case BulletShapeType::TriangleMeshShape: //mesh of triangles all with 1 material
                    delete (btBvhTriangleMeshShape*)mData;
                    break;
                case BulletShapeType::Size:
                [[fallthrough]];
                default:
                    //delete mData; //memory leak
                    std::cerr << "Memory Leak @ "<< __FILE__ << " @ " << __LINE__ << "\n";
                    break;
            }
            mData = nullptr;
        }
        mType = BulletShapeType::Size;
    }
    
}