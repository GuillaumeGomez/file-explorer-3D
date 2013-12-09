#ifndef GRAPHICHANDLER_HPP
#define GRAPHICHANDLER_HPP

#include <string>
#include <GL/glew.h>
#include "TextureHandler.hpp"
#include "Color.hpp"

class MyWindow;

#define FPS 30
#define MOUSE_MARGIN  100

class GraphicHandler
{
public:
    GraphicHandler(std::string const &winName, MyWindow*, unsigned int anti_ali = 0);
    virtual ~GraphicHandler();

    virtual bool  handleEvents() = 0;
    virtual void  switchScreenMode() = 0;
    virtual void  updateScreen() = 0;
    int           width();
    int           height();

    virtual GLuint  loadTexture(std::string const &s, bool useMipMap = true, GLuint *width = 0, GLuint *height = 0) = 0;
    virtual GLuint  loadTexture(const char *s, bool useMipMap = true, GLuint *width = 0, GLuint *height = 0) = 0;
    virtual GLuint  loadTextureFromMemory(void *data, int size, const char *name, bool useMipMap = true, GLuint *width = 0, GLuint *height = 0) = 0;
    virtual void createTextTexture(const char* text, GLuint *texture, int i = 0, Color c = Color()) = 0;
    virtual void createSkyBoxTextures(std::string textures[6]) = 0;
    virtual bool displayErrorMessage(const char *title, const char *msg) = 0;
    virtual bool displayWarningMessage(const char *title, const char *msg) = 0;
    virtual bool displayInformationMessage(const char *title, const char *msg) = 0;
    virtual float getElapsedTime() = 0;
    virtual bool destroyTexture(GLuint);
    virtual int  loadIconFile(const char*);
    virtual int  loadIconFile(std::string);

protected:
    std::string           m_name;
    MyWindow              *m_win;
    bool                  fullscreen;
    unsigned int          m_elapsedTime;
    unsigned int          m_anti_ali;
    int                   prev_width;
    int                   prev_height;
    int                   screenHeight;
    int                   screenWidth;
    static TextureHandler *tex_container;
};

#endif // GRAPHICHANDLER_HPP
