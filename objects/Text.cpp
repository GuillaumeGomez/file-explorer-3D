#include "Text.hpp"
#include "../MyWindow.hpp"
#include "../String_utils.hpp"
#include "../HandleError.hpp"
#include <cmath>
#include "../shaders/ShaderHandler.hpp"

#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

using namespace Object;

Text::Text(string text, Color c, float x, float y, float font_size)
  : myGLWidget(Vector3D(x, y, 0.f), Rotation(), c), m_text(text), m_font_size(font_size)
{
  if (m_text.length() > 0) {
      MyWindow::getLib()->createTextTexture(m_text.c_str(), &m_texture, c);
    } else {
      m_hasTexture = true;
    }
  m_size = 0.017f * m_text.length() * font_size * 5.f;
  m_lines = Utility::numberOfOccurence<std::string>(m_text, "\n") + 1;
  m_render2D = true;
  m_className = "Text";
}

Text::Text(const char *text, Color c, float x, float y, float font_size)
  : myGLWidget(Vector3D(x, y, 0.f), Rotation(), c), m_text(text ? text : ""), m_font_size(font_size)
{
  if (m_text.length() > 0) {
      MyWindow::getLib()->createTextTexture(m_text.c_str(), &m_texture, c);
    } else {
      m_hasTexture = true;
    }
  m_size = 0.017f * m_text.length() * font_size * 5.f;
  m_lines = Utility::numberOfOccurence<std::string>(m_text, "\n") + 1;
  m_render2D = true;
  m_className = "Text";
}

Text::Text(string text, Color c, Vector3D v, Rotation r, float font_size)
  : myGLWidget(v, r, c), m_text(text), m_font_size(font_size)
{
  if (m_text.length() > 0) {
      MyWindow::getLib()->createTextTexture(m_text.c_str(), &m_texture, c);
    } else {
      m_hasTexture = true;
    }
  m_size = 5 * m_text.length();
  m_lines = Utility::numberOfOccurence<std::string>(m_text, "\n") + 1;
  m_className = "Text";
}

Text::Text(const char *text, Color c, Vector3D v, Rotation r, float font_size)
  : myGLWidget(v, r, c), m_text(text ? text : ""), m_font_size(font_size)
{
  if (m_text.length() > 0) {
      MyWindow::getLib()->createTextTexture(m_text.c_str(), &m_texture, c);
    } else {
      m_hasTexture = true;
    }
  m_size = 5 * m_text.length();
  m_lines = Utility::numberOfOccurence<std::string>(m_text, "\n") + 1;
  m_className = "Text";
}

Text::Text(Texture const &s, std::string text, Vector3D v, Rotation r, float font_size)
  : myGLWidget(v, r, s), m_text(text), m_font_size(font_size)
{
  m_size = m_text.length() * 5;
  m_lines = Utility::numberOfOccurence<std::string>(m_text, "\n") + 1;
  m_className = "Text";
}

Text::Text(Texture const &s, float length, int lines, Vector3D v, Rotation r, float font_size)
  : myGLWidget(v, r, s), m_text(""), m_font_size(font_size)
{
  m_size = length;
  m_lines = (lines <= 0 ? 1 : lines);
  m_className = "Text";
}

Text::Text(Texture const &s, std::string text, float x, float y, float font_size)
  : myGLWidget(Vector3D(x, y, 0.f), Rotation(), s), m_text(text), m_font_size(font_size)
{
  m_size = m_text.length() * 5;
  m_lines = Utility::numberOfOccurence<std::string>(m_text, "\n") + 1;
  m_className = "Text";
}

Text::Text(Texture const &s, float length, int lines, float x, float y, float font_size)
  : myGLWidget(Vector3D(x, y, 0.f), Rotation(), s), m_text(""), m_font_size(font_size)
{
  m_size = length;
  m_lines = (lines <= 0 ? 1 : lines);
  m_className = "Text";
}

Text::~Text()
{
}

const GLfloat &Text::getSize() const
{
  return m_size;
}

