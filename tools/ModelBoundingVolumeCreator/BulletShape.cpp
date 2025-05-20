#include "BulletShape.hpp"

#include <array>
#include <iostream>
#include <cstring> //strlen

LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH();
#include <bullet/BulletCollision/CollisionShapes/btCompoundShape.h>
#include <bullet/btBulletDynamicsCommon.h>
LP_PRAGMA_DISABLE_ALL_WARNINGS_POP();

namespace lpt
{

    BulletShape::~BulletShape()
    {
        erase();
    }
    
    //THIS IS UNTESTED!!!
    void BulletShape::assign(btCollisionShape* collShape)
    {
        this->erase();
        const int bulletShapeType = collShape->getShapeType();

        mData = (void*)collShape;

        switch(bulletShapeType)
        {
            case COMPOUND_SHAPE_PROXYTYPE:
                this->mType = BulletShapeType::CompoundShape;
                break;
 
            case CAPSULE_SHAPE_PROXYTYPE: //XYZ
                {
                    const char* name = collShape->getName();
                    const std::size_t len = std::strlen(name);
                    const char last_char =  name[len - 1];
                    if(last_char == 'X')
                    {
                        this->mType = BulletShapeType::CapsuleShapeX;
                    } else if (last_char == 'Z')
                    {
                        this->mType = BulletShapeType::CapsuleShapeZ;
                    } else
                    {
                        this->mType = BulletShapeType::CapsuleShapeY;
                    }
                }
                break;
            case CONE_SHAPE_PROXYTYPE: //XYZ
                {
                    const char* name = collShape->getName();
                    const std::size_t len = std::strlen(name);
                    const char last_char =  name[len - 1];
                    if(last_char == 'X')
                    {
                        this->mType = BulletShapeType::ConeShapeX;
                    } else if (last_char == 'Z')
                    {
                        this->mType = BulletShapeType::ConeShapeZ;
                    } else
                    {
                        this->mType = BulletShapeType::ConeShapeY;
                    }
                }
                break;
            case CYLINDER_SHAPE_PROXYTYPE: //XYZ
                {
                    const char* name = collShape->getName();
                    const std::size_t len = std::strlen(name);
                    const char last_char =  name[len - 1];
                    if(last_char == 'X')
                    {
                        this->mType = BulletShapeType::CylinderShapeX;
                    } else if (last_char == 'Z')
                    {
                        this->mType = BulletShapeType::CylinderShapeZ;
                    } else
                    {
                        this->mType = BulletShapeType::CylinderShapeY;
                    }
                }
                break;

            case BOX_SHAPE_PROXYTYPE:
                this->mType = BulletShapeType::BoxShape;
                break;
            case SPHERE_SHAPE_PROXYTYPE:
                this->mType = BulletShapeType::SphereShape;
                break;
            case CONVEX_HULL_SHAPE_PROXYTYPE:
                this->mType = BulletShapeType::ConvexHullShape;
                break;
            case STATIC_PLANE_PROXYTYPE:
                this->mType = BulletShapeType::StaticPlaneShape;
                break;
            case TRIANGLE_SHAPE_PROXYTYPE:
                this->mType = BulletShapeType::TriangleMeshShape;
                break;
            default:
                this->mType = BulletShapeType::Size;
                break;
        }
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