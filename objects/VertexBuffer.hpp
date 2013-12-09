#ifndef VERTEXBUFFER_HPP
#define VERTEXBUFFER_HPP

#include <vector>
#include "GL/glew.h"

//class ShaderHandler;
class myGLWidget;

class VertexBuffer
{
public:
  VertexBuffer(std::vector<GLfloat>, myGLWidget*);
  VertexBuffer(std::vector<GLfloat>, std::vector<GLuint>, myGLWidget*);
  virtual ~VertexBuffer();
  void    draw();

private:
  void                  init();
  std::vector<GLfloat>  m_vertices;
  std::vector<GLuint>   m_elements;
  myGLWidget            *m_widget;
  GLuint                m_vbo;
  GLuint                m_vao;
  GLuint                m_ebo;
  GLint                 m_posAttrib;
  GLint                 m_colAttrib;
  GLint                 m_texAttrib;
  //ShaderHandler         *m_shader;
  GLuint                m_tex;
  GLint                 m_uniView;
  GLint                 m_uniProj;
};

#endif // VERTEXBUFFER_HPP
