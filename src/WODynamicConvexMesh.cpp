#include "WODynamicConvexMesh.h"

using namespace Aftr;
using namespace physx;

WODynamicConvexMesh* WODynamicConvexMesh::New(const std::string& modelFileName, Vector scale, MESH_SHADING_TYPE shadingType)
{
    WODynamicConvexMesh* wo = new WODynamicConvexMesh();
    wo->onCreate(modelFileName, scale, shadingType);
    return wo;
}

WODynamicConvexMesh::WODynamicConvexMesh()
    : IFace(this)
    , WOPhysXActor()
{
}

void WODynamicConvexMesh::createPhysXActor()
{
    physxActor = physxEngine->createConvexMesh(this);
    pushToPhysX();
}
