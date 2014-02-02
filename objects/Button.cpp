#include "Plane.hpp"
#include "Text.hpp"
#include "Button.hpp"
#include "../MyWindow.hpp"
#include "../String_utils.hpp"
#include "../HandleError.hpp"
#include "../Utils/MyException.hpp"
#include <cmath>
#include "../shaders/ShaderHandler.hpp"

#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

using namespace Object;

void    Button::createButton(std::string &t, Color &c, float &width, float &height, float &font_size)
{
  this->setRender2D(true);
  this->height = height;
  this->width = width;
  //m_buttonText = new Text(t, m_color, (m_pos.x() + width) / 2.f - (0.017f * t.length()) / 2.f, (m_pos.y() + height) / 2.f - font_size / 2.f, font_size);
  float tmp = 0.017f * t.length();
  m_buttonText = new Text(t, m_color, m_pos.x() + (width / 2.f - tmp / 2.f),
                          m_pos.y() + (height / 2.f - font_size / 2.f), font_size);
  m_color = c;
}

Button::Button(string text, Color c, Color background_color, Vector3D v, float width, float height, float font_size)
  : myGLWidget(v, Rotation(), c)
{
  this->createButton(text, background_color, width, height, font_size);
}

Button::Button(const char *text, Color c, Color background_color, Vector3D v, float width, float height, float font_size)
  : myGLWidget(v, Rotation(), c)
{
  std::string s;

  if (!text)
    throw MyException("Invalid null message in Button");
  s = text;
  this->createButton(s, background_color, width, height, font_size);
}


Button::~Button()
{
  delete m_buttonText;
}

void    Button::initializeGL()
{
  m_buttonText->initializeGL();

  std::string vert, frag;

  if (m_hasTexture) {
      vert =
          "#version 150 core\n"

          "in vec2 in_Vertex;\n"
          "in vec2 in_TexCoord0;\n"
          "out vec2 coordTexture;\n"

          "uniform mat4 projection;\n"

          "void main(){\n"
          "gl_Position.xy = in_Vertex;\n"
          "coordTexture = in_TexCoord0;\n"
          "}";
      frag =
          "#version 150 core\n"

          "in vec2 coordTexture;\n"
          "uniform sampler2D tex;\n"
          "out vec4 out_Color;\n"

          "void main(){\n"
          "out_Color = texture(tex, coordTexture);\n"
          "}\n";
    } else {
      vert =
          "#version 150 core\n"

          "in vec2 in_Vertex;\n"
          "in vec3 in_Color;\n"

          "out vec3 color;\n"

          "void main(){\n"
          "gl_Position.xy = in_Vertex;\n"
          "color = in_Color;\n"
          "}";
      frag =
          "#version 150 core\n"

          "out vec4 out_Color;\n"
          "in vec3 color;\n"

          "void main(){\n"
          "out_Color = vec4(color, 1.0);"
          "}\n";
    }
  m_shader = ShaderHandler::getInstance()->createShader(vert, frag);
  if (!m_shader){
      HandleError::showError("Shader didn't load in Button");
      exit(-1);
    }
  m_uniLoc_projection = glGetUniformLocation(m_shader->getProgramID(), "projection");
  m_uniLoc_modelView = glGetUniformLocation(m_shader->getProgramID(), "modelview");

  /*GLfloat verticesTmp[] = {
        m_pos.x() + this->width / 13.f, m_pos.y(),
        m_pos.x() + this->width / 17.f, m_pos.y() + this->height / 13.f,
        m_pos.x(), m_pos.y() + this->height / 17.f,
        m_pos.x() + this->width, m_pos.y(),
        m_pos.x() + this->width, m_pos.y() + this->height,
        m_pos.x(), m_pos.y() + this->height
    };*/
  GLfloat angle = (this->width < this->height ? this->width / 15.f : this->height / 15.f);
  GLfloat verticesTmp[] = {
    m_pos.x() + angle, m_pos.y(),
    m_pos.x(), m_pos.y() + angle,
    m_pos.x(), m_pos.y() + (this->height - angle),
    m_pos.x() + angle, m_pos.y() + this->height,
    m_pos.x() + (this->width - angle), m_pos.y() + this->height,
    m_pos.x() + this->width, m_pos.y() + (this->height - angle),
    m_pos.x() + this->width, m_pos.y() + angle,
    m_pos.x() + (this->width - angle), m_pos.y()
  };

  for (unsigned int i(0); i < sizeof(verticesTmp) / sizeof(verticesTmp[0]); ++i){
      m_vertices.push_back(verticesTmp[i]);
    }
  if (m_hasTexture)
    {
      GLfloat h = (m_texture.isRepeating() ? this->height / 10.f : 1.f);
      GLfloat w = (m_texture.isRepeating() ? this->width / 10.f : 1.f);

      GLfloat tex[] = {0.f, 0.f,
                       w, 0.f,
                       0.f, h,
                       w, h};
      for (unsigned int i = 0; i < sizeof(tex) / sizeof(tex[0]); ++i){
          m_textures.push_back(tex[i]);
        }
    }
  else
    {
      for (unsigned int i = 0; i < m_vertices.size() / 2; ++i){
          m_couleurs.push_back(m_color.red());
          m_couleurs.push_back(m_color.green());
          m_couleurs.push_back(m_color.blue());
        }
    }

  m_pointsNumber = m_vertices.size() / 2;

  this->initVertexBufferObject();
  this->initVertexArrayObject();
}

void    Button::paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix)
{
  // Activation du shader
  glUseProgram(m_shader->getProgramID());

  glBindVertexArray(m_vaoID);

  // Rendu
  if (m_hasTexture)
    m_texture.bind();
  glDrawArrays(GL_POLYGON, 0, m_pointsNumber);

  glBindVertexArray(0);
  glUseProgram(0);

  m_buttonText->paintGL(view_matrix, proj_matrix);
}

string Button::getClassName() const
{
  return std::string("Button");
}
