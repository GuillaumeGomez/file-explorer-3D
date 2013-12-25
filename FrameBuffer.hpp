#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include <vector>
#include "GL/glew.h"
#include "Texture.hpp"

class FrameBuffer
{
public:
  FrameBuffer();
  FrameBuffer(GLuint largeur, GLuint hauteur);
  ~FrameBuffer();
  void        createRenderBuffer(GLuint &id, GLenum formatInterne);
  bool        load();
  GLuint      getFrameBufferID() const;
  GLuint      getTextureID(unsigned int index) const;
  GLuint      getHeight() const;
  void        setHeight(GLuint);
  GLuint      getWidth() const;
  void        setWidth(GLuint);
  void        bind();
  void        unbind();

private:
  GLuint                  m_id;
  GLuint                  m_largeur;
  GLuint                  m_hauteur;
  std::vector<Texture*>   m_colorBuffers;
  GLuint                  m_depthBufferID;
  Texture                 *m_colorBuffer;
};

#endif // FRAMEBUFFER_HPP
