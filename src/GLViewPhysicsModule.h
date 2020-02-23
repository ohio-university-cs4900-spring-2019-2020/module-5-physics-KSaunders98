#pragma once

#include <memory>

#include "GLView.h"

namespace Aftr {
class Camera;
class NetMessengerClient;
class PhysXEngine;

/**
   \class GLViewPhysicsModule
   \author Scott Nykl 
   \brief A child of an abstract GLView. This class is the top-most manager of the module.

   Read \see GLView for important constructor and init information.

   \see GLView

    \{
*/

class GLViewPhysicsModule : public GLView {
public:
    static GLViewPhysicsModule* New(const std::vector<std::string>& outArgs);
    virtual ~GLViewPhysicsModule();
    virtual void updateWorld(); ///< Called once per frame
    virtual void loadMap(); ///< Called once at startup to build this module's scene
    virtual void onResizeWindow(GLsizei width, GLsizei height);
    virtual void onMouseDown(const SDL_MouseButtonEvent& e);
    virtual void onMouseDownSelection(unsigned int x, unsigned int y, Camera& cam);
    virtual void onMouseUp(const SDL_MouseButtonEvent& e);
    virtual void onMouseMove(const SDL_MouseMotionEvent& e);
    virtual void onKeyDown(const SDL_KeyboardEvent& key);
    virtual void onKeyUp(const SDL_KeyboardEvent& key);
    void spawnNewModel(const std::string& path, const Vector& scale, const Vector& position);
    void updateModel(unsigned int id, const Mat4& displayMatrix, const Vector& position);

protected:
    GLViewPhysicsModule(const std::vector<std::string>& args);
    virtual void onCreate();

    std::string teapotPath;
    std::shared_ptr<PhysXEngine> physxEngine;
    std::shared_ptr<NetMessengerClient> netClient;
};

/** \} */

} //namespace Aftr
