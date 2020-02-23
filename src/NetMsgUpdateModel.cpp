#include "NetMsgUpdateModel.h"

#include <sstream>

#include "GLViewPhysicsModule.h"
#include "ManagerGLView.h"

using namespace Aftr;

NetMsgMacroDefinition(NetMsgUpdateModel);

NetMsgUpdateModel::NetMsgUpdateModel()
{
    id = 0;
    displayMatrix = Mat4();
    position = Vector(0, 0, 0);
}

bool NetMsgUpdateModel::toStream(NetMessengerStreamBuffer& os) const
{
    os << id;
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            os << displayMatrix[i * 4 + j];
        }
    }
    os << position.x << position.y << position.z;

    return true;
}

bool NetMsgUpdateModel::fromStream(NetMessengerStreamBuffer& is)
{
    is >> id;
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            is >> displayMatrix[i * 4 + j];
        }
    }
    displayMatrix[15] = 1;
    is >> position.x >> position.y >> position.z;

    return true;
}

void NetMsgUpdateModel::onMessageArrived()
{
    // call spawnNewModel in GLView
    ManagerGLView::getGLView<GLViewPhysicsModule>()->updateModel(id, displayMatrix, position);
}

std::string NetMsgUpdateModel::toString() const
{
    std::stringstream ss;
    ss << "UpdateModel | " << id << " | " << displayMatrix << " | " << position;
    return ss.str();
}