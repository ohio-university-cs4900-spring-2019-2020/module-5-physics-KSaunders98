#include "WOStaticTriangleMesh.h"

using namespace Aftr;
using namespace physx;

WOStaticTriangleMesh* WOStaticTriangleMesh::New(const std::string& modelFileName, Vector scale, MESH_SHADING_TYPE shadingType)
{
    WOStaticTriangleMesh* wo = new WOStaticTriangleMesh();
    wo->onCreate(modelFileName, scale, shadingType);
    return wo;
}

WOStaticTriangleMesh::WOStaticTriangleMesh()
    : IFace(this), WOPhysXActor()
{

}

void WOStaticTriangleMesh::createPhysXActor()
{
    physxActor = physxEngine->createTriangleMesh(this);
    pushToPhysX();
}
