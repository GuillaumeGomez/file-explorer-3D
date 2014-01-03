#ifndef HANDLESDL_HPP
#define HANDLESDL_HPP

#include <string>
#include <SDL/SDL.h>
#include <SDL/SDL_framerate.h>
#include <GL/glew.h>
#include <SDL/SDL_ttf.h>
#include "TextureHandler.hpp"
#include "Color.hpp"
#include "GraphicHandler.hpp"
#include "Texture.hpp"

class MyWindow;

#define FPS 30
#define MOUSE_MARGIN  100

class HandleSDL : public GraphicHandler
{
public:
    HandleSDL(std::string const &winName, MyWindow*, unsigned int anti_ali = 0);
    ~HandleSDL();
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

private:
    GLuint  internTextureLoad(SDL_Surface *picture_surface, const char *name, bool useMipMap, GLuint *width, GLuint *height);
    FPSmanager    manager;
    //SDL_Surface   *screen;
    SDL_Window    *screen;
    Uint32        sdl_flags;
    SDL_GLContext glcontext;

    static TTF_Font       *m_font;
};

#endif // HANDLESDL_HPP
