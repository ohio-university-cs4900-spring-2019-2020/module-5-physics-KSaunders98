#pragma once

#include "WOPhysXActor.h"

namespace Aftr {
class WOStaticTriangleMesh : public WOPhysXActor {
public:
    WOMacroDeclaration(WOStaticTriangleMesh, WOPhysXActor);
    static WOStaticTriangleMesh* New(const std::string& modelFileName, Vector scale = Vector(1, 1, 1), MESH_SHADING_TYPE shadingType = MESH_SHADING_TYPE::mstAUTO);

protected:
    WOStaticTriangleMesh();
    virtual void createPhysXActor();
};
}