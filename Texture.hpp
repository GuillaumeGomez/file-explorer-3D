#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "GL/glew.h"
#include <string>

class Texture
{
public:
  Texture(GLuint id = 0);
  Texture(std::string imageFile);
  Texture(Texture const &t);
  Texture(GLuint width, GLuint height, GLenum format, GLenum internFormat, bool emptyTexture);
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
  void              setSize(GLuint width, GLuint height);
  void              operator=(Texture const&);

  static void       bind(GLuint);

private:
  GLuint      m_id;
  std::string m_textureName;
  GLuint      m_width;
  GLuint      m_height;
  GLenum      m_format;
  GLenum      m_internFormat;
  bool        m_emptyTexture;
  bool        m_repeat;
};

#endif // TEXTURE_HPP
