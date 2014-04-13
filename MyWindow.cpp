#include "MyWindow.hpp"
#include "myGLWidget.hpp"
#include "Camera.hpp"
#include "KeyHandler.hpp"
#include "Utils/HandleThread.hpp"
#include "objects/Wall.hpp"
#include "objects/Button.hpp"
#include "objects/Rectangle.hpp"
#include "objects/GraphicFile.hpp"
#include "objects/Cube.hpp"
#include "objects/Model.hpp"
#include "FrameBuffer.hpp"
#include "HandlePhysics.hpp"

#include "Loader/Loader.hpp"

#include "HandleError.hpp"
#include "Utils/Utils.hpp"
#include "objects/Text.hpp"
#include "HandleFpsCount.hpp"
#include "String_utils.hpp"
#include "Utils/MyException.hpp"
#include "Tetris.hpp"
#include "Handle_2048.hpp"

#include <iostream>

using namespace std;

HandleSDL  *MyWindow::sdl = 0;

void  *repeat_func(void *arg)
{
  MyWindow  *win(static_cast<MyWindow*>(arg));

  win->repeatKey();
  return arg;
}

MyWindow::MyWindow(std::string winName, int antiali, int fps)
  : m_printInfo(false), MIN(WTIMER / (fps <= 0 ? 40 : fps)), pause(false), m_wireframe(false),
    m_mode(MODE_NORMAL), m_physics(0), m_character(0)
{
  srand(time(0));
  m_camera = 0;
  m_key = 0;
  m_thread = 0;
  m_fps = 0;
  m_fbo = 0;
  m_tetris = 0;
  m_disp = 0;
  m_2048 = 0;
  (void)antiali;
  try {
    if (!sdl)
      sdl = new WINDOW_HANDLER(winName, this, 2);

    /* initialisation de glew */
    GLenum err;

    if ((err = glewInit())){
        HandleError::showError(string("glewInit() failed: ") + (const char*)glewGetErrorString(err));
        delete sdl;
        exit(-1);
      }

    // hiding cursor
    //SDL_ShowCursor(SDL_DISABLE);

    m_camera = new Camera;

    m_key = new KeyHandler;
    m_thread = new HandleThread(repeat_func, static_cast<void*>(this));
    m_fps = new HandleFpsCount;

    m_fbo = new FrameBuffer(sdl->width(), sdl->height());
    m_fbo->load();
    for (unsigned int i(0); i < 5; ++i){
        m_displayList[i] = new Object::Text("", BLUE, -0.98f, 0.88f - i * 0.12f, 0.12f);
        m_displayList[i]->initializeGL();
      }
    m_tetris = new Tetris;
    m_2048 = new Handle_2048;
    m_physics = new HandlePhysics;
  } catch (std::bad_alloc &err) {
    HandleError::showError(err.what());
    throw MyException("Bad alloc");
  }
}

MyWindow::~MyWindow()
{
  if (this->m_thread)
    delete this->m_thread;
  if (this->m_physics)
    delete this->m_physics;
  if (this->m_fps)
    delete this->m_fps;
  if (this->m_camera)
    delete this->m_camera;
  if (m_tetris)
    delete m_tetris;
  if (m_2048)
    delete m_2048;
  if (this->m_disp)
    delete m_disp;
  if (m_fbo)
    delete this->m_fbo;
  for (unsigned int i(0); i < 5; ++i){
      delete m_displayList[i];
    }
  for (auto tmp : objectList)
    delete tmp;
  for (auto tmp : _2D_objectList)
    delete tmp;
  for (auto tmp : m_pauseObjectList)
    delete tmp;
  objectList.clear();
  _2D_objectList.clear();
  m_pauseObjectList.clear();
  if (this->m_key)
    delete this->m_key;
  if (this->sdl)
    delete this->sdl;
}

void  MyWindow::repeatKey()
{
  int     *k(m_key->getKeys());

  while (1)
    {
      m_key->lock();
      int s(m_key->getNbKeys() - 1);

      if (m_mode == MODE_NORMAL) {
          int lat(0);
          int front(0);

          while (s >= 0){
              int tmp = k[s--];

              if (tmp == SDLK_a || tmp == SDLK_d)
                ++lat;
              else if (tmp == SDLK_w || tmp == SDLK_s)
                ++front;
            }
          if (front == 1 && lat == 1)
            m_camera->setSpeed(m_camera->speed() * 0.75f);
          s = m_key->getNbKeys() - 1;
          while (s >= 0)
            m_camera->keyPressEvent(k[s--]);
          if (front == 1 && lat == 1)
            m_camera->setSpeed(m_camera->speed() / 0.75f);
        }
      m_key->unlock();
      Utils::sleep(m_key->getInterval());
    }
}

