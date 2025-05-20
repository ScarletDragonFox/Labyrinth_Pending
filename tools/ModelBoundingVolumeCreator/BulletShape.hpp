#ifndef CDF_TOOL_BULLETSHAPE_HPP
#define CDF_TOOL_BULLETSHAPE_HPP
#include <type_traits>
#include <utility> //std::forward

#include "Labyrinth/Helpers/compilerErrors.hpp"


LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH();

//list taken from deps\bullet3\include\bullet\btBulletCollisionCommon.h

#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "BulletCollision/CollisionShapes/btCapsuleShape.h"
#include "BulletCollision/CollisionShapes/btCylinderShape.h"
#include "BulletCollision/CollisionShapes/btConeShape.h"
#include "BulletCollision/CollisionShapes/btStaticPlaneShape.h"
#include "BulletCollision/CollisionShapes/btConvexHullShape.h"
#include "BulletCollision/CollisionShapes/btTriangleMesh.h"
#include "BulletCollision/CollisionShapes/btConvexTriangleMeshShape.h"
#include "BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h"
#include "BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.h"
#include "BulletCollision/CollisionShapes/btTriangleMeshShape.h"
#include "BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h"
#include "BulletCollision/CollisionShapes/btCompoundShape.h"
#include "BulletCollision/CollisionShapes/btTetrahedronShape.h"
#include "BulletCollision/CollisionShapes/btEmptyShape.h"
#include "BulletCollision/CollisionShapes/btMultiSphereShape.h"
#include "BulletCollision/CollisionShapes/btUniformScalingShape.h"
LP_PRAGMA_DISABLE_ALL_WARNINGS_POP();

namespace lpt
{
    template <typename T>
    concept ConceptbtCollisionShapeDerived = std::is_base_of<btCollisionShape, T>::value;

    /// @brief enum of all btCollisionShape-derived classes that we support for @ref BulletShape
    ///
    /// @note look at BroadphaseNativeTypes from BulletCollision\BroadphaseCollision\btBroadphaseProxy.h
    enum class BulletShapeType
    {
        CompoundShape,
        CapsuleShapeY,
        CapsuleShapeX,
        CapsuleShapeZ,
        ConeShapeY,
        ConeShapeX,
        ConeShapeZ,
        CylinderShapeY,
        CylinderShapeX,
        CylinderShapeZ,
        BoxShape,
        SphereShape,
        ConvexHullShape,

        //STATIC:
        StaticPlaneShape, //infinite plane
        TriangleMeshShape, //mesh of triangles all with 1 material
        Size //should always be last. Also used as error value
    };

