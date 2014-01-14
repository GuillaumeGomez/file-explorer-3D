#ifndef CYLINDER_HPP
#define CYLINDER_HPP

#include "../myGLWidget.hpp"

namespace Object
{
  class Cylinder : public myGLWidget
  {
  public:
    Cylinder(Vector3D p, Rotation r, Color c, float rayon, float height);
    Cylinder(Vector3D p, Rotation r, std::string tex, float rayon, float height);
    virtual ~Cylinder(){}
    virtual void  initializeGL();
    virtual void  paintGL(const glm::mat4& view_matrix, const glm::mat4& proj_matrix);
    virtual void  initializeGLNoList();
    virtual std::string getClassName() const;

  private:
    float       m_rayon;
    float       m_height;
    const int   m_uSlices;
    GLuint      m_BordersVboId;
  };
}

#endif // CYLINDER_HPP
