#ifndef CUBE_HPP
#define CUBE_HPP

#include "Rectangle.hpp"

namespace Object
{
  class Cube : public Rectangle
  {
  public:
    Cube(Vector3D p, Rotation r, Color c, float size);
    Cube(Vector3D p, Rotation r, std::string tex, float size);
    virtual ~Cube(){}
  };
}

#endif // CUBE_HPP
