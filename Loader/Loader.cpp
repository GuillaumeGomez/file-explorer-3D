#include "Loader.hpp"
#include "ObjectFactory.hpp"
#include "../MyWindow.hpp"
#include "../myGLWidget.hpp"
#include "LoadingMenu.hpp"
#include "../HandleSDL.hpp"
#include "../objects/GraphicFile.hpp"
#include "../objects/Room.hpp"
#include "../objects/Cube.hpp"
#include "../objects/Cylinder.hpp"
#include "../objects/Plane.hpp"
#include "../objects/Sphere.hpp"
#include "../objects/Model.hpp"
#include "../objects/Line.hpp"
#include "../objects/Button.hpp"
#include "../Utils/Directory.hpp"
#include "../objects/HeightMap.hpp"

#include <iostream>
#include <string>

#include <assimp/version.h>

using namespace std;
using namespace Object;

Loader::Loader(MyWindow *w) : m_win(w)
{
    std::cout << "assimp version : " << aiGetVersionMajor() << "." << aiGetVersionMinor() << std::endl;
    objList.push_back(ObjectFactory::createCharacter<Object::Model, Vector3D, Rotation, const char*, float>(Vector3D(10.f, 20.f, 10.f), Rotation(), "models/dwarf/dwarf2.b3d", 4.f));
    objList.push_back(ObjectFactory::createNewObject<Object::Model, Vector3D, Rotation, const char*, float>(Vector3D(0.f, 40.f, 10.f), Rotation(90.f, 0.f, 0.f, -1.f), "models/guard/boblampclean.md5mesh", 6.f));
    //objList.push_back(ObjectFactory::createNewObject<Model, Vector3D, Rotation, const char*, float>(Vector3D(10.f, 30.f, 5.f), Rotation(), "models/Cartoon Girl/girl-cartoon.obj", 4.f));

    //objList.push_back(ObjectFactory::createCharacter<Model, Vector3D, Rotation, const char*, float>(Vector3D(0.f, 20.f, 10.f), Rotation(), "models/starting.obj", 3.f));
    objList.push_back(ObjectFactory::createNewObject<GraphicFile, Vector3D, Rotation, Color, const char*>(Vector3D(0.f, 2.f, -6.f), Rotation(), GREEN, "Camera.cpp"));
    objList.push_back(ObjectFactory::createNewObject<Object::Plane, Vector3D, Rotation, const char*, float, float>(Vector3D(), Rotation(90.f, 0.f, 0.f, 1.f), "textures/metal.jpg", 10.f, 80.f));
    objList.push_back(ObjectFactory::createNewObject<Cube, Vector3D, Rotation, Color, float>(Vector3D(-30.f, 10.f, 100.f), Rotation(45.f, 0.f, 0.f, 0.f), Color(0.9f, 0.1f, 0.1f), 20.f));
    objList.push_back(ObjectFactory::createNewObject<Cylinder, Vector3D, Rotation, const char*, float, float>(Vector3D(), Rotation(), "textures/box.jpg", 10.f, 80.f));
    objList.push_back(ObjectFactory::createNewObject<Sphere, Vector3D, Rotation, Color, float>(Vector3D(15.f, 20.f, 100.f), Rotation(), Color(0.9f, 0.18f, 0.7f), 9.f));
    objList.push_back(ObjectFactory::createNewObject<Sphere, Vector3D, Rotation, Color, float>(Vector3D(2.f, 90.f, 100.f), Rotation(0.f, 10.f, 1.f, 1.f, 0.f), BLUE, 10.f));
    objList.push_back(ObjectFactory::createNewObject<Sphere, Vector3D, Rotation, const char*, float>(Vector3D(0.f, 70.f, 100.f), Rotation(0.f, 10.f, 1.f, 1.f, 0.f), "textures/burn.jpg", 20.f));
    objList.push_back(ObjectFactory::createNewObject<Cube, Vector3D, Rotation, const char*, float>(Vector3D(-60.f, 10.f, 100.f), Rotation(45.f, 0.f, 1.f, 0.f), "textures/grass.jpg", 10.f));
    objList.push_back(ObjectFactory::createNewObject<Cube, Vector3D, Rotation, const char*, float>(Vector3D(0.f, 10.f, 100.f), Rotation(45.f, 7.f, 1.f, 1.f), "textures/box.jpg", 20.f));
    objList.push_back(ObjectFactory::createNewObject<Rectangle, Vector3D, Rotation, const char*, float, float, float>(Vector3D(-4.f, -3.f, -4.f), Rotation(170.f, 0.f, 1.f, 1.f), "textures/grass.jpg", 2.f, 3.f, 1.f));
    objList.push_back(ObjectFactory::createNewObject<Line, Vector3D, Vector3D, Color>(Vector3D(2.f, 2.f, 9.f), Vector3D(0.f, -70.f, 100.f), WHITE));
    objList.push_back(ObjectFactory::createNewObject<Object::Model, Vector3D, Rotation, const char*, float>(Vector3D(0.f, 0.f, 10.f), Rotation(), "models/Wolf/Wolf.obj", 3.f));
    objList.push_back(ObjectFactory::createNewObject<Object::Model, Vector3D, Rotation, const char*, float>(Vector3D(2.f, 0.f, 9.f), Rotation(), "models/bob/spongebob_bind.obj", 4.f));
    objList.push_back(ObjectFactory::createNew2DObject<Object::Plane, Vector3D, Rotation, const char*, float, float, bool>(Vector3D(-0.05f, -0.05f), Rotation(), "textures/cross.png", 0.1f, 0.1f, false));
    objList.push_back(ObjectFactory::createNewPauseObject<Button, const char*, Color, Color, Vector3D>("Test", RED, BLUE, Vector3D(-0.25f, -0.1f), 0.5f, 0.2f));
    objList.push_back(ObjectFactory::createNewObject<HeightMap, Vector3D, int, int, float>(Vector3D(-2090.f, -10.f, 5.f), 200, 200, 10.f));
    objList.push_back(ObjectFactory::createNewObject<HeightMap, Vector3D, std::string, float>(Vector3D(-100.f, -10.f, 5.f), "textures/heightmap/description.bmp", 10.f));

    //objList.push_back(ObjectFactory::createNewObject<Object::Model, Vector3D, Rotation, const char*, float>(Vector3D(2.f, 20.f, 9.f), Rotation(-90.f, 0.f, 0.f, 1.f), "models/stormtrooper.fbx", 2.f));

    /*const int MAX(17);
  int z = 0;
  for (int i = 0; i < MAX; ++i)
    for (int j = 0; j < MAX; j++)
      for (z = 0; z < MAX; ++z)
        objList.push_back(ObjectFactory::createNewObject<Cube, Vector3D, Rotation, Color, float>(Vector3D(j * 24 + 50, i * 24, z * -24), Rotation(), Color(j / float(MAX), i / float(MAX), z / float(MAX)), 10.f));
*/
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
                if (!tmp->m_isMainCharacter)
                    m_win->addObject(obj, tmp->m_isPauseObject);
                else
                    m_win->setMainCharacter(obj);
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
