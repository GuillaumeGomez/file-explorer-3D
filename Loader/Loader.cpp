#include "Loader.hpp"
#include "ObjectFactory.hpp"
#include "../MyWindow.hpp"
#include "../myGLWidget.hpp"
#include "LoadingMenu.hpp"
#include "../GraphicHandler.hpp"
#include "../objects/GraphicFile.hpp"
#include "../objects/Room.hpp"
#include "../objects/Cube.hpp"
#include "../objects/Cylinder.hpp"
#include "../objects/Plane.hpp"
#include "../objects/Sphere.hpp"
#include "../objects/Model.hpp"
#include "../Utils/Directory.hpp"

#include <iostream>

using namespace std;
using namespace Object;

Loader::Loader(MyWindow *w) : m_win(w)
{
  objList.push_back(ObjectFactory::createNewObject<GraphicFile, Vector3D, Rotation, Color, const char*>(Vector3D(0.f, 2.f, -6.f), Rotation(), GREEN, "./GraphicHandler.cpp"));
  objList.push_back(ObjectFactory::createNewObject<Plane, Vector3D, Rotation, const char*, float, float>(Vector3D(), Rotation(90.f, 0.f, 0.f, 1.f), "./textures/metal.jpg", 10.f, 80.f));
  objList.push_back(ObjectFactory::createNewObject<Cube, Vector3D, Rotation, Color, float>(Vector3D(-30.f, 10.f, 100.f), Rotation(45.f, 0.f, 0.f, 0.f), Color(0.9f, 0.1f, 0.1f), 20.f));
  objList.push_back(ObjectFactory::createNewObject<Cylinder, Vector3D, Rotation, const char*, float, float>(Vector3D(), Rotation(), "textures/box.jpg", 10.f, 80.f));
  objList.push_back(ObjectFactory::createNewObject<Sphere, Vector3D, Rotation, Color, float>(Vector3D(0.f, -20.f, 100.f), Rotation(), Color(0.9f, 0.18f, 0.7f), 20.f));
  objList.push_back(ObjectFactory::createNewObject<Sphere, Vector3D, Rotation, const char*, float>(Vector3D(0.f, -70.f, 100.f), Rotation(0.f, 10.f, 1.f, 1.f, 0.f), "./textures/burn.jpg", 20.f));
  objList.push_back(ObjectFactory::createNewObject<Cube, Vector3D, Rotation, const char*, float>(Vector3D(-60.f, 10.f, 100.f), Rotation(45.f, 0.f, 1.f, 0.f), "./textures/grass.jpg", 10.f));
  objList.push_back(ObjectFactory::createNewObject<Cube, Vector3D, Rotation, const char*, float>(Vector3D(0.f, 10.f, 100.f), Rotation(45.f, 7.f, 1.f, 0.f), "./textures/box.jpg", 20.f));
  objList.push_back(ObjectFactory::createNewObject<Rectangle, Vector3D, Rotation, const char*, float, float, float>(Vector3D(-4.f, -3.f, -4.f), Rotation(170.f, 0.f, 1.f, 1.f), "./textures/grass.jpg", 2.f, 3.f, 1.f));
  objList.push_back(ObjectFactory::createNewObject<Model, Vector3D, Rotation, const char*, const char*, float>(Vector3D(0.f, 0.f, 10.f), Rotation(), "models/bob/spongebob_bind.obj", "models/bob/spongebob_bind.mtl", 5.f));

  const int MAX(30);
  for (int i = 0; i< MAX; ++i)
    for (int j = 0; j < MAX; j++)
      objList.push_back(ObjectFactory::createNewObject<Cube, Vector3D, Rotation, Color, float>(Vector3D(j * 24 + 50, i * 24, /*z */ -24), Rotation(), Color(j / 5.f, i / 5.f, /*z */ 5.f), 18.f));

  m_load = new LoadingMenu(objList.size(), m_win);
}

Loader::~Loader()
{
  if (m_load)
    delete m_load;
}

void  Loader::loadDatas()
{
  /*#ifdef WIN32
  Room  *r = new Room(Vector3D(0.f, 0.f, 0.f), Rotation(), "C:/Users/imperio/Projects/My_World_SDL/My_World_SDL/My_World_SDL");
#else
  Room  *r = new Room(Vector3D(0.f, 0.f, 0.f), Rotation(), "/windows/Users/imperio/Projects/My_World_SDL/My_World_SDL/My_World_SDL");
#endif
  r->initializeGL();
  m_win->addObject(r);*/


  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  if (m_load)
    m_load->draw();
  while (objList.size() > 0)
    {
      ObjectFactory *tmp = (*objList.begin());

      if (tmp)
        {
          myGLWidget *obj = tmp->createObject();

          if (obj)
            {
              obj->initializeGL();
              m_win->addObject(obj, false/*tmp->is2D*/);
            }
          delete tmp;
        }
      objList.pop_front();
      if (m_load)
        m_load->newLoadedObject();
    }
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  if (m_load) {
      delete m_load;
      m_load = 0;
    }
}
