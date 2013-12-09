#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "GL/glew.h"
#include <string>

class Texture
{
public:
  Texture(GLuint id = 0);
  Texture(std::string);
  Texture(GLuint largeur, GLuint hauteur, GLenum format, GLenum formatInterne, bool textureVide);
  virtual ~Texture();
  bool              load();
  bool              hasBeenLoaded() const;
  GLuint            getTextureID() const;
  void              setTextureID(GLuint);
  void              setTexture(std::string);
  std::string const &getTextureName() const;
  GLuint            height() const;
  GLuint            width() const;
  void              bind();
  void              unbind();
  void              destroy();
  void              setRepeat(bool);
  bool              isRepeating() const;

  static void       bind(GLuint);

private:
  GLuint      m_id;
  std::string m_textureName;
  GLuint      m_largeur;
  GLuint      m_hauteur;
  GLenum      m_format;
  GLenum      m_formatInterne;
  bool        m_textureVide;
  bool        m_repeat;
};

#endif // TEXTURE_HPP
