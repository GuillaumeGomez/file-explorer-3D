#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "../myGLWidget.hpp"

namespace Object
{
  class Sphere : public myGLWidget
  {
  public:
    Sphere(Vector3D, Rotation, Color, float rayon);
    Sphere(Vector3D, Rotation, std::string tex, float rayon);
    virtual ~Sphere(){}
    virtual void    initializeGL();
    virtual void    initializeGLNoList();
    virtual void    paintGL(const glm::mat4& view_matrix, const glm::mat4& proj_matrix);
    virtual std::string getClassName() const;
    float const     &getRadius() const;

  protected:
    float   m_rayon;
    GLuint  vertexbuffer;
    GLuint  uvbuffer;
    GLuint  normalbuffer;
    GLuint  elementbuffer;
  };
}

#endif // SPHERE_HPP
