#include "GLViewPhysicsModule.h"

#include <chrono>

#include "Axes.h" //We can set Axes to on/off with this
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this
#include "PhysXEngine.h"
#include "PhysicsEngineODE.h"
#include "WorldList.h" //This is where we place all of our WOs
#include "WODynamicConvexMesh.h"
#include "WOStaticTriangleMesh.h"

//Different WO used by this module
#include "AftrGLRendererBase.h"
#include "Camera.h"
#include "CameraChaseActorAbsNormal.h"
#include "CameraChaseActorRelNormal.h"
#include "CameraChaseActorSmooth.h"
#include "CameraStandard.h"
#include "Model.h"
#include "ModelDataShared.h"
#include "ModelMesh.h"
#include "ModelMeshDataShared.h"
#include "ModelMeshSkin.h"
#include "WO.h"
#include "WOCar1970sBeater.h"
#include "WOHumanCal3DPaladin.h"
#include "WOHumanCyborg.h"
#include "WOLight.h"
#include "WONVDynSphere.h"
#include "WONVPhysX.h"
#include "WONVStaticPlane.h"
#include "WOSkyBox.h"
#include "WOStatic.h"
#include "WOStaticPlane.h"
#include "WOStaticTrimesh.h"
#include "WOTrimesh.h"
#include "WOWayPointSpherical.h"

//If we want to use way points, we need to include this.
#include "PhysicsModuleWayPoints.h"

using namespace Aftr;
using namespace physx;

GLViewPhysicsModule* GLViewPhysicsModule::New(const std::vector<std::string>& args)
{
    GLViewPhysicsModule* glv = new GLViewPhysicsModule(args);
    glv->init(Aftr::GRAVITY, Vector(0, 0, -1.0f), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE);
    glv->onCreate();
    return glv;
}

GLViewPhysicsModule::GLViewPhysicsModule(const std::vector<std::string>& args)
    : GLView(args)
{
    //Initialize any member variables that need to be used inside of LoadMap() here.
    //Note: At this point, the Managers are not yet initialized. The Engine initialization
    //occurs immediately after this method returns (see GLViewPhysicsModule::New() for
    //reference). Then the engine invoke's GLView::loadMap() for this module.
    //After loadMap() returns, GLView::onCreate is finally invoked.

    //The order of execution of a module startup:
    //GLView::New() is invoked:
    //    calls GLView::init()
    //       calls GLView::loadMap() (as well as initializing the engine's Managers)
    //    calls GLView::onCreate()

    //GLViewPhysicsModule::onCreate() is invoked after this module's LoadMap() is completed.

    physxEngine = std::make_shared<PhysXEngine>();
}

void GLViewPhysicsModule::onCreate()
{
    //GLViewPhysicsModule::onCreate() is invoked after this module's LoadMap() is completed.
    //At this point, all the managers are initialized. That is, the engine is fully initialized.

    if (this->pe != NULL) {
        //optionally, change gravity direction and magnitude here
        //The user could load these values from the module's aftr.conf
        this->pe->setGravityNormalizedVector(Vector(0, 0, -1.0f));
        this->pe->setGravityScalar(Aftr::GRAVITY);
    }
    this->setActorChaseType(STANDARDEZNAV); //Default is STANDARDEZNAV mode
    //this->setNumPhysicsStepsPerRender( 0 ); //pause physics engine on start up; will remain paused till set to 1
}

GLViewPhysicsModule::~GLViewPhysicsModule()
{
    //Implicitly calls GLView::~GLView()
    physxEngine->shutdown();
}

void GLViewPhysicsModule::updateWorld()
{
    GLView::updateWorld(); //Just call the parent's update world first.
        //If you want to add additional functionality, do it after
        //this call.

    using namespace std::chrono;

    // calculate delta time
    static auto last_time = steady_clock::now();
    auto now = steady_clock::now();
    float dt = duration_cast<duration<float>>(now - last_time).count();
    last_time = now;

    physxEngine->updateSimulation(dt);
}

void GLViewPhysicsModule::onResizeWindow(GLsizei width, GLsizei height)
{
    GLView::onResizeWindow(width, height); //call parent's resize method.
}

void GLViewPhysicsModule::onMouseDown(const SDL_MouseButtonEvent& e)
{
    GLView::onMouseDown(e);
}

void GLViewPhysicsModule::onMouseDownSelection(unsigned int x, unsigned int y, Camera& cam) {
    GLView::onMouseDownSelection(x, y, cam);

    if (getLastSelectedWO() != nullptr) {
        Vector pos = *getLastSelectedCoordinate() + Vector(0, 0, 15);

        WOPhysXActor* teapot = WODynamicConvexMesh::New(ManagerEnvironmentConfiguration::getLMM() + "/models/teapot.obj", Vector(2, 2, 2), MESH_SHADING_TYPE::mstFLAT);
        teapot->setPhysXEngine(physxEngine);
        teapot->setPosition(pos);
        teapot->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
        worldLst->push_back(teapot);
    }
}


