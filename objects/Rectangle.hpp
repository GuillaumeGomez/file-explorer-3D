#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include "../myGLWidget.hpp"

namespace Object
{
  class Rectangle : public myGLWidget
  {
  public:
    Rectangle(Vector3D p, Rotation r, Color c, float sizex = 0.f, float sizey = 0.f, float sizez = 0.f);
    Rectangle(Vector3D p, Rotation r, std::string tex, float sizex = 0.f, float sizey = 0.f, float sizez = 0.f);
    virtual ~Rectangle(){}
    virtual void    initializeGL();
    virtual void    paintGL(const glm::mat4& view_matrix, const glm::mat4& proj_matrix);

    float   sizeX;
    float   sizeY;
    float   sizeZ;
  };
}

#endif // RECTANGLE_HPP