void    MyWindow::clearScreen()
{
  if (m_mode != MODE_2048 && m_mode != MODE_TETRIS)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  else
    glClear(GL_DEPTH_BUFFER_BIT);
}

void  MyWindow::mousePressEvent(int but, int posx, int posy)
{
  (void)posx;
  (void)posy;
  if (but != SDL_BUTTON_LEFT)
    return;
  /*if (!Utils::playSound("./sounds/gun.wav"))
    HandleError::showError("Problem with playing file");*/
}

void  MyWindow::mouseReleaseEvent(int but, int posx, int posy)
{
  (void)posx;
  (void)posy;
  (void)but;
  /*if (but != SDL_BUTTON_LEFT)
    return;*/
}

void  MyWindow::keyReleaseEvent(int ev)
{
  m_key->lock();
  m_key->releaseKey(ev);
  m_key->unlock();
  switch (m_mode) {
    case MODE_TETRIS:
      m_tetris->keyReleaseEvent(ev);
      break;
    case MODE_NORMAL:
      m_camera->keyReleaseEvent(ev);
      break;
    case MODE_2048:
      m_2048->keyReleaseEvent(ev);
      break;
    }
}

void MyWindow::keyPressEvent(int key)
{
  switch (key) {
    case SDLK_F11:
      sdl->switchScreenMode();
      break;
    case SDLK_F12:
      this->takeScreenshot();
      break;
    default:
      if (m_mode == MODE_TETRIS) {
          switch (key)
            {
            case SDLK_TAB:
              glEnable(GL_DEPTH_TEST);
              this->m_mode = MODE_NORMAL;
              m_key->lock();
              m_key->setInterval(10);
              m_key->unlock();
              sdl->setFPSMode(true);
              //sdl->resetCursor();
              break;
              /*case SDLK_UP:
        case SDLK_SPACE:
          m_tetris->keyPressEvent(key);
          break;*/
            default:
              m_tetris->keyPressEvent(key);
              //m_key->lock();
              //m_key->addKey(key);
              //m_key->unlock();
            }
        } else if (m_mode == MODE_2048) {
          if (key == SDLK_TAB) {
              glEnable(GL_DEPTH_TEST);
              this->m_mode = MODE_NORMAL;
              sdl->setFPSMode(true);
              //sdl->resetCursor();
              //sdl->displayCursor(pause);
              /*m_key->lock();
              m_key->setInterval(10);
              m_key->unlock();*/
            }
        } else if (m_mode == MODE_NORMAL) {
          switch(key)
            {
            case SDLK_RETURN:
              this->m_printInfo = !this->m_printInfo;
              break;
            case SDLK_TAB:
              glDisable(GL_DEPTH_TEST);
              this->m_mode = MODE_TETRIS;
              sdl->setFPSMode(false);
              sdl->displayCursor(false);
              /*m_key->lock();
              m_key->setInterval(50);
              m_key->unlock();*/
              break;
            case SDLK_BACKSPACE:
              m_wireframe = !m_wireframe;
              glPolygonMode(GL_FRONT_AND_BACK, m_wireframe ? GL_LINE : GL_FILL);
              break;
            case SDLK_ESCAPE:
              sdl->setFPSMode(pause);
              pause = !pause;
              sdl->displayCursor(pause);
              break;
            case SDLK_0:
              glDisable(GL_DEPTH_TEST);
              this->m_mode = MODE_2048;
              sdl->setFPSMode(false);
              sdl->displayCursor(false);
              //sdl->displayCursor(true);
              break;
            default:
              m_key->lock();
              m_key->addKey(key);
              m_key->unlock();
            }
        }
    }
}

void MyWindow::resizeGL(int width, int height)
{
  m_camera->resize(width, height);
  m_fbo->resize(width, height);
  m_disp->setTexture(m_fbo->getTextureID(0));
}

