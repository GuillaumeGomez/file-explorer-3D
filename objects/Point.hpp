#ifndef POINT_HPP
#define POINT_HPP

#include "myGLWidget.hpp"

namespace Object {
  class Point : public myGLWidget
  {
  public:
    Point(Vector3D, Color);
    virtual ~Point();
    virtual void  initializeGL();
    virtual void  paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix);
  };
}

#endif // POINT_HPP
