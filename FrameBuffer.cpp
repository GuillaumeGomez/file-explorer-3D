#include "FrameBuffer.hpp"
#include "HandleError.hpp"
#include "HandleSDL.hpp"
#include "MyWindow.hpp"

FrameBuffer::FrameBuffer() : m_id(0), m_width(0), m_height(0), m_colorBuffers(0), m_depthBufferID(0), m_colorBuffer(0)
{
}

FrameBuffer::FrameBuffer(GLuint width, GLuint height)
  : m_id(0), m_width(width), m_height(height), m_colorBuffers(0), m_depthBufferID(0), m_colorBuffer(0)
{
}

FrameBuffer::~FrameBuffer()
{
  this->unbind();
  if (m_id)
    glDeleteFramebuffers(1, &m_id);
  if (m_depthBufferID)
    glDeleteRenderbuffers(1, &m_depthBufferID);
  if (m_colorBuffer)
    delete m_colorBuffer;
  m_colorBuffers.clear();
}

void FrameBuffer::createRenderBuffer(GLuint &id, GLenum internFormat)
{
  if (id){
      glDeleteRenderbuffers(1, &id);
      id = 0;
    }

  glGenRenderbuffers(1, &id);
  glBindRenderbuffer(GL_RENDERBUFFER, id);
  glRenderbufferStorage(GL_RENDERBUFFER, internFormat, m_width, m_height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

bool  FrameBuffer::load()
{
  if (m_id){
      glDeleteFramebuffers(1, &m_id);
      m_id = 0;
      m_colorBuffers.clear();
    }

  glGenFramebuffers(1, &m_id);

  glBindFramebuffer(GL_FRAMEBUFFER, m_id);

  if (m_colorBuffer)
    delete m_colorBuffer;

  m_colorBuffer = new Texture(m_width, m_height, GL_RGBA, GL_RGBA, true);

  if (!m_colorBuffer || !m_colorBuffer->load()) {
      if (m_colorBuffer){
          delete m_colorBuffer;
          m_colorBuffer = 0;
        }
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      return false;
    }

  m_colorBuffers.push_back(m_colorBuffer);
  this->createRenderBuffer(m_depthBufferID, GL_DEPTH24_STENCIL8);

  // Association of Color Buffer
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBuffers[0]->getTextureID(), 0);
  // Association of Depth and of Stencil Buffer
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthBufferID);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
      glDeleteFramebuffers(1, &m_id);
      glDeleteRenderbuffers(1, &m_depthBufferID);
      m_id = 0;
      m_depthBufferID = 0;

      delete m_colorBuffer;

      m_colorBuffers.clear();

      HandleError::showError("FBO not build correctly");
      return false;
    }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return true;
}

GLuint      FrameBuffer::getFrameBufferID() const
{
  return m_id;
}

GLuint      FrameBuffer::getTextureID(unsigned int index) const
{
  if (index >= m_colorBuffers.size())
    return 0;
  return m_colorBuffers[index]->getTextureID();
}

GLuint      FrameBuffer::getHeight() const
{
  return m_height;
}

void  FrameBuffer::setHeight(GLuint i)
{
  m_height = i;
}

GLuint      FrameBuffer::getWidth() const
{
  return m_width;
}

void  FrameBuffer::setWidth(GLuint i)
{
  m_width = i;
}

void  FrameBuffer::bind() const
{
  if (!m_id)
    return;
  glBindFramebuffer(GL_FRAMEBUFFER, m_id);
  glClearColor(0.f, 0.f, 0.f, 1.f);
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //glViewport(0, 0, m_largeur, m_hauteur);
}

void  FrameBuffer::unbind() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //glViewport(0, 0, MyWindow::getLib()->width(), MyWindow::getLib()->height());
}

void  FrameBuffer::resize(GLuint w, GLuint h)
{
  m_width = w;
  m_height = h;
  this->load();
}
