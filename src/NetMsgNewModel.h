#pragma once

#include <string>

#include "NetMsg.h"
#include "Vector.h"

#ifdef AFTR_CONFIG_USE_BOOST

namespace Aftr {
// message for creating a new model
class NetMsgNewModel : public NetMsg {
public:
    NetMsgMacroDeclaration(NetMsgNewModel);

    NetMsgNewModel();
    virtual bool toStream(NetMessengerStreamBuffer& os) const;
    virtual bool fromStream(NetMessengerStreamBuffer& is);
    virtual void onMessageArrived();
    virtual std::string toString() const;

    std::string path;
    Vector scale;
    Vector position;
};
}

#endif