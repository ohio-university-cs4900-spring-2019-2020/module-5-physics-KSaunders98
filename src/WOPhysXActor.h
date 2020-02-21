#pragma once

#include <memory>

#include "WO.h"
#include "PhysXEngine.h"

namespace Aftr {
class WOPhysXActor : public WO {
public:
    WOMacroDeclaration(WOPhysXActor, WO);

    virtual ~WOPhysXActor();

    virtual void pullFromPhysX();
    virtual void pushToPhysX();

    virtual void setPosition(const Vector& newXYZ);
    virtual void setPosition(float x, float y, float z);
    virtual void setPositionIgnoringAllChildren(const Vector& newXYZ);
    virtual void moveRelative(const Vector& dXdYdZ);
    virtual void moveRelativeIgnoringAllChildren(const Vector& dXdYdZ);

    virtual void rotateToIdentity();
    virtual void rotateAboutRelX(float deltaRadianAngle);
    virtual void rotateAboutRelY(float deltaRadianAngle);
    virtual void rotateAboutRelZ(float deltaRadianAngle);
    virtual void rotateAboutGlobalX(float deltaRadianAngle);
    virtual void rotateAboutGlobalY(float deltaRadianAngle);
    virtual void rotateAboutGlobalZ(float deltaRadianAngle);

    void setPhysXEngine(const std::shared_ptr<PhysXEngine>& engine);

protected:
    std::shared_ptr<PhysXEngine> physxEngine;
    physx::PxRigidActor* physxActor;
    WOPhysXActor();
    virtual void createPhysXActor() = 0; // must be implemented by inheriting classes
};
}