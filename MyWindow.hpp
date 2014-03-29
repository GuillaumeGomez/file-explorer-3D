#ifndef MYWINDOW_HPP
#define MYWINDOW_HPP

#include <GL/glew.h>
#include <ctime>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Color.hpp"
#include "Vector3D.hpp"
#include "Utils/HandleFile.hpp"
#include "Texture.hpp"

class myGLWidget;
class Camera;
class KeyHandler;
class HandleSDL;
class HandleThread;
class HandleFpsCount;
class FrameBuffer;
class Tetris;
class HandlePhysics;
class Handle_2048;

typedef std::vector<myGLWidget*>  WinList;

#define WTIMER  1000.f

//#ifdef WIN32
#define WINDOW_HANDLER  HandleSDL
#include "HandleSDL.hpp"
//#else
//#define WINDOW_HANDLER  HandleSFML
//#include "HandleSFML.hpp"
//#endif

namespace Object {
  class Model;
}

class MyWindow
{
  enum {
    MODE_NORMAL,
    MODE_TETRIS,
    MODE_2048
  };

public:
  explicit MyWindow(std::string winName, int antiali = 1, int fps = 30);
  virtual ~MyWindow();
  void        initializeGL();
  void        resizeGL(int width, int height);
  void        paintGL();
  void        paintGL2(glm::mat4 const&view, glm::mat4 const&pers);
  void        setPause(bool b);
  bool        isPaused() const;
  bool        isPlayingTetris() const;
  void        start();
  void        keyPressEvent(int key);
  void        keyReleaseEvent(int);
  void        mouseMoveEvent(int x, int y);
  void        mousePressEvent(int, int posx, int posy);
  void        mouseReleaseEvent(int, int posx, int posy);
  void        repeatKey();
  void        update();
  void        addObject(myGLWidget*, bool isPauseObject = false);
  void        picking();
  void        setDisplaySentence(std::string);
  void        setDisplaySentence(const char*);
  void        setMainCharacter(myGLWidget*);

  Vector3D const  &getPlayerPosition() const;

  static HandleSDL    *getLib();
  void                clearScreen();

private:
  bool                m_printInfo;
  const float         MIN;
  bool                pause;
  bool                m_wireframe;
  int                 m_mode;
  HandlePhysics       *m_physics;
  Object::Model       *m_character;
  WinList             objectList;
  WinList             _2D_objectList;
  WinList             m_pickObjects;
  WinList             m_pauseObjectList;
  Camera              *m_camera;
  KeyHandler          *m_key;
  clock_t             actual;
  clock_t             prev;
  bool                m_end;
  HandleThread        *m_thread;
  HandleFpsCount      *m_fps;
  std::string         display_sentence;
  myGLWidget          *m_displayList[5];
  myGLWidget          *m_disp;
  FrameBuffer         *m_fbo;
  Tetris              *m_tetris;
  Handle_2048         *m_2048;
  int                 mouseX;
  int                 mouseY;

  static HandleSDL      *sdl;
};

#endif // MYWINDOW_HPP
