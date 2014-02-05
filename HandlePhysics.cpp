#include "HandlePhysics.hpp"
#include "myGLWidget.hpp"
#include "Camera.hpp"
#include "objects/Line.hpp"
#include "objects/Sphere.hpp"
#include "Utils/HandleFile.hpp"
#include "String_utils.hpp"
#include "objects/HeightMap.hpp"
#include "objects/Point.hpp"
#include "shaders/ShaderHandler.hpp"

#include <BulletCollision/CollisionShapes/btShapeHull.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

//add drawDebugWorld

class DebugDrawer : public btIDebugDraw
{
protected:
  int m_debugMode;
  std::vector<GLfloat>  lines;
  Object::Point *m_p;
  bool  init, re_init;
  int   pos;
  GLuint  m_vboID;
  GLuint  m_vaoID;
  Shader  *m_shader;
  GLint                   m_uniLoc_modelView;
  GLint                   m_uniLoc_projection;

public:
  DebugDrawer() {
    m_debugMode = 0;
    init = false;
    re_init = true;
    m_p = new Object::Point(Vector3D(), WHITE);
    m_p->initializeGL();

    std::string vert = "#version 330\n"
        "in vec3 in_Vertex;\n"

        "uniform mat4 projection;\n"
        "uniform mat4 modelview;\n"

        "out vec3 color;\n"

        "mat4 my_translate(mat4 ori, vec3 t_pos){\n"
        "mat4 tmp = ori;\n"
        "tmp[3] = ori[0] * t_pos.x + ori[1] * t_pos.y + ori[2] * t_pos.z + ori[3];\n"
        "return tmp;\n}\n"

        "void main(){\n"
        "gl_Position = projection * modelview * vec4(in_Vertex, 1.0);\n"

        "color = vec3(1.0, 1.0, 1.0);\n"
        "}";

    if (!(m_shader = ShaderHandler::getInstance()->createShader(vert, Shader::getStandardFragmentShader(false)))) {
        exit(-14);
      }

    m_uniLoc_projection = glGetUniformLocation(m_shader->getProgramID(), "projection");
    m_uniLoc_modelView = glGetUniformLocation(m_shader->getProgramID(), "modelview");
  }
  void  draw()
  {
    if (!init) {
        glGenBuffers(1, &m_vboID);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
        glBufferData(GL_ARRAY_BUFFER, lines.size(), 0, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, lines.size(), &lines[0]);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenVertexArrays(1, &m_vaoID);
        glBindVertexArray(m_vaoID);

        glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

        glVertexAttribPointer(VERTEX_COORD, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
        glEnableVertexAttribArray(VERTEX_COORD);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
      }
    else if (!re_init) {
        glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
        glBufferData(GL_ARRAY_BUFFER, lines.size(), 0, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, lines.size(), &lines[0]);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
      }

    glUseProgram(m_shader->getProgramID());

    glBindVertexArray(m_vaoID);

    glUniformMatrix4fv(m_uniLoc_projection, 1, GL_FALSE, glm::value_ptr(Camera::getProjectionMatrix()));
    glUniformMatrix4fv(m_uniLoc_modelView, 1, GL_FALSE, glm::value_ptr(Camera::getViewMatrix()));

    glDrawArrays(GL_LINES, 0, lines.size() / 3);

    glBindVertexArray(0);
    glUseProgram(0);

    init = true;
    pos = 0;
  }

  virtual ~DebugDrawer() {
    delete m_p;
  }

  void  drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) {
    if (!init) {
        lines.push_back(from.x());
        lines.push_back(from.y());
        lines.push_back(from.z());

        lines.push_back(to.x());
        lines.push_back(to.y());
        lines.push_back(to.z());
      } else if (pos < lines.size() - 6) {
        lines[pos++] = from.x();
        lines[pos++] = from.y();
        lines[pos++] = from.z();

        lines[pos++] = to.x();
        lines[pos++] = to.y();
        lines[pos++] = to.z();
      } else {
        lines.push_back(from.x());
        lines.push_back(from.y());
        lines.push_back(from.z());

        lines.push_back(to.x());
        lines.push_back(to.y());
        lines.push_back(to.z());

        pos += 6;
        re_init = true;
      }
  }
  void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color){
    (void)normalOnB;
    (void)distance;
    (void)lifeTime;
    std::vector<GLfloat>  c;

    c.push_back(color[0]);
    c.push_back(color[1]);
    c.push_back(color[2]);
    m_p->updateColors(c);
    m_p->setPosition(Vector3D(PointOnB.x(), PointOnB.y(), PointOnB.z()));
    m_p->paintGL(Camera::getViewMatrix(), Camera::getProjectionMatrix());
  }
  void reportErrorWarning(const char*s){(void)s;}
  void draw3dText(const btVector3 &b, const char *s){(void)b;(void)s;}
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

  /*btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,1,0),1);

  btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-1,0)));
  btRigidBody::btRigidBodyConstructionInfo
      groundRigidBodyCI(0,groundMotionState,groundShape,btVector3(0,0,0));
  btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
  dynamicsWorld->addRigidBody(groundRigidBody);*/
}

