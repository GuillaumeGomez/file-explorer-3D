#include "MyWindow.hpp"
#include "myGLWidget.hpp"
#include "Camera.hpp"
#include "KeyHandler.hpp"
#include "GraphicHandler.hpp"
#include "Utils/HandleThread.hpp"
#include "objects/Wall.hpp"
#include "objects/Button.hpp"
#include "objects/Rectangle.hpp"
#include "objects/GraphicFile.hpp"
#include "objects/Cube.hpp"
#include "FrameBuffer.hpp"

#include "Loader/Loader.hpp"

#include "HandleError.hpp"
#include "Utils/Utils.hpp"
#include "objects/Text.hpp"
#include "HandleFpsCount.hpp"
#include "String_utils.hpp"
#include "Utils/MyException.hpp"
#include "Tetris.hpp"

#include <iostream>

#ifdef WIN32
#include "HandleSDL.hpp"
#else
#include "HandleSFML.hpp"
#endif

using namespace std;

GraphicHandler  *MyWindow::sdl = 0;

void  *repeat_func(void *arg)
{
  MyWindow  *win(static_cast<MyWindow*>(arg));

  win->repeatKey();
  return arg;
}

MyWindow::MyWindow(std::string winName, int antiali, int fps)
  : m_printInfo(false), MIN(WTIMER / (fps <= 0 ? 40 : fps)), pause(false), m_test(false), m_wireframe(false),
    m_tetrisMode(false)
{
  m_camera = 0;
  m_key = 0;
  m_thread = 0;
  m_fps = 0;
  m_fbo = 0;
  m_tetris = 0;
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

    // on cache le curseur
    //SDL_ShowCursor(SDL_DISABLE);
    // curseur bloque dans la fenetre
    //SDL_WM_GrabInput(SDL_GRAB_ON);

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
  } catch (std::bad_alloc &err) {
    HandleError::showError(err.what());
    throw MyException("Bad alloc");
  }
}

MyWindow::~MyWindow()
{
  if (this->m_thread)
    delete this->m_thread;
  if (this->m_fps)
    delete this->m_fps;
  if (this->m_camera)
    delete this->m_camera;
  if (m_fbo)
    delete this->m_fbo;
  for (unsigned int i(0); i < 5; ++i){
      delete m_displayList[i];
    }
  while (objectList.size() > 0){
      delete (*objectList.begin());
      objectList.erase(objectList.begin());
    }
  while (_2D_objectList.size() > 0){
      delete (*_2D_objectList.begin());
      _2D_objectList.erase(_2D_objectList.begin());
    }
  if (this->m_key)
    delete this->m_key;
  if (this->sdl)
    delete this->sdl;
  this->sdl = 0;
}

void  MyWindow::repeatKey()
{
  int     *k(m_key->getKeys());

  while (1)
    {
      m_key->lock();
      int s(m_key->getNbKeys() - 1);

      if (!m_tetrisMode) {
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
            m_camera->setSpeed(m_camera->getSpeed() * 0.75f);
          s = m_key->getNbKeys() - 1;
          while (s >= 0)
            m_camera->keyPressEvent(k[s--]);
          if (front == 1 && lat == 1)
            m_camera->setSpeed(m_camera->getSpeed() / 0.75f);
        } else {
          while (s >= 0)
            m_tetris->keyPressEvent(k[s--]);
        }
      m_key->unlock();
      Utils::sleep(m_key->getInterval());
    }
}

void  MyWindow::update()
{
  //glEnable(GL_DEPTH_TEST);
  //glEnable(GL_COLOR_MATERIAL);

  if (!m_tetrisMode)
    picking();
  //glEnable(GL_TEXTURE_2D);
  float tmp = sdl->getElapsedTime();
  if (tmp != 0.f) {
      if (m_tetrisMode) {
          m_tetris->update(tmp);
        } else {
          if (!pause)
            for (WinList::iterator it = objectList.begin(); it != objectList.end(); ++it)
              (*it)->update(tmp);
          for (WinList::iterator it = _2D_objectList.begin(); it != _2D_objectList.end(); ++it)
            (*it)->update(tmp);
        }
    }

  this->clearScreen();
  //glColor3f(0.f, 0.f, 0.f);

  if (m_tetrisMode) {
      glDepthMask(GL_FALSE);
      m_tetris->paintGL(Camera::getViewMatrix(), Camera::getProjectionMatrix());
      glDepthMask(GL_TRUE);
    } else {
      m_camera->look();
      this->paintGL();
    }
  //glDisable(GL_TEXTURE_2D);
  //glDisable(GL_COLOR_MATERIAL);
}

