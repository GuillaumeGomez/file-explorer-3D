#include "Texture.hpp"
#include "../MyWindow.hpp"

Texture::Texture()
  : m_id(0), m_textureName(""), m_width(0), m_height(0), m_format(0), m_internFormat(0),
    m_emptyTexture(false), m_repeat(false)
{
}

Texture::Texture(GLuint id)
  : m_id(id), m_textureName(""), m_width(0), m_height(0), m_format(0), m_internFormat(0),
    m_emptyTexture(false), m_repeat(false)
{
  TextureHandler::getInstance()->add(m_id);
}

Texture::Texture(std::string imageFile)
  : m_id(0), m_textureName(imageFile), m_width(0), m_height(0), m_format(0), m_internFormat(0),
    m_emptyTexture(false), m_repeat(false)
{
}

Texture::Texture(Texture const &t)
  : m_id(t.m_id), m_textureName(t.m_textureName), m_width(t.m_width), m_height(t.m_height),
    m_format(t.m_format), m_internFormat(t.m_internFormat), m_emptyTexture(t.m_emptyTexture),
    m_repeat(t.m_repeat)
{
  TextureHandler::getInstance()->add(m_id);
}

Texture::Texture(GLuint width, GLuint height, GLenum format, GLenum internFormat, bool emptyTexture)
  : m_id(0), m_textureName(""), m_width(width), m_height(height), m_format(format),
    m_internFormat(internFormat), m_emptyTexture(emptyTexture), m_repeat(false)
{
}

Texture::~Texture()
{
  this->destroy();
}

bool  Texture::load()
{
  if (m_id)
    return true;
  this->destroy();
  if (m_textureName.empty()){
      if (m_width == 0 || m_height == 0)
        return false;
      glGenTextures(1, &m_id);

      // Verrouillage
      glBindTexture(GL_TEXTURE_2D, m_id);

      if (m_repeat) {
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexImage2D(GL_TEXTURE_2D, 0, m_internFormat, m_width, m_height, 0, m_format, GL_UNSIGNED_BYTE, 0);

      // Deverrouillage
      glBindTexture(GL_TEXTURE_2D, 0);
      return true;
    }
  m_id = MyWindow::getLib()->loadTexture(m_textureName, true, &m_width, &m_height);
  return m_id != 0;
}

bool  Texture::hasBeenLoaded() const
{
  return m_id != 0;
}

GLuint  Texture::getTextureID() const
{
  return m_id;
}

void  Texture::setTextureID(GLuint i)
{
  this->destroy();
  m_id = i;
  TextureHandler::getInstance()->add(m_id);
}

void  Texture::setTexture(std::string s)
{
  if (s == m_textureName)
    return;
  m_textureName = s;
  if (m_id){
      this->destroy();
      this->load();
    }
}

std::string const &Texture::getTextureName() const
{
  return m_textureName;
}

GLuint  Texture::height() const
{
  return m_height;
}

GLuint  Texture::width() const
{
  return m_width;
}

void  Texture::bind()
{
  glBindTexture(GL_TEXTURE_2D, m_id);
}

void  Texture::unbind()
{
  glBindTexture(GL_TEXTURE_2D, 0);
}

void  Texture::bind(GLuint i)
{
  glBindTexture(GL_TEXTURE_2D, i);
}

void  Texture::destroy()
{
  m_width = 0;
  m_height = 0;
  if (!m_id)
    return;
  TextureHandler::getInstance()->destroy(m_id);
  m_id = 0;
}

void    Texture::setRepeat(bool b){
  m_repeat = b;
}

bool    Texture::isRepeating() const
{
  return m_repeat;
}

void  Texture::setSize(GLuint width, GLuint height)
{
  m_width = width;
  m_height = height;
}

void  Texture::operator=(Texture const &t)
{
  if (t.m_id != m_id) {
      this->destroy();
    }
  m_id = t.m_id;
  TextureHandler::getInstance()->add(m_id);
  m_textureName = t.m_textureName;
  m_width = t.m_width;
  m_height = t.m_height;
  m_format = t.m_format;
  m_internFormat = t.m_internFormat;
  m_emptyTexture = t.m_emptyTexture;
  m_repeat = t.m_repeat;
}

void  Texture::bindTexture(int i)
{
  glActiveTexture(GL_TEXTURE0 + i);
  glBindTexture(GL_TEXTURE_2D, m_id);
  //glBindSampler(i, uiSampler);
}
