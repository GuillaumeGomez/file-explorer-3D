#ifndef WIN32

#include "HandleSFML.hpp"
#include "MyWindow.hpp"
#include "HandleError.hpp"
#include <SFML2/Window.hpp>
#include <SFML2/OpenGL.hpp>
#include <SFML2/Window/ContextSettings.hpp>
#include <SFML2/Graphics/Image.hpp>

HandleSFML::HandleSFML(const std::string &winName, MyWindow *w) : GraphicHandler(winName, w)
{
    sf::ContextSettings settings;

    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 0;

    window = new sf::RenderWindow(sf::VideoMode(800, 600), winName, sf::Style::Default, settings);

    window->setVerticalSyncEnabled(true);
}

HandleSFML::~HandleSFML()
{
    delete window;
}

bool  HandleSFML::handleEvents()
{
    sf::Event event;

    while (window->pollEvent(event))
    {
        switch (event.type){
        case sf::Event::Closed:
            return true;
        case sf::Event::MouseMoved:
            if (event.mouseMove.x >= screenWidth - MOUSE_MARGIN || event.mouseMove.x <= MOUSE_MARGIN ||
                    event.mouseMove.y >= screenHeight - MOUSE_MARGIN || event.mouseMove.y <= MOUSE_MARGIN)
                sf::Mouse::setPosition(sf::Vector2i(screenWidth / 2, screenHeight / 2));
            m_win->mouseMoveEvent(event.mouseMove.x, event.mouseMove.y);
            break;
        case sf::Event::Resized:
            screenWidth = event.size.width;
            screenHeight = event.size.height;
            m_win->resizeGL(event.size.width, event.size.height);
            break;
        case sf::Event::KeyPressed:
            if (event.key.code == sf::Keyboard::Escape)
                return true;
            m_win->keyPressEvent(event.key.code);
            break;
        case sf::Event::KeyReleased:
            m_win->keyReleaseEvent(event.key.code);
            break;
        case sf::Event::MouseButtonPressed:
            m_win->mousePressEvent(event.mouseButton.button, event.mouseButton.x, event.mouseButton.y);
            break;
        case sf::Event::MouseButtonReleased:
            m_win->mouseReleaseEvent(event.mouseButton.button, event.mouseButton.x, event.mouseButton.y);
            break;
        default:
            break;
        }
        return false;
    }
}

void  HandleSFML::updateScreen()
{
    window->display();
}

void  HandleSFML::switchScreenMode()
{
}

GLuint  HandleSFML::loadTexture(std::string const &s, bool useMipMap, int *width, int *height)
{
    if (s.length() < 1)
        return 0;
    GLuint glID(tex_container->get(s));

    if (glID > 0)
      return glID;

    sf::Image picture_surface;

    if (!picture_surface.loadFromFile(s))
      {
        HandleError::showError(s + std::string(": No such file or directory"));
        return 0;
      }

    glGenTextures(1, &glID);
    glBindTexture(GL_TEXTURE_2D, glID);

    if (useMipMap)
      {

        gluBuild2DMipmaps(GL_TEXTURE_2D, 4, picture_surface.getSize().x,
                          picture_surface.getSize().y, GL_RGBA, GL_UNSIGNED_BYTE,
                          picture_surface.getPixelsPtr());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);

      }
    /*else if (skybox){
        gluBuild2DMipmaps(GL_TEXTURE_2D, 4, gl_fliped_surface->w,
                          gl_fliped_surface->h, GL_RGBA, GL_UNSIGNED_BYTE,
                          gl_fliped_surface->pixels);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      }*/
    else
      {
        glTexImage2D(GL_TEXTURE_2D, 0, 4, picture_surface.getSize().x,
                     picture_surface.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     picture_surface.getPixelsPtr());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (width)
      *width = picture_surface.getSize().x;
    if (height)
      *height = picture_surface.getSize().y;

    tex_container->add(s, glID);
    return glID;
}

GLuint  HandleSFML::loadTexture(const char *s, bool useMipMap, int *width, int *height)
{
    if (!s)
      return 0;
    std::string s2(s);
    return this->loadTexture(s, useMipMap, width, height);
}

void HandleSFML::createTextTexture(const char* text, GLuint *texture, int i, Color c)
{
}

void HandleSFML::createSkyBoxTextures(std::string textures[6])
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
        sf::Image picture_surface;

        if (!picture_surface.loadFromFile(textures[i]))
        {
            HandleError::showError(textures[i] + std::string(": No such file or directory"));
            continue;
        }

        picture_surface.flipVertically();
        glTexImage2D(cube_map_target[i], 0, 3, picture_surface.getSize().x, picture_surface.getSize().y,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, picture_surface.getPixelsPtr());
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP); //GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP); //GL_CLAMP_TO_EDGE
}

bool HandleSFML::displayErrorMessage(const char *title, const char *msg)
{
    return false;
}

bool HandleSFML::displayWarningMessage(const char *title, const char *msg)
{
    return false;
}

bool HandleSFML::displayInformationMessage(const char *title, const char *msg)
{
    return false;
}

#endif
