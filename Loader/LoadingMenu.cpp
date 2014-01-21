#include "LoadingMenu.hpp"
#include "../String_utils.hpp"
#include "../MyWindow.hpp"
#include "../HandleSDL.hpp"
#include "../objects/Plane.hpp"
#include "../Camera.hpp"
#include "../objects/Text.hpp"
#include <iostream>

using namespace std;

LoadingMenu::LoadingMenu(int nb, MyWindow *m) : m_win(m), pourcent(0), obj(0), nbObject(nb), m_timer(1.f / 30.f)
{
  m_text = "0 %";

  for (int i = 0; i <= 100; ++i)
    {
      m_text = Utility::toString<int>(i) + " %";
      MyWindow::getLib()->createTextTexture(m_text.c_str(), &m_texture[i], Color(1.f, 1.f, 1.f));
    }
  m_widget1 = new Object::Plane(Vector3D(0.f, -0.85f), Rotation(), RED, 2.f, 0.07f);
  m_widget2 = new Object::Plane(Vector3D(0.f, -0.85f), Rotation(), WHITE, 2.f, 0.07f);
  m_widget3 = new Object::Text(m_texture[0], 0.06f * 3.f, 1, Vector3D(-0.09f, -0.7f), Rotation(), 0.12f);

  m_widget3->setRender2D(true);
  m_widget1->initializeGL();
  m_widget2->initializeGL();
  m_widget3->initializeGL();
}

LoadingMenu::~LoadingMenu()
{
  m_widget3->setTexture(0);
  delete m_widget1;
  delete m_widget2;
  delete m_widget3;
}

void  LoadingMenu::newLoadedObject()
{
  int       tmp;
  SDL_Event e;

  ++obj;
  tmp = obj * 100 / nbObject;
  if (tmp != pourcent || m_win->getLib()->getElapsedTime() >= m_timer) {
      pourcent = tmp;
      this->draw();
    }
  while (m_win->getLib()->getEvent(&e)) {
      switch (e.type) {
        case SDL_QUIT:
          exit(10);
        case SDL_KEYDOWN:
          if (e.key.keysym.sym == SDLK_ESCAPE ||
              (e.key.keysym.sym == SDLK_F4 && (e.key.keysym.mod == KMOD_LALT || e.key.keysym.mod == KMOD_RALT)))
            exit(10);
        }
    }
}

void  LoadingMenu::draw()
{
  m_win->clearScreen();

  m_widget2->setPosition(Vector3D(pourcent / 50.f, -0.85f));
  m_widget3->setTexture(m_texture[pourcent]);

  m_widget3->paintGL(Camera::getViewMatrix(), Camera::get2DProjectionMatrix());

  m_widget2->paintGL(Camera::getViewMatrix(), Camera::get2DProjectionMatrix());
  m_widget1->paintGL(Camera::getViewMatrix(), Camera::get2DProjectionMatrix());

  m_win->getLib()->updateScreen();
}
