#ifndef TEXTUREHANDLER_HPP
#define TEXTUREHANDLER_HPP

#include <list>
#include <string>
#include "GL/glew.h"

typedef std::list<std::pair<std::string, std::pair<GLuint, unsigned int> > > tex_map;

class TextureHandler
{
public:
  static TextureHandler  *getInstance();
  virtual ~TextureHandler();

  GLuint            add(const char*, GLuint id); //retourne 0 si erreur
  GLuint            add(std::string const&, GLuint id); //pareil
  GLuint            add(GLuint);
  GLuint            get(const char*); //retourne 0 si inexistant
  GLuint            get(std::string const&); //pareil
  GLuint            operator[](const char*); //alias de la methode get
  GLuint            operator[](std::string const&); //alias de la methode get
  std::string const get(GLuint);
  std::string const operator[](GLuint);
  unsigned int      size() const;
  unsigned int      numberOfThisTexture(GLuint id);
  unsigned int      numberOfThisTexture(std::string);
  bool              destroy(GLuint); //return false if texture wasn't found
  bool              destroy(const char*); //return false if texture wasn't found
  bool              destroy(std::string const&); //return false if texture wasn't found

private:
  TextureHandler();
  tex_map::iterator findElement(GLuint);
  tex_map::iterator findElement(std::string);

  tex_map       textures;

  static TextureHandler *instance;
};

#endif // TEXTUREHANDLER_HPP