void GLViewPhysicsModule::onMouseUp(const SDL_MouseButtonEvent& e)
{
    GLView::onMouseUp(e);
}

void GLViewPhysicsModule::onMouseMove(const SDL_MouseMotionEvent& e)
{
    GLView::onMouseMove(e);
}

void GLViewPhysicsModule::onKeyDown(const SDL_KeyboardEvent& key)
{
    GLView::onKeyDown(key);
    if (key.keysym.sym == SDLK_0)
        this->setNumPhysicsStepsPerRender(1);

    if (key.keysym.sym == SDLK_1) {
    }
}

void GLViewPhysicsModule::onKeyUp(const SDL_KeyboardEvent& key)
{
    GLView::onKeyUp(key);
}

void Aftr::GLViewPhysicsModule::loadMap()
{
    this->worldLst = new WorldList(); //WorldList is a 'smart' vector that is used to store WO*'s
    this->actorLst = new WorldList();
    this->netLst = new WorldList();

    ManagerOpenGLState::GL_CLIPPING_PLANE = 1000.0;
    ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
    ManagerOpenGLState::enableFrustumCulling = false;
    Axes::isVisible = true;
    this->glRenderer->isUsingShadowMapping(false); //set to TRUE to enable shadow mapping, must be using GL 3.2+

    this->cam->setPosition(50, 50, 50);
    this->cam->setCameraLookAtPoint(Vector(0, 0, 0));

    std::string shinyRedPlasticCube(ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl");
    std::string wheeledCar(ManagerEnvironmentConfiguration::getSMM() + "/models/rcx_treads.wrl");
    std::string grass(ManagerEnvironmentConfiguration::getSMM() + "/models/grassFloor400x400_pp.wrl");
    std::string human(ManagerEnvironmentConfiguration::getSMM() + "/models/human_chest.wrl");

    //SkyBox Textures readily available
    std::vector<std::string> skyBoxImageNames; //vector to store texture paths
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_water+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_dust+6.jpg" );
    skyBoxImageNames.push_back(ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_mountains+6.jpg");
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_winter+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/early_morning+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_afternoon+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy3+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day2+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_deepsun+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_evening+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning2+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_noon+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_warp+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_Hubble_Nebula+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_gray_matter+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_easter+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_hot_nebula+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_ice_field+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_lemon_lime+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_milk_chocolate+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_solar_bloom+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_thick_rb+6.jpg" );

    float ga = 0.1f; //Global Ambient Light level for this module
    ManagerLight::setGlobalAmbientLight(aftrColor4f(ga, ga, ga, 1.0f));
    WOLight* light = WOLight::New();
    light->isDirectionalLight(true);
    light->setPosition(Vector(0, 0, 100));
    //Set the light's display matrix such that it casts light in a direction parallel to the -z axis (ie, downwards as though it was "high noon")
    //for shadow mapping to work, this->glRenderer->isUsingShadowMapping( true ), must be invoked.
    light->getModel()->setDisplayMatrix(Mat4::rotateIdentityMat({ 0, 1, 0 }, 90.0f * Aftr::DEGtoRAD));
    light->setLabel("Light");
    worldLst->push_back(light);

    //Create the SkyBox
    WO* wo = WOSkyBox::New(skyBoxImageNames.at(0), this->getCameraPtrPtr());
    wo->setPosition(Vector(0, 0, 0));
    wo->setLabel("Sky Box");
    wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
    worldLst->push_back(wo);

    WOPhysXActor* mountain = WOStaticTriangleMesh::New(ManagerEnvironmentConfiguration::getLMM() + "/models/mountain.obj", Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
    mountain->setPhysXEngine(physxEngine);
    mountain->setPosition(Vector(0, 0, 18));
    mountain->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
    worldLst->push_back(mountain);
    std::cout << wo->getModel()->getBoundingBox() << std::endl;

    WOPhysXActor* teapot = WODynamicConvexMesh::New(ManagerEnvironmentConfiguration::getLMM() + "/models/teapot.obj", Vector(2, 2, 2), MESH_SHADING_TYPE::mstFLAT);
    teapot->setPhysXEngine(physxEngine);
    teapot->setPosition(Vector(0, 0, 25));
    teapot->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
    worldLst->push_back(teapot);

    createPhysicsModuleWayPoints();
}

void GLViewPhysicsModule::createPhysicsModuleWayPoints()
{
    // Create a waypoint with a radius of 3, a frequency of 5 seconds, activated by GLView's camera, and is visible.
    WayPointParametersBase params(this);
    params.frequency = 5000;
    params.useCamera = true;
    params.visible = true;
    WOWayPointSpherical* wayPt = WOWP1::New(params, 3);
    wayPt->setPosition(Vector(50, 0, 3));
    worldLst->push_back(wayPt);
}
