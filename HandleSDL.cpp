#ifdef WIN32

#include "HandleSDL.hpp"
#include "MyWindow.hpp"
#include "HandleError.hpp"
#include "String_utils.hpp"

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <cstring>
#include <cstdio>

#include <iostream>

using namespace std;

TTF_Font    *HandleSDL::m_font = 0;

SDL_Surface *flipSurface(SDL_Surface * surface)
{
  int current_line,pitch;
  SDL_Surface * fliped_surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                                      surface->w, surface->h,
                                                      surface->format->BitsPerPixel,
                                                      surface->format->Rmask,
                                                      surface->format->Gmask,
                                                      surface->format->Bmask,
                                                      surface->format->Amask);



  SDL_LockSurface(surface);
  SDL_LockSurface(fliped_surface);

  pitch = surface->pitch;
  for (current_line = 0; current_line < surface->h; current_line ++)
    {
      memcpy(&((unsigned char* )fliped_surface->pixels)[current_line*pitch],
          &((unsigned char* )surface->pixels)[(surface->h - 1  -
                                               current_line)*pitch],
          pitch);
    }

  SDL_UnlockSurface(fliped_surface);
  SDL_UnlockSurface(surface);
  return fliped_surface;
}

HandleSDL::HandleSDL(const std::string &winName, MyWindow *win, unsigned int anti_ali) : GraphicHandler(winName, win, anti_ali)
{
  SDL_Init(SDL_INIT_VIDEO);

  if (TTF_Init() == -1)
    {
      HandleError::showError("TTF_init() failed");
      SDL_Quit();
      exit(-1);
    }
  if (!(screen = SDL_CreateWindow(winName.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                  800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE)))
    {
      HandleError::showError(SDL_GetError());
      TTF_Quit();
      SDL_Quit();
      exit(-1);
    }

  screenWidth = 640;
  screenHeight = 480;

  glcontext = SDL_GL_CreateContext(screen);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, m_anti_ali);

  SDL_initFramerate(&manager);
  SDL_setFramerate(&manager, FPS);
  //SDL_EnableKeyRepeat(10, 10);
  //desactivation du key repeat
  //SDL_EnableKeyRepeat(0, 0);

  /*sdl_flags = screen->flags;
  prev_width = screenWidth;
  prev_height = screenHeight;*/
  SDL_SetWindowMinimumSize(screen, 400, 400);
  sdl_flags = SDL_GetWindowFlags(screen);
  SDL_GetWindowSize(screen, &screenWidth, &screenHeight);
}

HandleSDL::~HandleSDL()
{
  if (m_font)
    TTF_CloseFont(m_font);
  TTF_Quit();
  //delete tex_container;
  /**/SDL_GL_DeleteContext(glcontext);
  /**/SDL_DestroyWindow(screen);
  SDL_Quit();
}

float  HandleSDL::getElapsedTime()
{
  int old_time = m_elapsedTime;

  m_elapsedTime = SDL_GetTicks();
  float tmp = (m_elapsedTime - old_time) / 1000.f;
  return 1000.f - (1000.f - tmp);
}

void  HandleSDL::createTextTexture(const char* text, GLuint *texture, int i, Color c)
{
  if (!m_font && !(m_font = TTF_OpenFont("./font/tabitha.ttf", 65)))
    {
      HandleError::showError("tabitha.ttf: No such file or directory");
      exit(-1);
    }

  SDL_Surface *m_surface;
  SDL_Color textColor = {c.red() * 255, c.green() * 255, c.blue() * 255, 0};

  m_surface = TTF_RenderText_Blended(m_font, text, textColor);
  if (NULL == m_surface)
    {
      HandleError::showError("TTF_RenderTextBlended() failed");
      exit(-1);
    }

  glEnable(GL_TEXTURE_2D);

  // Generation de la texture du texte
  glGenTextures(1, &texture[i]);

  // Selection de la texture generee
  glBindTexture(GL_TEXTURE_2D, texture[i]);

  // Parametrage
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Detection du codage des pixels
  GLenum codagePixel;
  if (m_surface->format->Rmask == 0x000000ff)
    {
      codagePixel = GL_RGBA;
    }
  else
    {
#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif
      codagePixel = GL_BGRA;
    }

  // Chargement de la texture du texte precedemment creee
  glTexImage2D(GL_TEXTURE_2D, 0, 4, m_surface->w, m_surface->h, 0, codagePixel, GL_UNSIGNED_BYTE, NULL);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_surface->w, m_surface->h, codagePixel, GL_UNSIGNED_BYTE, m_surface->pixels);
  glDisable(GL_TEXTURE_2D);
  // Liberation de l'image du texte
  SDL_FreeSurface(m_surface);
}