HandlePhysics::~HandlePhysics()
{
  /*for (auto it = rigidBodies.begin(); it != rigidBodies.end(); ++it){
      dynamicsWorld->removeRigidBody(*it);
      delete (*it)->getMotionState();
      //delete (*it)->getCollisionShape();
      delete (*it);
      rigidBodies.erase(it);
    }*/
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
  if (obj->getClassName() != "Cube" && obj->getClassName() != "GraphicFile" && obj->getClassName() != "Sphere"
      && obj->getClassName() != "HeightMap")
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

  if (obj->getClassName() == "HeightMap") {
      Object::HeightMap *t = static_cast<Object::HeightMap*>(obj);
      std::vector<GLfloat>  &tmp = t->getVertices();
      static std::vector<GLfloat> tmp2;

      static std::vector<int>  ss;

      int x = 0;
      for (unsigned int i = 0; i < tmp.size() / 3; i += 6) {
          ss.push_back(x);
          ss.push_back(x + 1);
          ss.push_back(x + 2);

          ss.push_back(x + 3);
          ss.push_back(x + 2);
          ss.push_back(x + 1);

          x += 2;
        }
      btTriangleIndexVertexArray  *caca = new btTriangleIndexVertexArray(ss.size() / 3, &ss[0], sizeof(ss[0]) * 3,
          tmp.size(), &tmp[0], sizeof(tmp[0]) * 3);
      /*btIndexedMesh part;

      part.m_vertexBase = (const unsigned char*)LandscapeVtx[i];
      part.m_vertexStride = sizeof(btScalar) * 3;
      part.m_numVertices = LandscapeVtxCount[i];
      part.m_triangleIndexBase = (const unsigned char*)&ss[0];
      part.m_triangleIndexStride = sizeof( short) * 3;
      part.m_numTriangles = ss.size() / 3;
      part.m_indexType = PHY_SHORT;
      caca->addIndexedMesh(part,PHY_SHORT);*/

      btBvhTriangleMeshShape* trimeshShape = new btBvhTriangleMeshShape(caca,true);



      /*for (auto it : tmp)
        tmp2.push_back(it / t->getMaxHeight());
      btHeightfieldTerrainShape *groundShape = new btHeightfieldTerrainShape(t->getWidth(), t->getHeight(), &tmp2[0],
          1, t->getMinHeight(), t->getMaxHeight(), 1, PHY_FLOAT, true);

      groundShape->setUseDiamondSubdivision(true);
      groundShape->setLocalScaling(btVector3(t->getCaseSize(), 1, t->getCaseSize()));*/

      /*btTriangleMesh  *tri = new btTriangleMesh;

      auto it = obj->getVertices();

      for (unsigned int i = 0; i < it.size(); i += 9) {
          tri->addTriangle(btVector3(it[i], it[i + 1], it[i + 2]),
              btVector3(it[i + 3], it[i + 6], it[i + 5]),
              btVector3(it[i + 6], it[i + 7], it[i + 8]));
        }
      btConvexShape *tmpshape = new btConvexTriangleMeshShape(tri);
      btShapeHull* hull = new btShapeHull(tmpshape);
      btScalar margin = tmpshape->getMargin();
      hull->buildHull(margin);*/

      /*btConvexHullShape* convexShape = new btConvexHullShape();
      bool updateLocalAabb = false;

      for (int i=0;i<hull->numVertices();i++)
        {
          convexShape->addPoint(hull->getVertexPointer()[i], updateLocalAabb);
        }
      convexShape->recalcLocalAabb();

      bool sEnableSAT = false;
      if (sEnableSAT)
        convexShape->initializePolyhedralFeatures();
      delete tmpshape;
      delete hull;*/

      btDefaultMotionState* motionstate = new btDefaultMotionState(btTransform(
                                                                     btQuaternion(rot.y(), rot.x(), rot.z()),
                                                                     btVector3(pos.x(), pos.y(), pos.z())
                                                                     ));

      btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
            0,                  // mass, in kg. 0 -> Static object, will never move.
            motionstate,
            trimeshShape,//convexShape,  // collision shape of body
            btVector3(0,0,0)    // local inertia
            );
      btRigidBody *rigidBody = new btRigidBody(rigidBodyCI);

      rigidBodies.push_back(rigidBody);
      dynamicsWorld->addRigidBody(rigidBody);
      rigidBody->setUserPointer(obj);
    }
  else if (obj->getClassName() != "Sphere"){
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
    }
  else {
      static btCollisionShape* sphereCollisionShape = new btSphereShape(static_cast<Object::Sphere*>(obj)->getRadius());

      btDefaultMotionState* motionstate = new btDefaultMotionState(btTransform(
                                                                     btQuaternion(rot.y(), rot.x(), rot.z(), 1),
                                                                     btVector3(pos.x(), pos.y(), pos.z())
                                                                     ));

      btVector3 fallInertia(0,0,0);
      btScalar mass = 5;
      sphereCollisionShape->calculateLocalInertia(mass, fallInertia);
      btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionstate, sphereCollisionShape, fallInertia);
      btRigidBody *rigidBody = new btRigidBody(rigidBodyCI);

      dynamicsWorld->addRigidBody(rigidBody);
      rigidBody->setUserPointer(obj);
      dynBodies.push_back(rigidBody);
    }
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
  static_cast<DebugDrawer*>(dynamicsWorld->getDebugDrawer())->draw();

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
  //dynamicsWorld->updateAabbs();
  dynamicsWorld->stepSimulation(t);
  for (btRigidBody *it : dynBodies) {
      btTransform trans;
      it->getMotionState()->getWorldTransform(trans);
      const btVector3 &p = trans.getOrigin();
      //const btQuaternion &r = it->getWorldTransform().getRotation();

      static_cast<myGLWidget*>(it->getUserPointer())->setPosition(Vector3D(p.getX(), p.getY(), p.getZ()));
      //static_cast<myGLWidget*>(it->getUserPointer())->rotation();

      /*static HandleFile f("res.txt");

      f.open();
      f.write("sphere height: " + Utility::toString<float>(trans.getOrigin().getY()) + "\n");
      f.flush();*/
    }
}
