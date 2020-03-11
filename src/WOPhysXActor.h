#pragma once

#include <functional>
#include <memory>

#include "WO.h"
#include "PhysXEngine.h"

namespace Aftr {
// Class for PhysX RigidActor world objects
class WOPhysXActor : public WO {
public:
    WOMacroDeclaration(WOPhysXActor, WO);

    virtual ~WOPhysXActor();

    // pull pose data from PhysX (calls updateCallback)
    virtual void pullFromPhysX();
    // push pose data to PhysX
    virtual void pushToPhysX() const;

    // have to overload all position/rotation updating methods to push those
    // changes to PhysX
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

    // set WO's PhysXEngine, thus creating its PhysX Actor and data
    void setPhysXEngine(const std::shared_ptr<PhysXEngine>& engine);
    // set callback for when WO receives a PhysX update
    void setPhysXUpdateCallback(const std::function<void()>& callback);

protected:
    std::shared_ptr<PhysXEngine> physxEngine;
    physx::PxRigidActor* physxActor;
    std::function<void()> updateCallback;

    WOPhysXActor();
    virtual void createPhysXActor() = 0; // must be implemented by inheriting classes
};
}