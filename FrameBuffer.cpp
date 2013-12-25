#include "FrameBuffer.hpp"
#include "HandleError.hpp"
#include "HandleSDL.hpp"
#include "MyWindow.hpp"

FrameBuffer::FrameBuffer() : m_id(0), m_largeur(0), m_hauteur(0), m_colorBuffers(0), m_depthBufferID(0), m_colorBuffer(0)
{
}

FrameBuffer::FrameBuffer(GLuint largeur, GLuint hauteur)
  : m_id(0), m_largeur(largeur), m_hauteur(hauteur), m_colorBuffers(0), m_depthBufferID(0), m_colorBuffer(0)
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

void FrameBuffer::createRenderBuffer(GLuint &id, GLenum formatInterne)
{
  if (id){
      glDeleteRenderbuffers(1, &id);
      id = 0;
    }

  // Generation de l'identifiant
  glGenRenderbuffers(1, &id);

  // Verrouillage
  glBindRenderbuffer(GL_RENDERBUFFER, id);

  // Configuration du Render Buffer
  glRenderbufferStorage(GL_RENDERBUFFER, formatInterne, m_largeur, m_hauteur);

  // Deverrouillage
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

  // Verrouillage du Frame Buffer
  glBindFramebuffer(GL_FRAMEBUFFER, m_id);

  if (m_colorBuffer)
    delete m_colorBuffer;

  m_colorBuffer = new Texture(m_largeur, m_hauteur, GL_RGBA, GL_RGBA, true);

  m_colorBuffer->load();
  m_colorBuffers.push_back(m_colorBuffer);
  this->createRenderBuffer(m_depthBufferID, GL_DEPTH24_STENCIL8);

  // Association du Color Buffer
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBuffers[0]->getTextureID(), 0);
  // Association du Depth et du Stencil Buffer
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthBufferID);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
      // Liberation des buffers
      glDeleteFramebuffers(1, &m_id);
      glDeleteRenderbuffers(1, &m_depthBufferID);
      m_id = 0;
      m_depthBufferID = 0;

      delete m_colorBuffer;

      m_colorBuffers.clear();

      HandleError::showError("FBO not build correctly");
      return false;
    }

  // Deverrouillage du Frame Buffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return true;
}

GLuint      FrameBuffer::getFrameBufferID() const
{
  return m_id;
}

GLuint      FrameBuffer::getTextureID(unsigned int index) const
{
  return m_colorBuffers[index]->getTextureID();
}

GLuint      FrameBuffer::getHeight() const
{
  return m_hauteur;
}

void  FrameBuffer::setHeight(GLuint i)
{
  m_hauteur = i;
}

GLuint      FrameBuffer::getWidth() const
{
  return m_largeur;
}

void  FrameBuffer::setWidth(GLuint i)
{
  m_largeur = i;
}

void  FrameBuffer::bind()
{
  if (!m_id)
    return;
  glBindFramebuffer(GL_FRAMEBUFFER, m_id);
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //glViewport(0, 0, m_largeur, m_hauteur);
}

void  FrameBuffer::unbind()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //glViewport(0, 0, MyWindow::getLib()->width(), MyWindow::getLib()->height());
}