void    Text::initializeGL()
{
  std::string vert;
  std::string frag;

  if (m_render2D) {
      vert =
          "#version 330\n"

          "in vec2 in_Vertex;\n"
          "in vec2 in_TexCoord0;\n"
          "out vec2 coordTexture;\n"

          "uniform mat4 projection;\n"

          "void main(){\n"
          "gl_Position.xy = in_Vertex;\n"
          "coordTexture = in_TexCoord0;\n"
          "}";
      frag =
          "#version 330\n"

          "in vec2 coordTexture;\n"
          "uniform sampler2D tex;\n"
          "out vec4 out_Color;\n"

          "void main(){\n"
          "out_Color = texture(tex, coordTexture);\n"
          "}\n";
    } else {
      vert =
          "#version 330\n"

          "in vec3 in_Vertex;\n"
          "in vec2 in_TexCoord0;\n"

          "uniform mat4 projection;\n"
          "uniform mat4 modelview;\n"

          "out vec2 coordTexture;\n"

          "void main()\n"
          "{\n"
          "gl_Position = projection * modelview * vec4(in_Vertex, 1.0);\n"
          "coordTexture = in_TexCoord0;\n"
          "}";

      frag =
          "#version 330\n"

          "in vec2 coordTexture;\n"

          "uniform sampler2D tex;\n"

          "out vec4 out_Color;\n"

          "void main()\n"
          "{\n"
          "out_Color = texture(tex, coordTexture);\n"
          "}";
    }

  m_shader = ShaderHandler::getInstance()->createShader(vert, frag);
  if (!m_shader){
      HandleError::showError("Shader didn't load in Text");
      exit(-1);
    }
  m_uniLoc_projection = glGetUniformLocation(m_shader->getProgramID(), "projection");
  m_uniLoc_modelView = glGetUniformLocation(m_shader->getProgramID(), "modelview");

  if (m_render2D) {
      this->fill2DVertices();
    } else {
      GLfloat tmp = m_font_size * m_lines + m_font_size * (m_lines - 1);

      GLfloat verticesTmp[] = {
        m_size / -2.f, tmp / 2.f, 0.f,
        m_size / 2.f, tmp / 2.f, 0.f,
        m_size / -2.f, tmp / -2.f, 0.f,
        m_size / 2.f, tmp / -2.f, 0.f
      };
      for (unsigned int i(0); i < sizeof(verticesTmp) / sizeof(verticesTmp[0]); ++i){
          m_vertices.push_back(verticesTmp[i]);
        }
    }
  GLfloat coordTextureTmp[] = {0.f, 0.f,
                               1.f, 0.f,
                               0.f, 1.f,
                               1.f, 1.f};

  for (unsigned int i(0); i < sizeof(coordTextureTmp) / sizeof(coordTextureTmp[0]); ++i){
      m_textures.push_back(coordTextureTmp[i]);
    }
  if (!m_render2D)
    m_pointsNumber = m_vertices.size() / 3;
  else
    m_pointsNumber = m_vertices.size() / 2;

  this->initVertexBufferObject(GL_DYNAMIC_DRAW);
  this->initVertexArrayObject();
}

void    Text::paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix)
{
  // Activation du shader
  glUseProgram(m_shader->getProgramID());

  glBindVertexArray(m_vaoID);

  // Envoi des matrices
  if (!m_render2D) {
      glUniformMatrix4fv(m_uniLoc_projection, 1, GL_FALSE, glm::value_ptr(proj_matrix));
      glm::mat4 tmp = glm::translate(view_matrix, glm::vec3(m_pos.x(), m_pos.y(), m_pos.z()));

      glUniformMatrix4fv(m_uniLoc_modelView, 1, GL_FALSE, glm::value_ptr(tmp));
    }

  m_texture.bind();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, m_pointsNumber);
  m_texture.unbind();

  glBindVertexArray(0);
  glUseProgram(0);
}

void  Text::fill2DVertices(bool recalc)
{
  if (recalc) {
      /*std::vector<std::string> v = Utility::split<std::string>(m_text, "\n");
      m_size = 0.f;

      if (v.size() > 0) {
          for (auto it = v.begin(); it != v.end(); ++it) {
              GLfloat tmp_size = 0.017f * (*it).length();

              if (tmp_size > m_size)
                m_size = tmp_size;
            }
        } else {
          m_size = 0.017f * m_text.length();
        }*/
    }
  m_size = m_texture.width() / 1000.f;
  //GLfloat tmp_h = m_font_size * m_lines + m_font_size * (m_lines - 1);
  //GLfloat tmp_h = m_texture.height() / 600.f;
  GLfloat tmp_h = m_font_size * (m_texture.height() / 72);

  //GLfloat tmp_w = m_texture.width() / 800;

  //GLfloat old_tmp = m_font_size + m_pos.y();

  GLfloat verticesTmp[] = {m_pos.x(), tmp_h + m_pos.y(),
                           m_size + m_pos.x(), tmp_h + m_pos.y(),
                           m_pos.x(), m_pos.y(),
                           m_size + m_pos.x(), m_pos.y()};

  m_vertices.clear();
  unsigned int size(sizeof(verticesTmp) / sizeof(verticesTmp[0]));

  for (unsigned int i(0); i < size; ++i){
      m_vertices.push_back(verticesTmp[i]);
    }
}

void  Text::updateColors()
{
  if (m_text.size() > 0)
    MyWindow::getLib()->createTextTexture(m_text.c_str(), &m_texture, m_color);
}

void    Text::setText(const char *s)
{
  if (s && m_text != s) {
      m_texture.destroy();
      MyWindow::getLib()->createTextTexture(s, &m_texture, this->getColor());
      m_text = s;
      //m_texture.setTextureID(tex);

      m_lines = Utility::numberOfOccurence<std::string>(s, "\n") + 1;
      if (m_render2D) {
          this->fill2DVertices(true);
          this->updateVertexBufferObject(&m_vertices[0], m_vertices.size() * sizeof(m_vertices[0]), 0);
        }
    }
}

std::string const &Text::getText() const
{
  return m_text;
}

void  Text::setTexture(const Texture &t)
{
  myGLWidget::setTexture(t);
  if (m_render2D) {
      this->fill2DVertices(true);
      this->updateVertexBufferObject(&m_vertices[0], m_vertices.size() * sizeof(m_vertices[0]), 0);
    }
}

void    Text::setText(std::string s)
{
  this->setText(s.c_str());
}
