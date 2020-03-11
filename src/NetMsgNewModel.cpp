#include "NetMsgNewModel.h"

#include <sstream>

#include "GLViewPhysicsModule.h"
#include "ManagerGLView.h"

using namespace Aftr;

NetMsgMacroDefinition(NetMsgNewModel);

NetMsgNewModel::NetMsgNewModel() {
    path = "";
    scale = Vector(1, 1, 1);
}

bool NetMsgNewModel::toStream(NetMessengerStreamBuffer& os) const
{
    os << path;
    os << scale.x << scale.y << scale.z;
    os << position.x << position.y << position.z;

    return true;
}

bool NetMsgNewModel::fromStream(NetMessengerStreamBuffer& is)
{
    is >> path;
    is >> scale.x >> scale.y >> scale.z;
    is >> position.x >> position.y >> position.z;

    return true;
}

void NetMsgNewModel::onMessageArrived()
{
    // call spawnNewModel in GLView
    ManagerGLView::getGLView<GLViewPhysicsModule>()->spawnNewModel(path, scale, position, false);
}

std::string NetMsgNewModel::toString() const
{
    std::stringstream ss;
    ss << "NewModel | " << path << " | " << scale;
    return ss.str();
}