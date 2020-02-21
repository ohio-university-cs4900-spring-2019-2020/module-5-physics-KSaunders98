#include "WOPhysXActor.h"
//#include "Mat4.h"
#include "Model.h"

using namespace Aftr;
using namespace physx;

WOPhysXActor::WOPhysXActor()
    : IFace(this), WO()
{
    physxEngine = nullptr;
    physxActor = nullptr;
}

WOPhysXActor::~WOPhysXActor()
{
    if (physxEngine != nullptr && physxActor != nullptr)
        physxEngine->destroyActor(physxActor);
}

void WOPhysXActor::pullFromPhysX()
{
    PxTransform t = physxActor->getGlobalPose();
    PxMat44 m = PxMat44(t);
    PxVec3 p = t.p;

    Mat4 mat;
    for (unsigned int i = 0; i < 3; ++i) {
        for (unsigned int j = 0; j < 3; ++j) {
            mat[i * 4 + j] = m[i][j];
        }
    }
    getModel()->setDisplayMatrix(mat);
    setPosition(p.x, p.y, p.z);
}

void WOPhysXActor::pushToPhysX()
{
    Mat4 mat = getDisplayMatrix();
    Vector p = getPosition();

    PxMat44 m;
    for (unsigned int i = 0; i < 3; ++i) {
        for (unsigned int j = 0; j < 3; ++j) {
            m[i][j] = mat[i * 4 + j];
        }
    }
    m[3] = PxVec4(p.x, p.y, p.z, 1.0f);
    physxActor->setGlobalPose(PxTransform(m));
}

void WOPhysXActor::setPosition(const Vector& newXYZ)
{
    WO::setPosition(newXYZ);

    PxTransform t = physxActor->getGlobalPose();
    t.p = PxVec3(newXYZ.x, newXYZ.y, newXYZ.z);
    physxActor->setGlobalPose(t);
}

void WOPhysXActor::setPosition(float x, float y, float z)
{
    WO::setPosition(x, y, z);

    PxTransform t = physxActor->getGlobalPose();
    t.p = PxVec3(x, y, z);
    physxActor->setGlobalPose(t);
}

void WOPhysXActor::setPositionIgnoringAllChildren(const Vector& newXYZ)
{
    WO::setPositionIgnoringAllChildren(newXYZ);

    PxTransform t = physxActor->getGlobalPose();
    t.p = PxVec3(newXYZ.x, newXYZ.y, newXYZ.z);
    physxActor->setGlobalPose(t);
}

void WOPhysXActor::moveRelative(const Vector& dXdYdZ)
{
    WO::moveRelative(dXdYdZ);

    PxTransform t = physxActor->getGlobalPose();
    t.p += PxVec3(dXdYdZ.x, dXdYdZ.y, dXdYdZ.z);
    physxActor->setGlobalPose(t);
}

void WOPhysXActor::moveRelativeIgnoringAllChildren(const Vector& dXdYdZ)
{
    WO::moveRelativeIgnoringAllChildren(dXdYdZ);

    PxTransform t = physxActor->getGlobalPose();
    t.p += PxVec3(dXdYdZ.x, dXdYdZ.y, dXdYdZ.z);
    physxActor->setGlobalPose(t);
}

void WOPhysXActor::rotateToIdentity()
{
    WO::rotateToIdentity();

    pushToPhysX();
}

void WOPhysXActor::rotateAboutRelX(float deltaRadianAngle)
{
    WO::rotateAboutRelX(deltaRadianAngle);

    pushToPhysX();
}

void WOPhysXActor::rotateAboutRelY(float deltaRadianAngle)
{
    WO::rotateAboutRelY(deltaRadianAngle);

    pushToPhysX();
}

void WOPhysXActor::rotateAboutRelZ(float deltaRadianAngle)
{
    WO::rotateAboutRelZ(deltaRadianAngle);

    pushToPhysX();
}

void WOPhysXActor::rotateAboutGlobalX(float deltaRadianAngle)
{
    WO::rotateAboutGlobalX(deltaRadianAngle);

    pushToPhysX();
}

void WOPhysXActor::rotateAboutGlobalY(float deltaRadianAngle)
{
    WO::rotateAboutGlobalY(deltaRadianAngle);

    pushToPhysX();
}

void WOPhysXActor::rotateAboutGlobalZ(float deltaRadianAngle)
{
    WO::rotateAboutGlobalZ(deltaRadianAngle);

    pushToPhysX();
}

void WOPhysXActor::setPhysXEngine(const std::shared_ptr<PhysXEngine>& engine) {
    physxEngine = engine;
    createPhysXActor();
}