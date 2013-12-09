#ifndef HANDLESFML_HPP
#define HANDLESFML_HPP

#ifndef WIN32

#include "GraphicHandler.hpp"
#include <SFML2/Graphics.hpp>
#include <SFML2/Window/Keyboard.hpp>

#define SDL_BUTTON_LEFT sf::Event::MouseLeft
#define SDLK_MOUSE_RIGHT sf::Event::MouseRight
#define SDLK_F11 sf::Keyboard::F11
#define SDLK_RETURN sf::Keyboard::Return
#define SDLK_TAB sf::Keyboard::Tab
#define SDLK_BACKSPACE sf::Keyboard::BackSpace
#define SDLK_a sf::Keyboard::A
#define SDLK_d sf::Keyboard::D
#define SDLK_w sf::Keyboard::W
#define SDLK_s sf::Keyboard::S

class HandleSFML : public GraphicHandler
{
public:
  HandleSFML(std::string const &winName, MyWindow*);
  virtual ~HandleSFML();

  virtual bool  handleEvents();
  virtual void  switchScreenMode();
  virtual void  updateScreen();
  int           width();
  int           height();

  virtual GLuint  loadTexture(std::string const &s, bool useMipMap = true, int *width = 0, int *height = 0);
  virtual GLuint  loadTexture(const char *s, bool useMipMap = true, int *width = 0, int *height = 0);
  virtual void createTextTexture(const char* text, GLuint *texture, int i = 0, Color c = Color());
  virtual void createSkyBoxTextures(std::string textures[6]);
  virtual bool displayErrorMessage(const char *title, const char *msg);
  virtual bool displayWarningMessage(const char *title, const char *msg);
  virtual bool displayInformationMessage(const char *title, const char *msg);

private:
  sf::RenderWindow *window;
};
#endif

#endif // HANDLESFML_HPP