    /// @brief get name of enum
    /// @param type BulletShapeType enum
    /// @return const char* to C-style string name
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
        return "InvalidShape2";
    }

    /// @brief A horrific abomination of a function.
    ///
    /// Used for converting a btCollisionShape-derived class -> enum
    /// @tparam T btCollisionShape-derived class type
    /// @return BulletShapeType::Size if not found
    template<ConceptbtCollisionShapeDerived T>
    consteval BulletShapeType getEnum()
    {
        if constexpr(std::is_same<T, btCompoundShape>::value)
        {
            return BulletShapeType::CompoundShape;
        } else if constexpr(std::is_same<T, btCapsuleShape>::value)
        {
            return BulletShapeType::CapsuleShapeY;
        } else if constexpr(std::is_same<T, btCapsuleShapeX>::value)
        {
            return BulletShapeType::CapsuleShapeX;
        } else if constexpr(std::is_same<T, btCapsuleShapeZ>::value)
        {
            return BulletShapeType::CapsuleShapeZ;
        } else if constexpr(std::is_same<T, btConeShape>::value)
        {
            return BulletShapeType::ConeShapeY;
        } else if constexpr(std::is_same<T, btConeShapeX>::value)
        {
            return BulletShapeType::ConeShapeX;
        } else if constexpr(std::is_same<T, btConeShapeZ>::value)
        {
            return BulletShapeType::ConeShapeZ;
        } else if constexpr(std::is_same<T, btCylinderShape>::value)
        {
            return BulletShapeType::CylinderShapeY;
        } else if constexpr(std::is_same<T, btCylinderShapeX>::value)
        {
            return BulletShapeType::CylinderShapeX;
        } else if constexpr(std::is_same<T, btCylinderShapeZ>::value)
        {
            return BulletShapeType::CylinderShapeZ;
        } else if constexpr(std::is_same<T, btBoxShape>::value)
        {
            return BulletShapeType::BoxShape;
        } else if constexpr(std::is_same<T, btSphereShape>::value)
        {
            return BulletShapeType::SphereShape;
        } else if constexpr(std::is_same<T, btConvexHullShape>::value)
        {
            return BulletShapeType::ConvexHullShape;
        } else if constexpr(std::is_same<T, btStaticPlaneShape>::value)
        {
            return BulletShapeType::StaticPlaneShape;
        } else if constexpr(std::is_same<T, btBvhTriangleMeshShape>::value)
        {
            return BulletShapeType::TriangleMeshShape;
        }
        return BulletShapeType::Size;
    }

    class BulletShape
    {
        public:
        /// @brief default constructor
        BulletShape() = default;

        /// @brief BulletShape factory
        ///
        /// Returns specified BulletShape instance object
        /// @tparam ...Args implementation detail
        /// @tparam T type of value to construct
        /// @param ...args can be 0 to n
        /// @return created BulletShape class object instance
        template<ConceptbtCollisionShapeDerived T, typename... Args>
        static BulletShape create(Args&&... args)
        {
            BulletShape tet;
            constexpr BulletShapeType cextype = getEnum<T>();
            tet.mType = cextype;
            static_assert(cextype != BulletShapeType::Size, "This class is not in the getEnum()!");
            tet.mData = new T(std::forward<Args>(args)...);
            return tet;
        };


        /// @brief assign an existing Collision Shape, replacing the current one (if there was one)
        void assign(btCollisionShape* collShape);

        /// @brief deleted copy constructor
        BulletShape(BulletShape&) = delete;

        /// @brief default move constructor
        BulletShape(BulletShape&& mv)
        {
            this->mData = mv.mData;
            this->mType = mv.mType;
            mv.mType = BulletShapeType::Size;
            mv.mData = nullptr;
        }

        /// @brief deleted copy assignment operator
        BulletShape& operator=(BulletShape&) = delete;

        /// @brief move assignment operator
        BulletShape& operator=(BulletShape&& cp)
        {
            this->mData = cp.mData;
            this->mType = cp.mType;
            cp.mType = BulletShapeType::Size;
            cp.mData = nullptr;
            return *this;
        }

        /// @brief deconstructs the class.
        ///
        /// Does not remove it from the physics world
        ~BulletShape();

        /// @brief returns false if ptr is nullptr
        operator bool() const
        {
            return mData != nullptr;
        }

        /// @brief 
        /// @tparam T one of the btCollisionShape-derived classes
        /// @return type T pointer, or nullptr/static_assert if failed
        template<ConceptbtCollisionShapeDerived T>
        T* get()const
        {
            constexpr BulletShapeType value = getEnum<T>();
            static_assert(value != BulletShapeType::Size, "This class is not in the getEnum()!");
            if constexpr(value == BulletShapeType::Size)
            {
                return nullptr;
            } else return static_cast<T*>(mData);
        }

        /// @brief get the raw void pointer
        /// @return void ptr. Can be nullptr
        void* getRaw()const{return mData;}

        /// @brief clear/erase the pointer and value
        ///
        /// DOES NOT REMOVE CollisionShape from the bullet World!
        ///
        /// Must be done manually!!
        void erase();

        private:

        /// @brief type of the value stored
        BulletShapeType mType = BulletShapeType::Size;
        /// @brief owning pointer to data buffer of allocated value in question
        void* mData = nullptr;
    };
}

#endif //CDF_TOOL_BULLETSHAPE_HPP