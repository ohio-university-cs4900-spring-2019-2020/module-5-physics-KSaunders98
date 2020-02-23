#pragma once

#include <map>

#include "PxPhysicsAPI.h"

namespace Aftr {
class ModelDataSharedID;
class WOPhysXActor;

class PhysXEngine {
public:
    PhysXEngine();
    ~PhysXEngine();
    PhysXEngine(const PhysXEngine& other) = delete;
    PhysXEngine& operator=(const PhysXEngine& other) = delete;

    void shutdown();

    physx::PxPhysics* getPhysics() { return physics; }
    physx::PxScene* getScene() { return scene; }
    physx::PxFoundation* getFoundation() { return foundation; }

    physx::PxRigidActor* createTriangleMesh(WOPhysXActor* wo);
    physx::PxRigidActor* createConvexMesh(WOPhysXActor* wo);

    void destroyActor(physx::PxActor* actor);

    void updateSimulation(float dt);

private:
    physx::PxDefaultAllocator allocator;
    physx::PxDefaultErrorCallback errCallback;
    physx::PxFoundation* foundation;
    physx::PxPhysics* physics;
    physx::PxCooking* cooking;
    physx::PxScene* scene;
    physx::PxDefaultCpuDispatcher* dispatcher;
    physx::PxPvd* pvd;
    physx::PxMaterial* defaultMaterial;

    std::map<ModelDataSharedID, physx::PxShape*> triangleMeshShapes;
    std::map<ModelDataSharedID, physx::PxShape*> convexMeshShapes;
};
}