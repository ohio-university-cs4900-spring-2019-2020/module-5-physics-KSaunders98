#include "PhysXEngine.h"

#include <iostream>

#include "Model.h"
#include "WOPhysXActor.h"

using namespace Aftr;
using namespace physx;

PhysXEngine::PhysXEngine()
{
    foundation = PxCreateFoundation(PX_PHYSICS_VERSION, allocator, errCallback);

    pvd = PxCreatePvd(*foundation);
    PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
    pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

    physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale(), true, pvd);
    cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, PxCookingParams(PxTolerancesScale()));

    PxSceneDesc s(physics->getTolerancesScale());
    s.gravity = PxVec3(0.0f, 0.0f, -9.81f);
    dispatcher = PxDefaultCpuDispatcherCreate(4);
    s.cpuDispatcher = dispatcher;
    s.filterShader = PxDefaultSimulationFilterShader;
    s.flags = PxSceneFlag::eENABLE_ACTIVE_ACTORS;
    scene = physics->createScene(s);

    PxPvdSceneClient* pvdClient = scene->getScenePvdClient();
    if (pvdClient) {
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }

    defaultMaterial = physics->createMaterial(0.5f, 0.5f, 0.6f);

    std::cout << "Successfully initialized PhysX engine" << std::endl;
}

PhysXEngine::~PhysXEngine()
{
    shutdown();
}

void PhysXEngine::shutdown()
{
    // release shape maps
    for (auto const& x : triangleMeshShapes) {
        x.second->release();
    }
    for (auto const& x : convexMeshShapes) {
        x.second->release();
    }
    triangleMeshShapes.clear();
    convexMeshShapes.clear();

    if (defaultMaterial != nullptr) {
        defaultMaterial->release();
        defaultMaterial = nullptr;
    }
    if (scene != nullptr) {
        scene->release();
        scene = nullptr;
    }
    if (physics != nullptr) {
        physics->release();
        physics = nullptr;
    }
    if (cooking != nullptr) {
        cooking->release();
        cooking = nullptr;
    }
    if (pvd != nullptr) {
        PxPvdTransport* transport = pvd->getTransport();
        pvd->release();
        pvd = nullptr;
        if (transport != nullptr) {
            transport->release();
        }
    }
    if (foundation != nullptr) {
        foundation->release();
        foundation = nullptr;
    }
}

PxRigidActor* PhysXEngine::createTriangleMesh(WOPhysXActor* wo)
{
    ModelDataShared* modelData = wo->getModel()->getModelDataShared();
    ModelDataSharedID modelID(modelData->getFileName(), modelData->getInitialScaleFactor());
    PxShape* shape;

    auto it = triangleMeshShapes.find(modelID);
    if (it == triangleMeshShapes.end()) {
        const std::vector<Vector>& verts = wo->getModel()->getCompositeVertexList();
        const std::vector<unsigned int>& inds = wo->getModel()->getCompositeIndexList();

        // describe triangle mesh geometry
        PxTriangleMeshDesc desc;
        desc.points.count = PxU32(verts.size());
        desc.points.stride = sizeof(Vector);
        desc.points.data = &verts.front();
        desc.triangles.count = PxU32(inds.size() / 3);
        desc.triangles.stride = sizeof(unsigned int) * 3;
        desc.triangles.data = &inds.front();

        // cook geometry into triangle mesh
        PxDefaultMemoryOutputStream buf;
        if (!cooking->cookTriangleMesh(desc, buf))
            exit(-1);
        PxDefaultMemoryInputData stream(buf.getData(), buf.getSize());
        PxTriangleMesh* triangleMesh = physics->createTriangleMesh(stream);
        
        // create shape and add it to map
        shape = physics->createShape(PxTriangleMeshGeometry(triangleMesh), *defaultMaterial);
        triangleMeshShapes.insert(std::make_pair(modelID, shape));
    } else {
        // reuse existing shape
        shape = it->second;
    }

    // create actor and add it to scene
    PxRigidStatic* actor = PxCreateStatic(*physics, PxTransform(PxVec3(0, 0, 0)), *shape);
    scene->addActor(*actor);
    actor->userData = wo;

    return actor;
}

PxRigidActor* PhysXEngine::createConvexMesh(WOPhysXActor* wo)
{
    ModelDataShared* modelData = wo->getModel()->getModelDataShared();
    ModelDataSharedID modelID(modelData->getFileName(), modelData->getInitialScaleFactor());
    PxShape* shape;

    auto it = convexMeshShapes.find(modelID);
    if (it == convexMeshShapes.end()) {
        const std::vector<Vector>& verts = wo->getModel()->getCompositeVertexList();
        const std::vector<unsigned int>& inds = wo->getModel()->getCompositeIndexList();

        // describe convex mesh geometry
        PxConvexMeshDesc desc;
        desc.points.count = PxU32(verts.size());
        desc.points.stride = sizeof(Vector);
        desc.points.data = &verts.front();
        desc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

        // cook geometry into convex mesh
        PxDefaultMemoryOutputStream buf;
        if (!cooking->cookConvexMesh(desc, buf))
            exit(-1);
        PxDefaultMemoryInputData stream(buf.getData(), buf.getSize());
        PxConvexMesh* triangleMesh = physics->createConvexMesh(stream);

        // create shape and add it to map
        shape = physics->createShape(PxConvexMeshGeometry(triangleMesh), *defaultMaterial);
        convexMeshShapes.insert(std::make_pair(modelID, shape));
    } else {
        // reuse existing shape
        shape = it->second;
    }

    // create actor and add it to scene
    PxRigidDynamic* actor = PxCreateDynamic(*physics, PxTransform(PxVec3(0, 0, 0)), *shape, PxReal(2.0f));
    scene->addActor(*actor);
    actor->userData = wo;

    return actor;
}

void PhysXEngine::destroyActor(PxActor* actor)
{
    if (scene != nullptr) {
        scene->removeActor(*actor);
        actor->release();
    }
}

void PhysXEngine::updateSimulation(float dt)
{
    scene->simulate(dt);
    scene->fetchResults(true);

    PxU32 numActors = 0;
    PxActor** actors = scene->getActiveActors(numActors);
    if (numActors > 0 && actors != nullptr) {
        for (size_t i = 0; i < numActors; ++i) {
            if (actors[i]->userData != nullptr) {
                static_cast<WOPhysXActor*>(actors[i]->userData)->pullFromPhysX();
            }
        }
    }
}
