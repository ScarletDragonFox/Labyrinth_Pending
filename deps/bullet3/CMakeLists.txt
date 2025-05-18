cmake_minimum_required(VERSION 3.21...3.31)

project(bullet3
    VERSION 3.2.5
    LANGUAGES C CXX 
    HOMEPAGE_URL "https://github.com/bulletphysics/bullet3"
    DESCRIPTION "Bullet Physics SDK: real-time collision detection and multi-physics simulation for VR, games, visual effects, robotics, machine learning etc.")

set(BULLET3_SOURCE_FILES
    ./src/clew/clew.c
    ./src/btBulletCollisionAll.cpp
    ./src/btBulletDynamicsAll.cpp
    ./src/btLinearMathAll.cpp
    ./src/Bullet3Collision/BroadPhaseCollision/b3DynamicBvh.cpp
    ./src/Bullet3Collision/BroadPhaseCollision/b3DynamicBvhBroadphase.cpp
    ./src/Bullet3Collision/BroadPhaseCollision/b3OverlappingPairCache.cpp
    ./src/Bullet3Collision/NarrowPhaseCollision/b3ConvexUtility.cpp
    ./src/Bullet3Collision/NarrowPhaseCollision/b3CpuNarrowPhase.cpp
    ./src/Bullet3Common/b3AlignedAllocator.cpp
    ./src/Bullet3Common/b3Logging.cpp
    ./src/Bullet3Common/b3Vector3.cpp
    ./src/Bullet3Dynamics/b3CpuRigidBodyPipeline.cpp
    ./src/Bullet3Dynamics/ConstraintSolver/b3FixedConstraint.cpp
    ./src/Bullet3Dynamics/ConstraintSolver/b3Generic6DofConstraint.cpp
    ./src/Bullet3Dynamics/ConstraintSolver/b3PgsJacobiSolver.cpp
    ./src/Bullet3Dynamics/ConstraintSolver/b3Point2PointConstraint.cpp
    ./src/Bullet3Dynamics/ConstraintSolver/b3TypedConstraint.cpp
    ./src/Bullet3Geometry/b3ConvexHullComputer.cpp
    ./src/Bullet3Geometry/b3GeometryUtil.cpp
    # ./src/Bullet3OpenCL/BroadphaseCollision/b3GpuGridBroadphase.cpp
    # ./src/Bullet3OpenCL/BroadphaseCollision/b3GpuParallelLinearBvh.cpp
    # ./src/Bullet3OpenCL/BroadphaseCollision/b3GpuParallelLinearBvhBroadphase.cpp
    # ./src/Bullet3OpenCL/BroadphaseCollision/b3GpuSapBroadphase.cpp
    # ./src/Bullet3OpenCL/Initialize/b3OpenCLUtils.cpp
    # ./src/Bullet3OpenCL/NarrowphaseCollision/b3ContactCache.cpp
    # ./src/Bullet3OpenCL/NarrowphaseCollision/b3ConvexHullContact.cpp
    # ./src/Bullet3OpenCL/NarrowphaseCollision/b3GjkEpa.cpp
    # ./src/Bullet3OpenCL/NarrowphaseCollision/b3OptimizedBvh.cpp
    # ./src/Bullet3OpenCL/NarrowphaseCollision/b3QuantizedBvh.cpp
    # ./src/Bullet3OpenCL/NarrowphaseCollision/b3StridingMeshInterface.cpp
    # ./src/Bullet3OpenCL/NarrowphaseCollision/b3TriangleCallback.cpp
    # ./src/Bullet3OpenCL/NarrowphaseCollision/b3TriangleIndexVertexArray.cpp
    # ./src/Bullet3OpenCL/NarrowphaseCollision/b3VoronoiSimplexSolver.cpp
    # ./src/Bullet3OpenCL/ParallelPrimitives/b3BoundSearchCL.cpp
    # ./src/Bullet3OpenCL/ParallelPrimitives/b3FillCL.cpp
    # ./src/Bullet3OpenCL/ParallelPrimitives/b3LauncherCL.cpp
    # ./src/Bullet3OpenCL/ParallelPrimitives/b3PrefixScanCL.cpp
    # ./src/Bullet3OpenCL/ParallelPrimitives/b3PrefixScanFloat4CL.cpp
    # ./src/Bullet3OpenCL/ParallelPrimitives/b3RadixSort32CL.cpp
    # ./src/Bullet3OpenCL/Raycast/b3GpuRaycast.cpp
    # ./src/Bullet3OpenCL/RigidBody/b3GpuGenericConstraint.cpp
    # ./src/Bullet3OpenCL/RigidBody/b3GpuJacobiContactSolver.cpp
    # ./src/Bullet3OpenCL/RigidBody/b3GpuNarrowPhase.cpp
    # ./src/Bullet3OpenCL/RigidBody/b3GpuPgsConstraintSolver.cpp
    # ./src/Bullet3OpenCL/RigidBody/b3GpuPgsContactSolver.cpp
    # ./src/Bullet3OpenCL/RigidBody/b3GpuRigidBodyPipeline.cpp
    # ./src/Bullet3OpenCL/RigidBody/b3Solver.cpp


    # ./src/Bullet3Serialize/Bullet2FileLoader/b3BulletFile.cpp
    # ./src/Bullet3Serialize/Bullet2FileLoader/b3Chunk.cpp
    # ./src/Bullet3Serialize/Bullet2FileLoader/b3DNA.cpp
    # ./src/Bullet3Serialize/Bullet2FileLoader/b3File.cpp
    # ./src/Bullet3Serialize/Bullet2FileLoader/b3Serializer.cpp


    ./src/BulletCollision/BroadphaseCollision/btAxisSweep3.cpp
    ./src/BulletCollision/BroadphaseCollision/btBroadphaseProxy.cpp
    ./src/BulletCollision/BroadphaseCollision/btCollisionAlgorithm.cpp
    ./src/BulletCollision/BroadphaseCollision/btDbvt.cpp
    ./src/BulletCollision/BroadphaseCollision/btDbvtBroadphase.cpp
    ./src/BulletCollision/BroadphaseCollision/btDispatcher.cpp
    ./src/BulletCollision/BroadphaseCollision/btOverlappingPairCache.cpp
    ./src/BulletCollision/BroadphaseCollision/btQuantizedBvh.cpp
    ./src/BulletCollision/BroadphaseCollision/btSimpleBroadphase.cpp
    ./src/BulletCollision/CollisionDispatch/btActivatingCollisionAlgorithm.cpp
    ./src/BulletCollision/CollisionDispatch/btBox2dBox2dCollisionAlgorithm.cpp
    ./src/BulletCollision/CollisionDispatch/btBoxBoxCollisionAlgorithm.cpp
    ./src/BulletCollision/CollisionDispatch/btBoxBoxDetector.cpp
    ./src/BulletCollision/CollisionDispatch/btCollisionDispatcher.cpp
    ./src/BulletCollision/CollisionDispatch/btCollisionDispatcherMt.cpp
    ./src/BulletCollision/CollisionDispatch/btCollisionObject.cpp
    ./src/BulletCollision/CollisionDispatch/btCollisionWorld.cpp
    ./src/BulletCollision/CollisionDispatch/btCollisionWorldImporter.cpp
    ./src/BulletCollision/CollisionDispatch/btCompoundCollisionAlgorithm.cpp
    ./src/BulletCollision/CollisionDispatch/btCompoundCompoundCollisionAlgorithm.cpp
    ./src/BulletCollision/CollisionDispatch/btConvex2dConvex2dAlgorithm.cpp
    ./src/BulletCollision/CollisionDispatch/btConvexConcaveCollisionAlgorithm.cpp
    ./src/BulletCollision/CollisionDispatch/btConvexConvexAlgorithm.cpp
    ./src/BulletCollision/CollisionDispatch/btConvexPlaneCollisionAlgorithm.cpp
    ./src/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.cpp
    ./src/BulletCollision/CollisionDispatch/btEmptyCollisionAlgorithm.cpp
    ./src/BulletCollision/CollisionDispatch/btGhostObject.cpp
    ./src/BulletCollision/CollisionDispatch/btHashedSimplePairCache.cpp
    ./src/BulletCollision/CollisionDispatch/btInternalEdgeUtility.cpp
    ./src/BulletCollision/CollisionDispatch/btManifoldResult.cpp
    ./src/BulletCollision/CollisionDispatch/btSimulationIslandManager.cpp
    ./src/BulletCollision/CollisionDispatch/btSphereBoxCollisionAlgorithm.cpp
    ./src/BulletCollision/CollisionDispatch/btSphereSphereCollisionAlgorithm.cpp
    ./src/BulletCollision/CollisionDispatch/btSphereTriangleCollisionAlgorithm.cpp
    ./src/BulletCollision/CollisionDispatch/btUnionFind.cpp
    ./src/BulletCollision/CollisionDispatch/SphereTriangleDetector.cpp
    ./src/BulletCollision/CollisionShapes/btBox2dShape.cpp
    ./src/BulletCollision/CollisionShapes/btBoxShape.cpp
    ./src/BulletCollision/CollisionShapes/btBvhTriangleMeshShape.cpp
    ./src/BulletCollision/CollisionShapes/btCapsuleShape.cpp
    ./src/BulletCollision/CollisionShapes/btCollisionShape.cpp
    ./src/BulletCollision/CollisionShapes/btCompoundShape.cpp
    ./src/BulletCollision/CollisionShapes/btConcaveShape.cpp
    ./src/BulletCollision/CollisionShapes/btConeShape.cpp
    ./src/BulletCollision/CollisionShapes/btConvex2dShape.cpp
    ./src/BulletCollision/CollisionShapes/btConvexHullShape.cpp
    ./src/BulletCollision/CollisionShapes/btConvexInternalShape.cpp
    ./src/BulletCollision/CollisionShapes/btConvexPointCloudShape.cpp
    ./src/BulletCollision/CollisionShapes/btConvexPolyhedron.cpp
    ./src/BulletCollision/CollisionShapes/btConvexShape.cpp
    ./src/BulletCollision/CollisionShapes/btConvexTriangleMeshShape.cpp
    ./src/BulletCollision/CollisionShapes/btCylinderShape.cpp
    ./src/BulletCollision/CollisionShapes/btEmptyShape.cpp
    ./src/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.cpp
    ./src/BulletCollision/CollisionShapes/btMiniSDF.cpp
    ./src/BulletCollision/CollisionShapes/btMinkowskiSumShape.cpp
    ./src/BulletCollision/CollisionShapes/btMultimaterialTriangleMeshShape.cpp
    ./src/BulletCollision/CollisionShapes/btMultiSphereShape.cpp
    ./src/BulletCollision/CollisionShapes/btOptimizedBvh.cpp
    ./src/BulletCollision/CollisionShapes/btPolyhedralConvexShape.cpp
    ./src/BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.cpp
    ./src/BulletCollision/CollisionShapes/btSdfCollisionShape.cpp
    ./src/BulletCollision/CollisionShapes/btShapeHull.cpp
    ./src/BulletCollision/CollisionShapes/btSphereShape.cpp
    ./src/BulletCollision/CollisionShapes/btStaticPlaneShape.cpp
    ./src/BulletCollision/CollisionShapes/btStridingMeshInterface.cpp
    ./src/BulletCollision/CollisionShapes/btTetrahedronShape.cpp
    ./src/BulletCollision/CollisionShapes/btTriangleBuffer.cpp
    ./src/BulletCollision/CollisionShapes/btTriangleCallback.cpp
    ./src/BulletCollision/CollisionShapes/btTriangleIndexVertexArray.cpp
    ./src/BulletCollision/CollisionShapes/btTriangleIndexVertexMaterialArray.cpp
    ./src/BulletCollision/CollisionShapes/btTriangleMesh.cpp
    ./src/BulletCollision/CollisionShapes/btTriangleMeshShape.cpp
    ./src/BulletCollision/CollisionShapes/btUniformScalingShape.cpp
    ./src/BulletCollision/Gimpact/btContactProcessing.cpp
    ./src/BulletCollision/Gimpact/btGenericPoolAllocator.cpp
    ./src/BulletCollision/Gimpact/btGImpactBvh.cpp
    ./src/BulletCollision/Gimpact/btGImpactCollisionAlgorithm.cpp
    ./src/BulletCollision/Gimpact/btGImpactQuantizedBvh.cpp
    ./src/BulletCollision/Gimpact/btGImpactShape.cpp
    ./src/BulletCollision/Gimpact/btTriangleShapeEx.cpp
    ./src/BulletCollision/Gimpact/gim_box_set.cpp
    ./src/BulletCollision/Gimpact/gim_contact.cpp
    ./src/BulletCollision/Gimpact/gim_memory.cpp
    ./src/BulletCollision/Gimpact/gim_tri_collision.cpp
    ./src/BulletCollision/NarrowPhaseCollision/btContinuousConvexCollision.cpp
    ./src/BulletCollision/NarrowPhaseCollision/btConvexCast.cpp
    ./src/BulletCollision/NarrowPhaseCollision/btGjkConvexCast.cpp
    ./src/BulletCollision/NarrowPhaseCollision/btGjkEpa2.cpp
    ./src/BulletCollision/NarrowPhaseCollision/btGjkEpaPenetrationDepthSolver.cpp
    ./src/BulletCollision/NarrowPhaseCollision/btGjkPairDetector.cpp
    ./src/BulletCollision/NarrowPhaseCollision/btMinkowskiPenetrationDepthSolver.cpp
    ./src/BulletCollision/NarrowPhaseCollision/btPersistentManifold.cpp
    ./src/BulletCollision/NarrowPhaseCollision/btPolyhedralContactClipping.cpp
    ./src/BulletCollision/NarrowPhaseCollision/btRaycastCallback.cpp
    ./src/BulletCollision/NarrowPhaseCollision/btSubSimplexConvexCast.cpp
    ./src/BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.cpp
    ./src/BulletDynamics/Character/btKinematicCharacterController.cpp
    ./src/BulletDynamics/ConstraintSolver/btBatchedConstraints.cpp
    ./src/BulletDynamics/ConstraintSolver/btConeTwistConstraint.cpp
    ./src/BulletDynamics/ConstraintSolver/btContactConstraint.cpp
    ./src/BulletDynamics/ConstraintSolver/btFixedConstraint.cpp
    ./src/BulletDynamics/ConstraintSolver/btGearConstraint.cpp
    ./src/BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.cpp
    ./src/BulletDynamics/ConstraintSolver/btGeneric6DofSpring2Constraint.cpp
    ./src/BulletDynamics/ConstraintSolver/btGeneric6DofSpringConstraint.cpp
    ./src/BulletDynamics/ConstraintSolver/btHinge2Constraint.cpp
    ./src/BulletDynamics/ConstraintSolver/btHingeConstraint.cpp
    ./src/BulletDynamics/ConstraintSolver/btNNCGConstraintSolver.cpp
    ./src/BulletDynamics/ConstraintSolver/btPoint2PointConstraint.cpp
    ./src/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.cpp
    ./src/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolverMt.cpp
    ./src/BulletDynamics/ConstraintSolver/btSliderConstraint.cpp
    ./src/BulletDynamics/ConstraintSolver/btSolve2LinearConstraint.cpp
    ./src/BulletDynamics/ConstraintSolver/btTypedConstraint.cpp
    ./src/BulletDynamics/ConstraintSolver/btUniversalConstraint.cpp
    ./src/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.cpp
    ./src/BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.cpp
    ./src/BulletDynamics/Dynamics/btRigidBody.cpp
    ./src/BulletDynamics/Dynamics/btSimpleDynamicsWorld.cpp
    ./src/BulletDynamics/Dynamics/btSimulationIslandManagerMt.cpp
    ./src/BulletDynamics/Featherstone/btMultiBody.cpp
    ./src/BulletDynamics/Featherstone/btMultiBodyConstraint.cpp
    ./src/BulletDynamics/Featherstone/btMultiBodyConstraintSolver.cpp
    ./src/BulletDynamics/Featherstone/btMultiBodyDynamicsWorld.cpp
    ./src/BulletDynamics/Featherstone/btMultiBodyFixedConstraint.cpp
    ./src/BulletDynamics/Featherstone/btMultiBodyGearConstraint.cpp
    ./src/BulletDynamics/Featherstone/btMultiBodyJointLimitConstraint.cpp
    ./src/BulletDynamics/Featherstone/btMultiBodyJointMotor.cpp
    ./src/BulletDynamics/Featherstone/btMultiBodyMLCPConstraintSolver.cpp
    ./src/BulletDynamics/Featherstone/btMultiBodyPoint2Point.cpp
    ./src/BulletDynamics/Featherstone/btMultiBodySliderConstraint.cpp
    ./src/BulletDynamics/Featherstone/btMultiBodySphericalJointLimit.cpp
    ./src/BulletDynamics/Featherstone/btMultiBodySphericalJointMotor.cpp
    ./src/BulletDynamics/MLCPSolvers/btDantzigLCP.cpp
    ./src/BulletDynamics/MLCPSolvers/btLemkeAlgorithm.cpp
    ./src/BulletDynamics/MLCPSolvers/btMLCPSolver.cpp
    ./src/BulletDynamics/Vehicle/btRaycastVehicle.cpp
    ./src/BulletDynamics/Vehicle/btWheelInfo.cpp
    ./src/BulletInverseDynamics/details/MultiBodyTreeImpl.cpp
    ./src/BulletInverseDynamics/details/MultiBodyTreeInitCache.cpp
    ./src/BulletInverseDynamics/IDMath.cpp
    ./src/BulletInverseDynamics/MultiBodyTree.cpp
    ./src/BulletSoftBody/btDefaultSoftBodySolver.cpp
    ./src/BulletSoftBody/btDeformableBackwardEulerObjective.cpp
    ./src/BulletSoftBody/btDeformableBodySolver.cpp
    ./src/BulletSoftBody/btDeformableContactConstraint.cpp
    ./src/BulletSoftBody/btDeformableContactProjection.cpp
    ./src/BulletSoftBody/btDeformableMultiBodyConstraintSolver.cpp
    ./src/BulletSoftBody/btDeformableMultiBodyDynamicsWorld.cpp
    ./src/BulletSoftBody/btSoftBody.cpp
    ./src/BulletSoftBody/btSoftBodyConcaveCollisionAlgorithm.cpp
    ./src/BulletSoftBody/btSoftBodyHelpers.cpp
    ./src/BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.cpp
    ./src/BulletSoftBody/btSoftMultiBodyDynamicsWorld.cpp
    ./src/BulletSoftBody/btSoftRigidCollisionAlgorithm.cpp
    ./src/BulletSoftBody/btSoftRigidDynamicsWorld.cpp
    ./src/BulletSoftBody/btSoftSoftCollisionAlgorithm.cpp
    ./src/BulletSoftBody/BulletReducedDeformableBody/btReducedDeformableBody.cpp
    ./src/BulletSoftBody/BulletReducedDeformableBody/btReducedDeformableBodyHelpers.cpp
    ./src/BulletSoftBody/BulletReducedDeformableBody/btReducedDeformableBodySolver.cpp
    ./src/BulletSoftBody/BulletReducedDeformableBody/btReducedDeformableContactConstraint.cpp
    ./src/BulletSoftBody/poly34.cpp
    ./src/LinearMath/btAlignedAllocator.cpp
    ./src/LinearMath/btConvexHull.cpp
    ./src/LinearMath/btConvexHullComputer.cpp
    ./src/LinearMath/btGeometryUtil.cpp
    ./src/LinearMath/btPolarDecomposition.cpp
    ./src/LinearMath/btQuickprof.cpp
    ./src/LinearMath/btReducedVector.cpp
    ./src/LinearMath/btSerializer.cpp
    ./src/LinearMath/btSerializer64.cpp
    ./src/LinearMath/btThreads.cpp
    ./src/LinearMath/btVector3.cpp
    ./src/LinearMath/TaskScheduler/btTaskScheduler.cpp
    ./src/LinearMath/TaskScheduler/btThreadSupportPosix.cpp
    ./src/LinearMath/TaskScheduler/btThreadSupportWin32.cpp

    
    ./include/bullet/Extras/Serialize/BulletFileLoader/bChunk.cpp
    ./include/bullet/Extras/Serialize/BulletFileLoader/bDNA.cpp
    ./include/bullet/Extras/Serialize/BulletFileLoader/bFile.cpp
    ./include/bullet/Extras/Serialize/BulletFileLoader/btBulletFile.cpp
    ./include/bullet/Extras/Serialize/BulletWorldImporter/btBulletWorldImporter.cpp
    ./include/bullet/Extras/Serialize/BulletWorldImporter/btMultiBodyWorldImporter.cpp
    ./include/bullet/Extras/Serialize/BulletWorldImporter/btWorldImporter.cpp
)

# in msys: (cd src && find . -name '*.h' -print | tar --create --files-from -) | (cd EXISTING_FOLDER_TO_WITCH_ALL_FILES_WILL_GO && tar xvfp -)


add_library(bullet3 STATIC ${BULLET3_SOURCE_FILES})
target_include_directories(bullet3 PUBLIC include include/bullet)
target_compile_options(bullet3 PRIVATE "-O3")