GLuint   HandleSDL::loadTexture(std::string const &s, bool useMipMap, GLuint *width, GLuint *height)
{
  return HandleSDL::loadTexture(s.c_str(), useMipMap, width, height);
}

GLuint  HandleSDL::loadTextureFromMemory(void *data, int size, const char *name, bool useMipMap, GLuint *width, GLuint *height)
{
  if (!name)
    return 0;

  GLuint glID(tex_container->get(name));
  if (glID > 0)
    return glID;
  SDL_RWops *d = SDL_RWFromMem(data, size);

  return this->internTextureLoad(IMG_Load_RW(d, 0), name, useMipMap, width, height);
}

GLuint   HandleSDL::loadTexture(const char *s, bool useMipMap, GLuint *width, GLuint *height)
{
  if (!s)
    return 0;

  GLuint glID(tex_container->get(s));
  if (glID > 0)
    return glID;
  SDL_Surface *picture_surface = IMG_Load(s);
  if (picture_surface == NULL)
    {
      HandleError::showError(s + std::string(": No such file or directory"));
      return 0;
    }
  return this->internTextureLoad(picture_surface, s, useMipMap, width, height);
}

GLuint  HandleSDL::internTextureLoad(SDL_Surface *picture_surface, const char *name, bool useMipMap, GLuint *width, GLuint *height)
{
  SDL_Surface *gl_surface = NULL;
  SDL_Surface * gl_fliped_surface = NULL;
  Uint32 rmask, gmask, bmask, amask;
  GLuint  glID = 0;

  if (picture_surface == NULL)
    {
      return 0;
    }

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  rmask = 0xff000000;
  gmask = 0x00ff0000;
  bmask = 0x0000ff00;
  amask = 0x000000ff;
#else
  rmask = 0x000000ff;
  gmask = 0x0000ff00;
  bmask = 0x00ff0000;
  amask = 0xff000000;
#endif

  SDL_PixelFormat format = *(picture_surface->format);
  format.BitsPerPixel = 32;
  format.BytesPerPixel = 4;
  format.Rmask = rmask;
  format.Gmask = gmask;
  format.Bmask = bmask;
  format.Amask = amask;

  gl_surface = SDL_ConvertSurface(picture_surface, &format, SDL_SWSURFACE);

  gl_fliped_surface = flipSurface(gl_surface);

  glGenTextures(1, &glID);
  glBindTexture(GL_TEXTURE_2D, glID);

  if (useMipMap)
    {

      gluBuild2DMipmaps(GL_TEXTURE_2D, 4, gl_fliped_surface->w,
                        gl_fliped_surface->h, GL_RGBA, GL_UNSIGNED_BYTE,
                        gl_fliped_surface->pixels);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                      GL_LINEAR_MIPMAP_LINEAR);
    }
  else
    {
      glTexImage2D(GL_TEXTURE_2D, 0, 4, gl_fliped_surface->w,
                   gl_fliped_surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                   gl_fliped_surface->pixels);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (width)
    *width = gl_fliped_surface->w;
  if (height)
    *height = gl_fliped_surface->h;
  SDL_FreeSurface(gl_fliped_surface);
  SDL_FreeSurface(gl_surface);
  SDL_FreeSurface(picture_surface);
  tex_container->add(name, glID);
  return glID;
}

