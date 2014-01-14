#ifndef PLANE_HPP
#define PLANE_HPP

#include "../myGLWidget.hpp"

namespace Object
{
  class Plane : public myGLWidget
  {
  public:
    Plane(Vector3D, Rotation, Color, float width = 2.f, float height = 2.f);
    Plane(Vector3D, Rotation, std::string tex, float width = 2.f, float height = 2.f);
    virtual ~Plane(){}
    virtual void    initializeGL();
    virtual void    initializeGLNoList();
    virtual void    paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix);
    virtual std::string getClassName() const;

  protected:
    float   width;
    float   height;
  };
}

#endif // PLANE_HPP
