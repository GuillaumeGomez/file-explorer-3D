#ifndef HANDLESDL_HPP
#define HANDLESDL_HPP

#include <string>
#include <SDL/SDL.h>
#include <SDL/SDL_framerate.h>
#include <GL/glew.h>
#include <SDL/SDL_ttf.h>
#include "TextureHandler.hpp"
#include "Color.hpp"
#include "Texture.hpp"

class MyWindow;

#define FPS 30
#define MOUSE_MARGIN  100

class HandleSDL
{
public:
  HandleSDL(std::string const &winName, MyWindow*, unsigned int anti_ali = 0);
  virtual ~HandleSDL();
  bool  handleEvents();
  void  switchScreenMode();
  void  updateScreen();
  virtual float  getElapsedTime();
  virtual GLuint  loadTexture(std::string const &s, bool useMipMap = true, GLuint *width = 0, GLuint *height = 0);
  virtual GLuint  loadTexture(const char *s, bool useMipMap = true, GLuint *width = 0, GLuint *height = 0);
  virtual GLuint  loadTextureFromMemory(void *data, int size, const char *name, bool useMipMap = true, GLuint *width = 0, GLuint *height = 0);
  virtual Texture *createTextTexture(const char* text, Texture *texture = 0, Color c = Color());
  virtual void    createSkyBoxTextures(std::string textures[6]);
  virtual bool    displayErrorMessage(const char *title, const char *msg);
  virtual bool    displayWarningMessage(const char *title, const char *msg);
  virtual bool    displayInformationMessage(const char *title, const char *msg);
  int             height();
  int             width();
  GLuint          loadIconFile(std::string s);
  GLuint          loadIconFile(const char *s);
  bool            destroyTexture(GLuint id);
  int             getEvent(SDL_Event*);
  void            displayCursor(bool display);
  void            moveCursor(int x, int y);
  void            setFPSMode(bool p);
  bool            saveImage(SDL_Surface *s, const char *filename);
  bool            saveImage(SDL_Surface *s, std::string filename);
  bool            saveImage(Texture*, const char *filename);
  bool            saveImage(Texture*, std::string filename);
  SDL_Surface     *convertTextureToSurface(Texture*);
  static SDL_Surface      *loadImage(std::string img);
  static SDL_Surface      *loadImage(const char*);
  static void             freeImage(SDL_Surface*);
  static Color            getPixelColor(SDL_Surface*, unsigned int x, unsigned int y);

private:
  void    resetCursor();
  GLuint  internTextureLoad(SDL_Surface *picture_surface, const char *name, bool useMipMap, GLuint *width, GLuint *height);
  std::string           m_name;
  MyWindow              *m_win;
  bool                  fullscreen;
  unsigned int          m_elapsedTime;
  unsigned int          m_anti_ali;
  int                   prev_width;
  int                   prev_height;
  int                   screenHeight;
  int                   screenWidth;
  int                   mouse_x;
  int                   mouse_y;
  FPSmanager    manager;
  //SDL_Surface   *screen;
  SDL_Window    *screen;
  Uint32        sdl_flags;
  SDL_GLContext glcontext;

  static TTF_Font       *m_font;
};

#endif // HANDLESDL_HPP
