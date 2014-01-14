#ifndef HANDLEPHYSICS_HPP
#define HANDLEPHYSICS_HPP

#include <btBulletDynamicsCommon.h>
#include <vector>

class myGLWidget;
namespace Object
{
  class Cube;
}

class HandlePhysics
{
public:
  HandlePhysics();
  virtual ~HandlePhysics();
  bool        addObject(myGLWidget *obj);
  void        deleteObject(myGLWidget *obj);
  void        update(float const&);
  myGLWidget  *pick(int mouseX, int mouseY, int screenWidth, int screenHeight);

private:
  btBroadphaseInterface *broadphase;
  btDefaultCollisionConfiguration *collisionConfiguration;
  btCollisionDispatcher *dispatcher;
  btSequentialImpulseConstraintSolver *solver;
  btDiscreteDynamicsWorld *dynamicsWorld;
  std::vector<btRigidBody*> rigidBodies;
  std::vector<Object::Cube*>        debugCube;
};

#endif // HANDLEPHYSICS_HPP
