#ifndef BOX_HPP
#define BOX_HPP

#include "../myGLWidget.hpp"
#include <vector>

class Box : public myGLWidget
{
public:
  Box(Vector3D v, Rotation r, Color c, float size);
  Box(Vector3D v, Rotation r, std::string texture, float size);
  virtual ~Box();
  virtual void  paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix);
  virtual void  initializeGL();
  virtual void  initializeGLNoList();
  virtual std::string getClassName() const;

protected:
  float                 m_size;
};

#endif // BOX_HPP
