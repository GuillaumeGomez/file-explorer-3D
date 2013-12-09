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
#include "../Utils/Directory.hpp"

#include <iostream>

using namespace std;
using namespace Object;

Loader::Loader(MyWindow *w) : m_win(w)
{
  objList.push_back(new SubObjectFactory<Cube, Vector3D, Rotation, Color, float>(Vector3D(-30.f, 10.f, 100.f), Rotation(45.f, 0.f, 0.f, 0.f), Color(0.9f, 0.1f, 0.1f), 20.f));
  objList.push_back(new SubObjectFactory<Cylinder, Vector3D, Rotation, const char*, float, float>(Vector3D(), Rotation(), "./textures/box.jpg", 10.f, 80.f));
  objList.push_back(new SubObjectFactory<Plane, Vector3D, Rotation, const char*, float, float>(Vector3D(), Rotation(90.f, 0.f, 0.f, 1.f), "./textures/metal.jpg", 10.f, 80.f));
#ifndef WIN32
  objList.push_back(ObjectCreator::sphere(Vector3D(0.f, -70.f, 100.f), Rotation(0.f, 10.f, 1.f, 1.f, 0.f), "/windows/Users/imperio/Projects/My_World_SDL/My_World_SDL/My_World_SDL/textures/burn.jpg", 20));
  objList.push_back(ObjectCreator::cube(Vector3D(-60.f, 10.f, 100.f), Rotation(45.f, 0.f, 1.f, 0.f), "/windows/Users/imperio/Projects/My_World_SDL/My_World_SDL/My_World_SDL//textures/grass.jpg", 10));
  objList.push_back(ObjectCreator::cube(Vector3D(0.f, 10.f, 100.f), Rotation(45.f, 0.7f, 1.f, 0.f), "/windows/Users/imperio/Projects/My_World_SDL/My_World_SDL/My_World_SDL//textures/grass.jpg", 20));
#else
  objList.push_back(new SubObjectFactory<Sphere, Vector3D, Rotation, Color, float>(Vector3D(0.f, -20.f, 100.f), Rotation(), Color(0.9f, 0.18f, 0.7f), 20.f));
  objList.push_back(new SubObjectFactory<Sphere, Vector3D, Rotation, const char*, float>(Vector3D(0.f, -70.f, 100.f), Rotation(0.f, 10.f, 1.f, 1.f, 0.f), "./textures/burn.jpg", 20.f));
  objList.push_back(new SubObjectFactory<Cube, Vector3D, Rotation, const char*, float>(Vector3D(-60.f, 10.f, 100.f), Rotation(45.f, 0.f, 1.f, 0.f), "./textures/grass.jpg", 10.f));
  objList.push_back(new SubObjectFactory<Cube, Vector3D, Rotation, const char*, float>(Vector3D(0.f, 10.f, 100.f), Rotation(45.f, 7.f, 1.f, 0.f), "./textures/box.jpg", 20.f));
  objList.push_back(new SubObjectFactory<Rectangle, Vector3D, Rotation, const char*, float, float, float>(Vector3D(-4.f, -3.f, -4.f), Rotation(170.f, 0.f, 1.f, 1.f), "./textures/grass.jpg", 2.f, 3.f, 1.f));
#endif

  const int MAX(30);
  for (int i = 0; i< MAX; ++i)
    for (int j = 0; j < MAX; j++)
      objList.push_back(new SubObjectFactory<Cube, Vector3D, Rotation, Color, float>(Vector3D(j * 24 + 50, i * 24, /*z */ -24), Rotation(), Color(j / 5.f, i / 5.f, /*z */ 5.f), 18.f));

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
        m_load->newLoadedObject(m_win->getLib());
    }
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  if (m_load) {
      delete m_load;
      m_load = 0;
    }
}
