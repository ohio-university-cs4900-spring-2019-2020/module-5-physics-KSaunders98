#pragma once

#include "WOPhysXActor.h"

namespace Aftr {
// class for a PhysX DynamicConvexMesh Actor
class WODynamicConvexMesh : public WOPhysXActor {
public:
    WOMacroDeclaration(WODynamicConvexMesh, WOPhysXActor);
    static WODynamicConvexMesh* New(const std::string& modelFileName, Vector scale = Vector(1, 1, 1), MESH_SHADING_TYPE shadingType = MESH_SHADING_TYPE::mstAUTO);

protected:
    WODynamicConvexMesh();
    virtual void createPhysXActor();
};
}