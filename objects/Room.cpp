#include "Room.hpp"
#include "../Utils/Directory.hpp"
#include "Door.hpp"
#include "../Utils/MyException.hpp"
#include "Plane.hpp"
#include "Cylinder.hpp"
#include "Wall.hpp"
#include "GraphicFile.hpp"
#include <cmath>
#include "Rotation.hpp"

#include <string>

using namespace Object;

float my_abs(float const &x)
{
    return x > 0.f ? x : -x;
}

Room::Room(Vector3D p, Rotation r, std::string path) : myGLWidget(p, r)
{
    m_pickAllow = true;
    /*if (!path) {
        throw MyException(std::string("Room: Invalid path"));
    }*/
    try {
        Directory   dir(path);
        int         pos(0);
        float       decal(170.f);
        //float       x(m_pos.x());

        //partie dossier
        vector<string> v = dir.getDirectoryList();
        int tmp = v.size();
        float tmp_x;

        if (tmp & 1)
            tmp_x = (decal * (v.size() / 2)) + decal;
        else
            tmp_x = decal * (v.size() / 2);

#ifdef WIN32
        //objects.push_back(new Plane(m_pos, Rotation(), "./textures/metal.jpg", 160.f, tmp_x));
#else
        //objects.push_back(new Plane(m_pos, Rotation(), "/windows/Users/imperio/Projects/My_World_SDL/My_World_SDL/My_World_SDL/textures/metal.jpg", 160.f, tmp_x));
#endif
        //objects.push_back(new Plane(Vector3D(m_pos.x(), m_pos.y() + 130.f, m_pos.z()), Rotation(), ORANGE, 160.f, tmp_x));

        /*for (vector<string>::iterator it = v.begin(); it != v.end(); ++it) {
            Door  *t = new Door(Vector3D(x + 10.f, m_pos.y(), m_pos.z() + (pos & 1 ? -80.f : 80.f)), Rotation(), (*it).c_str());
#ifdef WIN32
            objects.push_back(new Cylinder(Vector3D(x, m_pos.y(), m_pos.z() + (pos & 1 ? -80.f : 80.f)), Rotation(-90.f, 0.f, 0.f, 1.f), "./textures/box.jpg", 20.f, 130.f));
#else
            objects.push_back(new Cylinder(Vector3D(x, m_pos.y(), m_pos.z() + (pos & 1 ? -80.f : 80.f)), Rotation(-90.f, 0.f, 0.f, 1.f), "/windows/Users/imperio/Projects/My_World_SDL/My_World_SDL/My_World_SDL/textures/box.jpg", 20.f, 130.f));
#endif
            if (!t)
                continue;
            dirs.push_back(t);
            if (pos & 1)
                x += decal;
            pos++;
        }
        if (pos & 1){
#ifdef WIN32
            objects.push_back(new Cylinder(Vector3D(x, m_pos.y(), m_pos.z() -80.f), Rotation(-90.f, 0.f, 0.f, 1.f), "./textures/box.jpg", 20.f, 130.f));
#else
            objects.push_back(new Cylinder(Vector3D(x, m_pos.y(), m_pos.z() -80.f), Rotation(-90.f, 0.f, 0.f, 1.f), "/windows/Users/imperio/Projects/My_World_SDL/My_World_SDL/My_World_SDL/textures/box.jpg", 20.f, 130.f));
#endif
            x += decal;
            objects.push_back(new Wall(Vector3D(x + 10.f - decal, m_pos.y(), m_pos.z() - 80.f), Rotation(), BLUE, 150.f, 130.f));
        }
#ifdef WIN32
        objects.push_back(new Cylinder(Vector3D(x, m_pos.y(), m_pos.z() - 80.f), Rotation(-90.f, 0.f, 0.f, 1.f), "./textures/box.jpg", 20.f, 130.f));
        objects.push_back(new Cylinder(Vector3D(x, m_pos.y(), m_pos.z() + 80.f), Rotation(-90.f, 0.f, 0.f, 1.f), "./textures/box.jpg", 20.f, 130.f));
#else
        objects.push_back(new Cylinder(Vector3D(x, m_pos.y(), m_pos.z() - 80.f), Rotation(-90.f, 0.f, 0.f, 1.f), "/windows/Users/imperio/Projects/My_World_SDL/My_World_SDL/My_World_SDL/textures/box.jpg", 20.f, 130.f));
        objects.push_back(new Cylinder(Vector3D(x, m_pos.y(), m_pos.z() + 80.f), Rotation(-90.f, 0.f, 0.f, 1.f), "/windows/Users/imperio/Projects/My_World_SDL/My_World_SDL/My_World_SDL/textures/box.jpg", 20.f, 130.f));
#endif*/

        v = dir.getFileList();
        pos = 1;
        tmp = sqrtf(v.size()) + 1;
        float spacing = -5.f;
        /*float */tmp_x = m_pos.x() - spacing * (tmp - 1.f) / 2;
        float x = tmp_x;
        float z = m_pos.z() + spacing;

#ifdef WIN32
        objects.push_back(new Plane(Vector3D(m_pos.x(), m_pos.y(), m_pos.z() - (my_abs(spacing) * tmp) / 2.f), Rotation(0.f, 0.f, 90.f),
                                    "./textures/metal.jpg", tmp * my_abs(spacing), my_abs(spacing) * tmp));
#else
        objects.push_back(new Plane(Vector3D(m_pos.x(), m_pos.y(), m_pos.z() - (my_abs(spacing) * tmp) / 2.f), Rotation(0.f, 0.f, 90.f),
                                    "textures/metal.jpg", (tmp + 1.f) * my_abs(spacing), my_abs(spacing) * tmp));
#endif

        for (vector<string>::iterator it = v.begin(); it != v.end(); ++it) {
            GraphicFile  *t = new GraphicFile(Vector3D(x, m_pos.y() + 2.f, z), Rotation(), Color(), it->c_str());

            if (!t)
                continue;
            x += spacing;
            if (pos == tmp) {
                z += spacing;
                pos = 0;
                x = tmp_x;
            }
            pos++;
            files.push_back(t);
        }
        save_pos.resize(files.size());
    } catch (MyException &e){

    }
    m_className = "Room";
}

