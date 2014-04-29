#ifndef LINE_HPP
#define LINE_HPP

#include "myGLWidget.hpp"

namespace Object
{
  class Line : public myGLWidget
  {
  public:
    Line(Vector3D from, Vector3D to, Color);
    virtual ~Line();
    virtual void    initializeGL();
    virtual void    paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix);
    void            setTo(Vector3D);
    void            setFrom(Vector3D);

  private:
    Vector3D  m_to;
  };
}

#endif // LINE_HPP
