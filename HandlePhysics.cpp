#include "HandlePhysics.hpp"
#include "myGLWidget.hpp"
#include "Camera.hpp"
#include "objects/Line.hpp"

//add drawDebugWorld

class DebugDrawer : public btIDebugDraw
{
protected:
  int m_debugMode;
  Object::Line  *line;

public:
  DebugDrawer() {
    line = new Object::Line(Vector3D(), Vector3D(), WHITE);
    line->initializeGL();
    m_debugMode = 0;
  }
  virtual ~DebugDrawer() {
    delete line;
  }

  void  drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) {
    std::vector<GLfloat> &v = line->getVertices();

    for (int i = 0; i < 3; ++i) {
        v[i] = from[i];
        v[i + 3] = to[i];
      }
    (void)color;
    line->updateVertices(v);
    line->paintGL(Camera::getViewMatrix(), Camera::getProjectionMatrix());
  }
  void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color){}
  void reportErrorWarning(const char*s){}
  void draw3dText(const btVector3 &b, const char *s){}
  void setDebugMode(int x){m_debugMode = x;}
  int  getDebugMode() const{return m_debugMode;}
};

HandlePhysics::HandlePhysics()
{
  // Build the broadphase
  broadphase = new btDbvtBroadphase();

  // Set up the collision configuration and dispatcher
  collisionConfiguration = new btDefaultCollisionConfiguration();
  dispatcher = new btCollisionDispatcher(collisionConfiguration);

  // The actual physics solver
  solver = new btSequentialImpulseConstraintSolver;

  // The world.
  dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
  dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));

  DebugDrawer *tt = new DebugDrawer;
  tt->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
  dynamicsWorld->setDebugDrawer(tt);
}

HandlePhysics::~HandlePhysics()
{
  for (auto it = rigidBodies.begin(); it != rigidBodies.end(); ++it){
      dynamicsWorld->removeRigidBody(*it);
      delete (*it)->getMotionState();
      //delete (*it)->getCollisionShape();
      delete (*it);
      rigidBodies.erase(it);
    }
  delete dynamicsWorld;
  delete solver;
  delete dispatcher;
  delete collisionConfiguration;
  delete broadphase;
}

bool  findDataInVector(std::vector<btRigidBody*> &v, myGLWidget *value)
{
  for (auto it = v.begin(); it != v.end(); ++it){
      if ((*it)->getUserPointer() == value)
        return true;
    }
  return false;
}

bool  HandlePhysics::addObject(myGLWidget *obj)
{
  if (!obj || findDataInVector(rigidBodies, obj))
    return false;
  if (obj->getClassName() != "Cube" && obj->getClassName() != "GraphicFile")
    return false;
  //if (rigidBodies.size() > 20)
  //return false;
  /*btTriangleMesh* mesh = new btTriangleMesh();

  std::vector<GLfloat>  v = obj->getVertices();*/
  Vector3D pos = obj->getPosition();
  Rotation rot = obj->rotation();

  /*for (unsigned int i = 0; i < v.size();)
    {
      btVector3 bv[3];

      for (int x = 0; x < 3; ++x) {
          bv[x] = btVector3(v[i], v[i + 1], v[i + 2]);
          i += 3;
        }
      mesh->addTriangle(bv[0], bv[1], bv[2]);
    }
  btConvexTriangleMeshShape *shape = new btConvexTriangleMeshShape(mesh, true);
  //btBvhTriangleMeshShape *shape = new btBvhTriangleMeshShape(mesh, true);*/

  static btCollisionShape* boxCollisionShape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));

  btDefaultMotionState* motionstate = new btDefaultMotionState(btTransform(
                                                                 btQuaternion(rot.y(), rot.x(), rot.z()),
                                                                 btVector3(pos.x(), pos.y(), pos.z())
                                                                 ));

  btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
        0,                  // mass, in kg. 0 -> Static object, will never move.
        motionstate,
        boxCollisionShape,  // collision shape of body
        btVector3(0,0,0)    // local inertia
        );
  btRigidBody *rigidBody = new btRigidBody(rigidBodyCI);

  rigidBodies.push_back(rigidBody);
  dynamicsWorld->addRigidBody(rigidBody);
  rigidBody->setUserPointer(obj);

  return true;
}

