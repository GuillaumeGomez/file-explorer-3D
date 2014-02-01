#ifndef HEIGHTMAP_HPP
#define HEIGHTMAP_HPP

#include "myGLWidget.hpp"

namespace Object {
  class HeightMap : public myGLWidget
  {
  public:
    HeightMap(Vector3D, unsigned int size);
    HeightMap(Vector3D, std::string img);
    virtual ~HeightMap();
    virtual void  initializeGL();
    virtual void  paintGL(const glm::mat4& view_matrix, const glm::mat4& proj_matrix);

  private:
    unsigned int m_width;
    unsigned int m_height;
    std::string m_img;
    Texture     *m_tex[5];
  };
}

#endif // HEIGHTMAP_HPP
