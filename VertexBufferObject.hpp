#ifndef VERTEXBUFFEROBJECT_HPP
#define VERTEXBUFFEROBJECT_HPP

#include "GL/glew.h"
#include <map>
#include <vector>

class myGLWidget;

class VertexBufferObject
{
public:
  VertexBufferObject();
  virtual ~VertexBufferObject();

private:
  GLuint                              m_buffer;
  bool                                m_dataUploaded;
  int                                 m_bufferType;
  std::map<myGLWidget*, unsigned int> m_container;
  std::vector<GLfloat>                m_datas;
};

#endif // VERTEXBUFFEROBJECT_HPP
