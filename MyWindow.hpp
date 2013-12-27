#ifndef MYWINDOW_HPP
#define MYWINDOW_HPP

#include <GL/glew.h>
#include <ctime>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Color.hpp"
#include "Vector3D.hpp"
#include "VertexBuffer.hpp"
#include "Utils/HandleFile.hpp"

class myGLWidget;
class Camera;
class KeyHandler;
class GraphicHandler;
class HandleThread;
class HandleFpsCount;
class FrameBuffer;
class Tetris;

typedef std::vector<myGLWidget*>  WinList;

#define WTIMER  1000.f

#ifdef WIN32
#define WINDOW_HANDLER  HandleSDL
#else
#define WINDOW_HANDLER  HandleSFML
#endif

class MyWindow
{
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
  void        addObject(myGLWidget*, bool is2D = false);
  void        picking();
  void        setDisplaySentence(std::string);
  void        setDisplaySentence(const char*);

  Vector3D const  &getPlayerPosition() const;

  static GraphicHandler   *getLib();
  static GLuint   loadTexture(std::string const &s, bool useMipMap = true, GLuint *width = 0, GLuint *height = 0);
  static GLuint   loadTexture(const char *s, bool useMipMap = true, GLuint *width = 0, GLuint *height = 0);
  static void     createTextTexture(const char* text, GLuint *texture, int i = 0, Color c = Color());
  static void     createSkyBoxTextures(std::string textures[6]);
  static bool     displayErrorMessage(const char *title, const char *msg);
  static bool     displayWarningMessage(const char *title, const char *msg);
  static bool     displayInformationMessage(const char *title, const char *msg);
  static bool     destroyTexture(GLuint);
  static int      loadIconFile(const char*);
  static int      loadIconFile(std::string);
  static void     clearScreen();

private:
  bool                m_printInfo;
  const float         MIN;
  bool                pause;
  bool                m_test;
  bool                m_wireframe;
  bool                m_tetrisMode;
  //FPSCount            m_fps;
  WinList             objectList;
  WinList             _2D_objectList;
  WinList             m_pickObjects;
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
  int                 mouseX;
  int                 mouseY;

  static GraphicHandler      *sdl;
};

#endif // MYWINDOW_HPP
