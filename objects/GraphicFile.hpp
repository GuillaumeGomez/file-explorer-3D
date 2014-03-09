#ifndef GRAPHICFILE_HPP
#define GRAPHICFILE_HPP

#include "Rectangle.hpp"
#include <string>

namespace Object
{
  class GraphicFile : public Rectangle
  {
  public:
    GraphicFile(Vector3D, Rotation, Color, std::string fileName);
    GraphicFile(Vector3D, Rotation, Color, const char *fileName);
    virtual ~GraphicFile();
    virtual void    initializeGL();
    virtual void    paintGL(const glm::mat4& view_matrix, const glm::mat4& proj_matrix);
    virtual void    pick(const glm::mat4& view_matrix, const glm::mat4& proj_matrix);
    virtual void    update(const float &);
    std::string const &getFileName() const;
    virtual void    setSelected(bool);

  private:
    Rotation    tmpRotation;
    std::string fileName;
    GLuint      text;
    Shader      *m_shader_color;
    GLint       m_uniLoc_projection2, m_uniLoc_modelView2;
    std::vector<GLfloat>  m_tmpColor;
  };
}

#endif // GRAPHICFILE_HPP