void    MyWindow::clearScreen()
{
  //glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void  MyWindow::mousePressEvent(int but, int posx, int posy)
{
  (void)posx;
  (void)posy;
  if (but != SDL_BUTTON_LEFT)
    return;
  if (!Utils::playSound("./sounds/gun.wav"))
    HandleError::showError("Problem with playing file");
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
  if (m_tetrisMode)
    m_tetris->keyReleaseEvent(ev);
  else
    m_camera->keyReleaseEvent(ev);
}

void MyWindow::keyPressEvent(int key)
{
  if (!m_tetrisMode) {
      switch(key)
        {
        case SDLK_F11:
          sdl->switchScreenMode();
          break;
        case SDLK_RETURN:
          this->m_printInfo = !this->m_printInfo;
          break;
        case SDLK_TAB:
          //this->m_test = !this->m_test;
          this->m_tetrisMode = !this->m_tetrisMode;
          m_key->lock();
          m_key->setInterval(100);
          m_key->unlock();
          break;
        case SDLK_BACKSPACE:
          m_wireframe = !m_wireframe;
          glPolygonMode(GL_FRONT_AND_BACK, m_wireframe ? GL_LINE : GL_FILL);
          break;
        case SDLK_ESCAPE:
          pause = !pause;
          break;
        default:
          m_key->lock();
          m_key->addKey(key);
          m_key->unlock();
        }
    } else {
      switch(key)
        {
        case SDLK_F11:
          sdl->switchScreenMode();
          break;
        /*case SDLK_RETURN:
          this->m_printInfo = !this->m_printInfo;*/
          break;
        case SDLK_TAB:
          this->m_tetrisMode = !this->m_tetrisMode;
          m_key->lock();
          m_key->setInterval(10);
          m_key->unlock();
          break;
        case SDLK_BACKSPACE:
          m_wireframe = !m_wireframe;
          glPolygonMode(GL_FRONT_AND_BACK, m_wireframe ? GL_LINE : GL_FILL);
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
    }
}

void MyWindow::resizeGL(int width, int height)
{
  m_camera->resize(width, height);
}

void MyWindow::initializeGL()
{
  Loader m_loader(this);

  glEnable(GL_DEPTH_TEST);
  m_loader.loadDatas();

  myGLWidget *tmp = new Object::Button("Test", RED, BLUE, Vector3D(-0.25f, -0.1f), 0.5f, 0.2f);

  tmp->initializeGL();
  this->addObject(tmp, true);
  tmp = new Object::GraphicFile(Vector3D(0.f, 2.f, -6.f), Rotation(), GREEN, "./GraphicHandler.cpp");
  tmp->setMainWindow(this);
  tmp->setPickingAllowed(true);
  tmp->initializeGL();
  this->addObject(tmp);

  m_disp = new Object::Cube(Vector3D(-3.f, 2.f, -6.f), Rotation(), "", 2.f);
  m_disp->setTexture(1);
  m_disp->initializeGL();

  m_tetris->initializeGL();
}

void MyWindow::paintGL()
{
  float fps;

  fps = this->m_fps->getFpsCount();
  display_sentence = "";

  /* 3D part */
  const glm::mat4 view_mat = Camera::getViewMatrix();
  const glm::mat4 proj_mat = Camera::getProjectionMatrix();
  glEnable(GL_TEXTURE_2D);

  for (WinList::iterator it = objectList.begin(); it != objectList.end(); ++it){
      (*it)->paintGL(view_mat, proj_mat);
    }

  m_disp->paintGL(view_mat, proj_mat);

  /* 2D part */
  const glm::mat4 proj2d_mat = Camera::get2DProjectionMatrix();

  glDepthMask(GL_FALSE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  if (this->m_printInfo) {
      static_cast<Object::Text*>(m_displayList[0])->setText("x : " + Utility::toString<float>(m_camera->getPosition().x()));
      m_displayList[0]->paintGL(view_mat, proj2d_mat);
      static_cast<Object::Text*>(m_displayList[1])->setText("y : " + Utility::toString<float>(m_camera->getPosition().y()));
      m_displayList[1]->paintGL(view_mat, proj2d_mat);
      static_cast<Object::Text*>(m_displayList[2])->setText("z : " + Utility::toString<float>(m_camera->getPosition().z()));
      m_displayList[2]->paintGL(view_mat, proj2d_mat);
      static_cast<Object::Text*>(m_displayList[3])->setText("fps : " + Utility::toString<float>(fps));
      m_displayList[3]->paintGL(view_mat, proj2d_mat);
    }
  if (display_sentence != ""){
      static_cast<Object::Text*>(m_displayList[4])->setText(display_sentence);
      m_displayList[4]->paintGL(view_mat, proj2d_mat);
    }

  if (pause)
    for (WinList::iterator it = _2D_objectList.begin(); it != _2D_objectList.end(); ++it){
        (*it)->paintGL(view_mat, proj2d_mat);
      }

  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  glDepthMask(GL_TRUE);
}

void  MyWindow::paintGL2(const glm::mat4 &view, const glm::mat4 &pers)
{
  int loop(0);

  //const glm::mat4 view_mat = Camera::getViewMatrix();
  //const glm::mat4 proj_mat = Camera::getProjectionMatrix();

  for (WinList::iterator it = m_pickObjects.begin(); it != m_pickObjects.end(); ++it){
      (*it)->pick(loop, view, pers);
      //(*it)->paintGL(view, pers);
    }
}

void  MyWindow::addObject(myGLWidget *s, bool is2D)
{
  if (!s)
    return;
  s->setMainWindow(this);
  if (!is2D)
    objectList.push_back(s);
  else {
      _2D_objectList.push_back(s);
    }
  if (s->isPickingAllowed())
    m_pickObjects.push_back(s);
}

bool  MyWindow::isPlayingTetris() const
{
  return m_tetrisMode;
}

void  MyWindow::mouseMoveEvent(int x, int y)
{
  m_camera->mouseMoveEvent(x, y);
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
      sdl->updateScreen();
    }
}

GraphicHandler *MyWindow::getLib()
{
  return sdl;
}

void  MyWindow::picking()
{
  //GLuint  buffer[512];
  //GLint   hits;
  //GLint   viewport[4];
  glm::vec4 viewport;

  //glGetIntegerv(GL_VIEWPORT, viewport);
  //glSelectBuffer(512, buffer);
  //glRenderMode(GL_SELECT);

  //glInitNames();
  //glPushName(0);

  for (WinList::iterator it = objectList.begin(); it != objectList.end(); ++it){
      (*it)->setSelected(false);
    }

  /*glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();*/

  //glm::mat4 pickMat = glm::pickMatrix(glm::vec2(m_camera->mouseX(), (GLdouble)(viewport[3] - m_camera->mouseY())), glm::vec2(1.f, 1.f), viewport);
  glm::mat4 pickMat = Camera::getViewMatrix();

  //gluPickMatrix((GLdouble)m_camera->mouseX(), (GLdouble)(viewport[3] - m_camera->mouseY()), 1.0f, 1.0f, viewport);
  glm::mat4 perspectiveMat = glm::perspective(70.f, 1.f, 0.1f, 75.f);
  //gluPerspective(70.0f, (GLfloat) (viewport[2]-viewport[0])/(GLfloat) (viewport[3]-viewport[1]), 0.1f, 75.f);
  //glMatrixMode(GL_MODELVIEW);
  //glLoadIdentity();
  //m_camera->lookAt();
  //DrawTargets();

  m_fbo->bind();
  glEnable(GL_TEXTURE_2D);
  paintGL2(pickMat, perspectiveMat);
  glDisable(GL_TEXTURE_2D);
  unsigned char pixel[3];
  glReadPixels(0, 0, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
  m_fbo->unbind();
  m_disp->setTexture(m_fbo->getTextureID(0));

  Color comp(pixel[0] / 255.f, pixel[1] / 255.f, pixel[2] / 255.f);

  for (WinList::iterator it = m_pickObjects.begin(); it != m_pickObjects.end(); ++it){
      if ((*it)->getPrimaryColor() == comp) {
          (*it)->setSelected(true);
          break;
        }
    }
  //glMatrixMode(GL_PROJECTION);
  //glPopMatrix();
  //glMatrixMode(GL_MODELVIEW);

  /*if ((hits = glRenderMode(GL_RENDER)) > 0){
      int choose = buffer[3];
      int depth = buffer[1];

      for (int loop = 1; loop < hits; loop++)
        {
          // If This Object Is Closer To Us Than The One We Have Selected
          if (buffer[loop * 4 + 1] < GLuint(depth))
            {
              choose = buffer[loop * 4 + 3]; // Select The Closer Object
              depth = buffer[loop * 4 + 1]; // Store How Far Away It Is
            }
        }

      for (WinList::iterator it = m_pickObjects.begin(); it != m_pickObjects.end(); ++it){
          if (choose >= (*it)->objectSize()){
              choose -= (*it)->objectSize();
            }
          else{
              if ((*it)->objectSize() > 1){
                  (*it)->hasBeenPicked(choose);
                }
              else{
                  (*it)->setSelected(true);
                }
              break;
            }
        }
      /*if (choose < objectList.size()){
          objectList[choose]->setSelected(true);
        }*/
  //}
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

GLuint  MyWindow::loadTexture(std::string const &s, bool useMipMap, GLuint *width, GLuint *height)
{
  return sdl->loadTexture(s, useMipMap, width, height);
}

GLuint  MyWindow::loadTexture(const char *s, bool useMipMap, GLuint *width, GLuint *height)
{
  return sdl->loadTexture(s, useMipMap, width, height);
}

void MyWindow::createTextTexture(const char* text, GLuint *texture, int i, Color c)
{
  return sdl->createTextTexture(text, texture, i, c);
}

void MyWindow::createSkyBoxTextures(std::string textures[6])
{
  return sdl->createSkyBoxTextures(textures);
}

bool MyWindow::displayErrorMessage(const char *title, const char *msg)
{
  return sdl->displayErrorMessage(title, msg);
}

bool MyWindow::displayWarningMessage(const char *title, const char *msg)
{
  return sdl->displayWarningMessage(title, msg);
}

bool MyWindow::displayInformationMessage(const char *title, const char *msg)
{
  return sdl->displayInformationMessage(title, msg);
}

bool  MyWindow::destroyTexture(GLuint id)
{
  return sdl->destroyTexture(id);
}

int      MyWindow::loadIconFile(const char *s)
{
  return sdl->loadIconFile(s);
}

int      MyWindow::loadIconFile(std::string s)
{
  return sdl->loadIconFile(s);
}
