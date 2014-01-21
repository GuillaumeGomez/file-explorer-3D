#ifndef LOADINGMENU_HPP
#define LOADINGMENU_HPP

#include <string>
#include <GL/glew.h>
#include "Texture.hpp"

class MyWindow;
class GraphicHandler;
class myGLWidget;

class LoadingMenu
{
public:
    LoadingMenu(int nbObject, MyWindow*);
    virtual ~LoadingMenu();
    void    newLoadedObject();
    void    createTextTexture(const char*, int i);
    void    draw();

private:
    MyWindow    *m_win;
    myGLWidget  *m_widget1;
    myGLWidget  *m_widget2;
    myGLWidget  *m_widget3;
    int         pourcent;
    int         obj;
    int         nbObject;
    const float m_timer;
    Texture     m_texture[101];
    std::string m_text;
};

#endif // LOADINGMENU_HPP