Room::~Room()
{
    for (std::vector<myGLWidget*>::iterator it = objects.begin(); it != objects.end(); ++it){
        delete (*it);
    }
    objects.clear();
    for (std::vector<myGLWidget*>::iterator it = dirs.begin(); it != dirs.end(); ++it){
        delete (*it);
    }
    dirs.clear();
    for (std::vector<myGLWidget*>::iterator it = files.begin(); it != files.end(); ++it){
        delete (*it);
    }
    files.clear();
}

void  Room::initializeGL()
{
    for (std::vector<myGLWidget*>::iterator it = files.begin(); it != files.end(); ++it){
        (*it)->initializeGL();
    }
    for (std::vector<myGLWidget*>::iterator it = objects.begin(); it != objects.end(); ++it){
        (*it)->initializeGL();
    }
}

void  Room::paintGL(const glm::mat4& view_matrix, const glm::mat4& proj_matrix)
{
    /*Vector3D  v(getMainWindow()->getPlayerPosition());

    if (v != old_pos){

        for (size_t it = 0; it < files.size(); ++it){
            Vector3D  tmp(files[it]->getPosition());

            tmp.setX(tmp.x() * -1.f);
            tmp.setZ(tmp.z() * -1.f);
            save_pos[it] = abs(tmp.getDistance(v));
        }
        if (files.size()) {
            for (int it = 0; it < (int)files.size() - 1; ++it){
                if (save_pos[it] < save_pos[it + 1]){
                    float tmp = save_pos[it];
                    save_pos[it] = save_pos[it + 1];
                    save_pos[it + 1] = tmp;
                    myGLWidget *t = files[it];
                    files[it] = files[it + 1];
                    files[it + 1] = t;

                    it -= 2;
                    if (it < -1)
                        it = -1;
                }
            }
            old_pos = v;
            GraphicFile *t = static_cast<GraphicFile*>(files[files.size() - 1]);

            t->getPikingColor();
        }
    }*/
    for (std::vector<myGLWidget*>::iterator it = files.begin(); it != files.end(); ++it){
        (*it)->paintGL(view_matrix, proj_matrix);
    }
    for (std::vector<myGLWidget*>::iterator it = objects.begin(); it != objects.end(); ++it){
        (*it)->paintGL(view_matrix, proj_matrix);
    }
}

unsigned int  Room::nbObjects() const
{
    return files.size() + 1;
}

void  Room::pick(const glm::mat4& view_matrix, const glm::mat4& proj_matrix)
{
    for (std::vector<myGLWidget*>::iterator it = files.begin(); it != files.end(); ++it){
        (*it)->setSelected(false);
        (*it)->pick(view_matrix, proj_matrix);
    }
}

bool  Room::hasBeenPicked(int c)
{
    for (std::vector<myGLWidget*>::iterator it = files.begin(); it != files.end(); ++it){
        if (!c){
            (*it)->setSelected(true);
            return true;
        }
        --c;
    }
    return false;
}

int Room::objectSize()
{
    return files.size();
}


void    Room::setMainWindow(MyWindow *w)
{
    myGLWidget::setMainWindow(w);
    for (std::vector<myGLWidget*>::iterator it = objects.begin(); it != objects.end(); ++it){
        (*it)->setMainWindow(w);
    }
    for (std::vector<myGLWidget*>::iterator it = dirs.begin(); it != dirs.end(); ++it){
        (*it)->setMainWindow(w);
    }
    for (std::vector<myGLWidget*>::iterator it = files.begin(); it != files.end(); ++it){
        (*it)->setMainWindow(w);
    }
}
