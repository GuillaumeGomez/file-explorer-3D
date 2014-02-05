#ifndef HEIGHTMAP_HPP
#define HEIGHTMAP_HPP

#include "myGLWidget.hpp"

namespace Object {
  class HeightMap : public myGLWidget
  {
  public:
    HeightMap(Vector3D, unsigned int width, unsigned int height, float case_size = 5.f);
    HeightMap(Vector3D, std::string img, float case_size = 5.f);
    virtual ~HeightMap();
    virtual void  initializeGL();
    virtual void  paintGL(const glm::mat4& view_matrix, const glm::mat4& proj_matrix);
    virtual std::string getClassName() const;
    float const   &getMinHeight() const;
    float const   &getMaxHeight() const;
    unsigned int  getWidth() const;
    unsigned int  getHeight() const;
    float const   &getCaseSize() const;

  private:
    unsigned int  m_width;
    unsigned int  m_height;
    float         m_case_size;
    float         m_tex_repeat;
    std::string   m_img;
    Texture       *m_tex[4];
    GLint         m_uniloc_height;
    float         height, min_height, max_height;
  };
}

#endif // HEIGHTMAP_HPP