void MyWindow::initializeGL()
{
  Loader m_loader(this);

  glEnable(GL_DEPTH_TEST);
  m_loader.loadDatas();

  /*myGLWidget *tmp = new Object::Button("Test", RED, BLUE, Vector3D(-0.25f, -0.1f), 0.5f, 0.2f);

  tmp->initializeGL();
  this->addObject(tmp, true);*/
  //tmp = new Object::GraphicFile(Vector3D(0.f, 2.f, -6.f), Rotation(), GREEN, "./GraphicHandler.cpp");
  //tmp->setPickingAllowed(true);
  //tmp->initializeGL();
  //this->addObject(tmp);

  sdl->displayCursor(false);
  m_disp = new Object::Cube(Vector3D(-3.f, 2.f, -6.f), Rotation(), "", 2.f);
  m_disp->setTexture(m_fbo->getTextureID(0));
  m_disp->initializeGL();

  m_tetris->initializeGL();
  m_2048->initializeGL();
}

void  MyWindow::update()
{
  //glEnable(GL_COLOR_MATERIAL);

  this->clearScreen();

  m_camera->update();

  float tmp = sdl->getElapsedTime();
  float fps = this->m_fps->getFpsCount();
  if (tmp != 0.f) {
      switch (m_mode) {
      case MODE_TETRIS:
          m_tetris->update(tmp);
          break;
        case MODE_NORMAL:
          if (!pause) {
              if (m_character)
                m_character->update(tmp);
              m_physics->update(tmp);
              for (WinList::iterator it = objectList.begin(); it != objectList.end(); ++it)
                (*it)->update(tmp);
            }
          for (WinList::iterator it = _2D_objectList.begin(); it != _2D_objectList.end(); ++it)
            (*it)->update(tmp);

          if (this->m_printInfo) {
              static_cast<Object::Text*>(m_displayList[0])->setText("x : " + Utility::toString<float>(m_camera->getPosition().x()));
              static_cast<Object::Text*>(m_displayList[1])->setText("y : " + Utility::toString<float>(m_camera->getPosition().y()));
              static_cast<Object::Text*>(m_displayList[2])->setText("z : " + Utility::toString<float>(m_camera->getPosition().z()));
              static_cast<Object::Text*>(m_displayList[3])->setText("fps : " + Utility::toString<float>(fps));
            }

          m_camera->look();
          picking();
          break;
        case MODE_2048:
          m_2048->update(tmp);
          break;
        }
    }

  //glDisable(GL_COLOR_MATERIAL);
}

void MyWindow::paintGL()
{
  const glm::mat4 view_mat = Camera::getViewMatrix();
  const glm::mat4 proj_mat = Camera::getProjectionMatrix();

  const glm::mat4 proj2d_mat = Camera::get2DProjectionMatrix();

  switch (m_mode) {
    case MODE_NORMAL:
      /* 3D part */
      glEnable(GL_TEXTURE_2D);

      for (WinList::iterator it = objectList.begin(); it != objectList.end(); ++it){
          (*it)->paintGL(view_mat, proj_mat);
        }
      m_character->paintGL(view_mat, proj_mat);
      m_disp->paintGL(view_mat, proj_mat);

      /* 2D part */
      glDepthMask(GL_FALSE);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      if (this->m_printInfo) {
          m_displayList[0]->paintGL(view_mat, proj2d_mat);
          m_displayList[1]->paintGL(view_mat, proj2d_mat);
          m_displayList[2]->paintGL(view_mat, proj2d_mat);
          m_displayList[3]->paintGL(view_mat, proj2d_mat);
        }
      if (display_sentence != ""){
          m_displayList[4]->setPosition(Vector3D());
          static_cast<Object::Text*>(m_displayList[4])->setText(display_sentence);
          m_displayList[4]->paintGL(view_mat, proj2d_mat);
        }

      for (WinList::iterator it = _2D_objectList.begin(); it != _2D_objectList.end(); ++it){
          (*it)->paintGL(view_mat, proj2d_mat);
        }

      if (pause)
        for (auto it = m_pauseObjectList.begin(); it != m_pauseObjectList.end(); ++it)
          (*it)->paintGL(view_mat, proj2d_mat);

      //display_sentence = "";

      glDisable(GL_BLEND);
      glDisable(GL_TEXTURE_2D);
      glDepthMask(GL_TRUE);
      break;
    case MODE_TETRIS:
      glDepthMask(GL_FALSE);
      m_tetris->paintGL(Camera::getViewMatrix(), Camera::getProjectionMatrix());
      glDepthMask(GL_TRUE);
      break;
    case MODE_2048:
      glDepthMask(GL_FALSE);
      m_2048->paintGL(Camera::getViewMatrix(), Camera::getProjectionMatrix());
      glDepthMask(GL_TRUE);
      break;
    }
}

