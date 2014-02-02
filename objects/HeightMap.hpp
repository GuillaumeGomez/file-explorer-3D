#ifndef HEIGHTMAP_HPP
#define HEIGHTMAP_HPP

#include "myGLWidget.hpp"

namespace Object {
  class HeightMap : public myGLWidget
  {
  public:
    HeightMap(Vector3D, unsigned int width, unsigned int height, float case_size);
    HeightMap(Vector3D, std::string img, float case_size);
    virtual ~HeightMap();
    virtual void  initializeGL();
    virtual void  paintGL(const glm::mat4& view_matrix, const glm::mat4& proj_matrix);

  private:
    unsigned int  m_width;
    unsigned int  m_height;
    float         m_case_size;
    float         m_tex_repeat;
    std::string   m_img;
    Texture       *m_tex[4];
    GLint         m_uniloc_height;
    float         height;
  };
}

#endif // HEIGHTMAP_HPP
