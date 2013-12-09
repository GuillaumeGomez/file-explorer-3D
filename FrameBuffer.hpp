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
  GLuint      getColorBufferID(unsigned int index) const;
  GLuint      getHeight() const;
  void        setHeight(GLuint);
  GLuint      getWidth() const;
  void        setWidth(GLuint);

private:
  GLuint                m_id;
  GLuint                m_largeur;
  GLuint                m_hauteur;
  std::vector<Texture>  m_colorBuffers;
  GLuint                m_depthBufferID;
};

#endif // FRAMEBUFFER_HPP