void  MyWindow::paintGL2(const glm::mat4 &view, const glm::mat4 &pers)
{
  for (WinList::iterator it = m_pickObjects.begin(); it != m_pickObjects.end(); ++it){
      (*it)->pick(view, pers);
      //(*it)->paintGL(view, pers);
    }
}

void  MyWindow::addObject(myGLWidget *s, bool isPauseObject)
{
  if (!s)
    return;
  s->setMainWindow(this);
  if (isPauseObject) {
      m_pauseObjectList.push_back(s);
    }
  else if (!s->is2D()) {
      objectList.push_back(s);
      m_physics->addObject(s);
    }
  else {
      _2D_objectList.push_back(s);
    }
  if (s->isPickingAllowed())
    m_pickObjects.push_back(s);
}

bool  MyWindow::isPlaying() const
{
  return m_mode != MODE_NORMAL;
}

void  MyWindow::mouseMoveEvent(int x, int y)
{
  if (!pause && m_mode == MODE_NORMAL) {
      m_camera->mouseMoveEvent(x, y);
      //sdl->resetCursor();
    }
  mouseX = x;
  mouseY = y;
}

void  MyWindow::setPause(bool b)
{
  pause = b;
}

bool    MyWindow::isPaused() const
{
  return pause;
}

void  MyWindow::start()
{
  m_end = false;

  this->initializeGL();
  m_camera->setView(this);

  this->m_thread->start();
  while (!m_end)
    {
      m_end = sdl->handleEvents();
      this->update();
      this->paintGL();
      sdl->updateScreen();
    }
}

HandleSDL *MyWindow::getLib()
{
  return sdl;
}

void  MyWindow::picking()
{
  /*  glm::mat4 pickMat = Camera::getViewMatrix();
  glm::mat4 perspectiveMat = glm::perspective(70.f, Camera::getRatio(), 0.1f, 20.f);

  m_fbo->bind();
  paintGL2(pickMat, perspectiveMat);
  unsigned char pixel[3];
  glReadPixels(mouseX, sdl->height() - mouseY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
  m_fbo->unbind();
  m_disp->setTexture(m_fbo->getTextureID(0));

  Color comp(pixel[0] / 255.f, pixel[1] / 255.f, pixel[2] / 255.f);

  for (WinList::iterator it = m_pickObjects.begin(); it != m_pickObjects.end(); ++it){
      (*it)->setSelected(false);
      if ((*it)->getPrimaryColor() == comp) {
          (*it)->setSelected(true);
          break;
        }
    }*/
  static myGLWidget *w(0);

  myGLWidget *tmp = m_physics->pick(sdl->width() / 2, sdl->height() / 2, sdl->width(), sdl->height());
  if (w != tmp && w) {
        w->setSelected(false);
    }
  if (!tmp)
    return;
  tmp->setSelected(true);
  w = tmp;
  /*if (!w->isSelected()) {
      for (auto it = objectList.begin(); it != objectList.end(); ++it){
          (*it)->setSelected(false);
          /*if ((*it) == w) {
          (*it)->setSelected(true);
          break;
          }*/
        /*}
      w->setSelected(true);
    }*/
}

void  MyWindow::setDisplaySentence(std::string s)
{
  if (s != display_sentence)
    display_sentence = s;
}

void  MyWindow::setDisplaySentence(const char *s)
{
  if (!s || display_sentence == s)
    return;
  display_sentence = s;
}

Vector3D const  &MyWindow::getPlayerPosition() const
{
  return m_camera->getPosition();
}

void  MyWindow::setMainCharacter(myGLWidget *w)
{
  if (!w)
    return;
  m_character = static_cast<Object::Model*>(w);
  m_character->cutAnimation("", "walk", 0.f, 393.f);
  m_character->setCurrentAnimation("walk");
  m_character->play();
}

void  MyWindow::takeScreenshot(std::string filename)
{
  m_fbo->bind();
  this->clearScreen();
  m_camera->look();
  this->paintGL();
  m_fbo->unbind();
  if (filename.empty()) {
      char text[100] = "";
      time_t now = time(0);
      struct tm *t = localtime(&now);

      strftime(text, 99, "%H-%M-%S %d-%m-%Y", t);
      filename = text;
      filename += ".png";
    }
  Texture tmp;

  tmp.setTextureID(m_fbo->getTextureID(0));
  if (sdl->saveImage(&tmp, filename))
    std::cout << filename << " saved" << std::endl;
  else
    std::cout << filename << " not saved" << std::endl;
}
