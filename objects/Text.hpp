#ifndef TEXT_HPP
#define TEXT_HPP

#include <string>
#include "../myGLWidget.hpp"

namespace Object
{
  class Text : public myGLWidget
  {
  public:
    Text(std::string text, Color = Color(), float x = 0.f, float y = 0.f, float font_size = 0.2f);
    Text(const char *text, Color = Color(), float x = 0.f, float y = 0.f, float font_size = 0.2f);
    Text(std::string text, Color = Color(), Vector3D = Vector3D(), Rotation = Rotation(), float font_size = 0.2f);
    Text(const char *text, Color = Color(), Vector3D = Vector3D(), Rotation = Rotation(), float font_size = 0.2f);
    Text(GLuint tex_id, float text_length, Vector3D v, Rotation r, float font_size = 0.2f);
    virtual ~Text();
    virtual void    initializeGL();
    virtual void    initializeGLNoList();
    virtual void    paintGL(const glm::mat4& view_matrix, const glm::mat4& proj_matrix);
    void            setText(std::string);
    void            setText(const char*);
    GLfloat         getSize() const;

  protected:
    std::string m_text;
    float       m_font_size;
    float       m_size;

    static Shader   *s_m_shader;
    static GLint    s_m_uniLoc_projection;
    static GLint    s_m_uniLoc_modelView;
  };
}

#endif // TEXT_HPP