void  HandleSDL::createSkyBoxTextures(string textures[])
{
  GLenum cube_map_target[6] = {
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,
    GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB
  };

  for (int i = 0; i < 6; i++)
    {
      SDL_Surface * picture_surface = NULL;
      SDL_Surface *gl_surface = NULL;
      SDL_Surface * gl_fliped_surface = NULL;
      Uint32 rmask, gmask, bmask, amask;

      picture_surface = IMG_Load(textures[i].c_str());
      if (picture_surface == NULL)
        {
          HandleError::showError(textures[i] + std::string(": No such file or directory"));
          continue;
        }

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
      rmask = 0xff000000;
      gmask = 0x00ff0000;
      bmask = 0x0000ff00;
      amask = 0x000000ff;
#else
      rmask = 0x000000ff;
      gmask = 0x0000ff00;
      bmask = 0x00ff0000;
      amask = 0xff000000;
#endif

      SDL_PixelFormat format = *(picture_surface->format);
      format.BitsPerPixel = 32;
      format.BytesPerPixel = 4;
      format.Rmask = rmask;
      format.Gmask = gmask;
      format.Bmask = bmask;
      format.Amask = amask;

      gl_surface = SDL_ConvertSurface(picture_surface, &format, SDL_SWSURFACE);

      gl_fliped_surface = flipSurface(gl_surface);
      /*glTexImage2D(cube_map_target[i], 0, 3, gl_fliped_surface->w, gl_fliped_surface->h,
                   0, GL_RGBA, GL_UNSIGNED_BYTE, gl_fliped_surface->pixels);*/
      gluBuild2DMipmaps(cube_map_target[i], 4, gl_fliped_surface->w,
                        gl_fliped_surface->h, GL_RGBA, GL_UNSIGNED_BYTE,
                        gl_fliped_surface->pixels);

      SDL_FreeSurface(gl_fliped_surface);
      SDL_FreeSurface(gl_surface);
      SDL_FreeSurface(picture_surface);
    }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //GL_NEAREST_MIPMAP_LINEAR
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //GL_NEAREST_MIPMAP_LINEAR
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //GL_CLAMP
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

bool  HandleSDL::handleEvents()
{
  SDL_Event event;

  while (SDL_PollEvent(&event))
    {
      switch(event.type)
        {
        case SDL_QUIT:
          return true;
        case SDL_KEYDOWN:
          if (event.key.keysym.sym == SDLK_F4 && (event.key.keysym.mod ==
                                                  KMOD_LALT || event.key.keysym.mod == KMOD_RALT))
            return true;
          m_win->keyPressEvent(event.key.keysym.sym);
          break;
        case SDL_KEYUP:
          m_win->keyReleaseEvent(event.key.keysym.sym);
          break;
        case SDL_MOUSEMOTION:
          if (!m_win->isPaused()) {
              if (event.motion.x >= screenWidth - MOUSE_MARGIN || event.motion.x <= MOUSE_MARGIN ||
                  event.motion.y >= screenHeight - MOUSE_MARGIN || event.motion.y <= MOUSE_MARGIN)
                SDL_WarpMouseInWindow(screen, screenWidth / 2, screenHeight / 2);
              m_win->mouseMoveEvent(event.motion.x, event.motion.y);
            }
          break;
        case SDL_MOUSEBUTTONUP:
          m_win->mouseReleaseEvent(event.button.button, event.button.x, event.button.y);
          break;
        case SDL_MOUSEBUTTONDOWN:
          m_win->mousePressEvent(event.button.button, event.button.x, event.button.y);
          break;
        case SDL_WINDOWEVENT:
          switch (event.window.event) {
            case SDL_WINDOWEVENT_RESIZED:
              SDL_GetWindowSize(screen, &screenWidth, &screenHeight);
              m_win->resizeGL(screenWidth, screenHeight);
              break;
              // not working...
            case SDL_WINDOWEVENT_CLOSE:
              return true;
            }
          break;
        default:
          break;
        }
    }
  return false;
}

void  HandleSDL::switchScreenMode()
{
  SDL_SetWindowFullscreen(screen, fullscreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
  fullscreen = !fullscreen;
  SDL_GetWindowSize(screen, &screenWidth, &screenHeight);
  m_win->resizeGL(screenWidth, screenHeight);
}

void  HandleSDL::updateScreen()
{
  //SDL_GL_SwapBuffers();
  SDL_GL_SwapWindow(screen);
  //SDL_framerateDelay(&manager);
}

bool  HandleSDL::displayErrorMessage(const char *title, const char *msg)
{
  return !SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, msg, NULL);
}

bool  HandleSDL::displayWarningMessage(const char *title, const char *msg)
{
  return !SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, title, msg, NULL);
}

bool  HandleSDL::displayInformationMessage(const char *title, const char *msg)
{
  return !SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, title, msg, NULL);
}

#endif
