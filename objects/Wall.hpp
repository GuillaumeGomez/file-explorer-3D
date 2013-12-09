#ifndef WALL_HPP
#define WALL_HPP

#include "Plane.hpp"

namespace Object
{
  class Wall : public Plane
  {
  public:
    Wall(Vector3D, Rotation, Color, float width = 2.f, float height = 2.f);
    Wall(Vector3D, Rotation, std::string tex, float width = 2.f, float height = 2.f);
    virtual ~Wall(){}
    virtual void    initializeGL();
    virtual void    initializeGLNoList();
    virtual void    paintGL(const glm::mat4& view_matrix, const glm::mat4& proj_matrix);
  };
}

#endif // WALL_HPP
