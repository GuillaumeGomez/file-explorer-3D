#ifndef TEXTUREHANDLER_HPP
#define TEXTUREHANDLER_HPP

#include <map>
#include <string>
#include "GL/glew.h"

typedef std::map<std::string, std::pair<GLuint, unsigned int> > tex_map;

class TextureHandler
{
public:
  TextureHandler();
  virtual ~TextureHandler();

  GLuint            add(const char*, GLuint id); //retourne 0 si erreur
  GLuint            add(std::string const&, GLuint id); //pareil
  GLuint            get(const char*); //retourne 0 si inexistant
  GLuint            get(std::string const&); //pareil
  GLuint            operator[](const char*); //alias de la methode get
  GLuint            operator[](std::string const&); //alias de la methode get
  std::string const get(GLuint);
  std::string const operator[](GLuint);
  unsigned int      size() const;
  bool              destroy(GLuint);
  bool              destroy(const char*);
  bool              destroy(std::string const&);

private:
  std::pair<GLuint, unsigned int>  getPair(GLuint id);
  std::pair<GLuint, unsigned int>  getPair(const std::string&);

  tex_map       textures;
};

#endif // TEXTUREHANDLER_HPP