void  HandlePhysics::deleteObject(myGLWidget *obj)
{
  for (auto it = rigidBodies.begin(); it != rigidBodies.end(); ++it){
      if ((*it)->getUserPointer() == obj) {
          dynamicsWorld->removeRigidBody(*it);
          delete (*it)->getMotionState();
          //delete (*it)->getCollisionShape();
          delete (*it);
          rigidBodies.erase(it);
          return;
        }
    }
}

myGLWidget  *HandlePhysics::pick(int mouseX, int mouseY, int screenWidth, int screenHeight)
{
  dynamicsWorld->debugDrawWorld();

  glm::vec4 lRayStart_NDC(((float)mouseX/(float)screenWidth  - 0.5f) * 2.f, // [0,1024] -> [-1,1]
                          ((float)mouseY/(float)screenHeight - 0.5f) * 2.f, // [0, 768] -> [-1,1]
                          -1.f, // The near plane maps to Z=-1 in Normalized Device Coordinates
                          1.f);
  glm::vec4 lRayEnd_NDC(((float)mouseX/(float)screenWidth  - 0.5f) * 2.f,
                        ((float)mouseY/(float)screenHeight - 0.5f) * 2.f,
                        0.f,
                        1.f);

  //glm::mat4 InverseProjectionMatrix = glm::inverse(Camera::getProjectionMatrix());

  // The View Matrix goes from World Space to Camera Space.
  // So inverse(ViewMatrix) goes from Camera Space to World Space.
  /*glm::mat4 InverseViewMatrix = glm::inverse(Camera::getViewMatrix());

  glm::vec4 lRayStart_camera = InverseProjectionMatrix * lRayStart_NDC;    lRayStart_camera/=lRayStart_camera.w;
  glm::vec4 lRayStart_world  = InverseViewMatrix       * lRayStart_camera; lRayStart_world /=lRayStart_world .w;
  glm::vec4 lRayEnd_camera   = InverseProjectionMatrix * lRayEnd_NDC;      lRayEnd_camera  /=lRayEnd_camera  .w;
  glm::vec4 lRayEnd_world    = InverseViewMatrix       * lRayEnd_camera;   lRayEnd_world   /=lRayEnd_world   .w;*/
  glm::mat4 M = glm::inverse(Camera::getProjectionMatrix() * Camera::getViewMatrix());
  glm::vec4 lRayStart_world = M * lRayStart_NDC; lRayStart_world/=lRayStart_world.w;
  glm::vec4 lRayEnd_world   = M * lRayEnd_NDC  ; lRayEnd_world  /=lRayEnd_world.w;

  glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
  lRayDir_world = glm::normalize(lRayDir_world);

  glm::vec3 out_origin = glm::vec3(lRayStart_world);
  glm::vec3 out_direction = glm::normalize(lRayDir_world);

  out_direction = out_direction * 1000.f;

  btCollisionWorld::ClosestRayResultCallback RayCallback(btVector3(out_origin.x, out_origin.y, out_origin.z),
                                                         btVector3(out_direction.x, out_direction.y, out_direction.z));
  dynamicsWorld->rayTest(btVector3(out_origin.x, out_origin.y, out_origin.z),
                         btVector3(out_direction.x, out_direction.y, out_direction.z),
                         RayCallback);

  if (RayCallback.hasHit()) {
      return static_cast<myGLWidget*>(RayCallback.m_collisionObject->getUserPointer());
    }
  return 0;
}

void  HandlePhysics::update(const float &t)
{
  dynamicsWorld->updateAabbs();
  dynamicsWorld->stepSimulation(t / 1000.f);
}
