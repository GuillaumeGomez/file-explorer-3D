#include "Plane.hpp"
#include "../HandleError.hpp"
#include "../Camera.hpp"
#include <cmath>
#include <iostream>
#include "../shaders/ShaderHandler.hpp"

using namespace Object;

Plane::Plane(Vector3D p, Rotation r, Color c, float width, float height, bool repeat)
  : myGLWidget(Vector3D(p.x(), p.y(), p.z()), r, c), width(width), height(height)
{
  m_texture.setRepeat(repeat);
  m_className = "Plane";
}

Plane::Plane(Vector3D p, Rotation r, std::string tex, float width, float height, bool repeat)
  : myGLWidget(Vector3D(p.x(), p.y(), p.z()), r, tex), width(width), height(height)
{
  m_texture.setRepeat(repeat);
  m_className = "Plane";
}

void Plane::initializeGL()
{
  loadTexture();

  std::string vert;
  std::string frag;

  if (!m_render2D) {
      if (m_hasTexture) {
          vert =
              "#version 150 core\n"

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
              "#version 150 core\n"

              "in vec2 coordTexture;\n"

              "uniform sampler2D tex;\n"

              "out vec4 out_Color;\n"

              "void main()\n"
              "{\n"
              "out_Color = texture(tex, coordTexture);\n"
              "}";
        } else {
          vert = "#version 150 core\n"
              "in vec3 in_Vertex;\n"
              "in vec3 in_Color;\n"

              "uniform mat4 projection;\n"
              "uniform mat4 modelview;\n"

              "out vec3 color;\n"
              "void main(){\n"
              "gl_Position = projection * modelview * vec4(in_Vertex, 1.0);\n"

              "color = in_Color;\n"
              "}";
          frag =
              "#version 150 core\n"

              "in vec3 color;\n"
              "out vec4 out_Color;\n"

              "void main()\n"
              "{\n"
              "out_Color = vec4(color, 1.0);\n"
              "}";
        }
    }
  if (!m_render2D)
    m_shader = ShaderHandler::getInstance()->createShader(vert, frag);
  else {
      std::string vert;

      if (m_hasTexture) {
          vert =
              "#version 330\n"

              "in vec2 in_Vertex;\n"
              "in vec2 in_TexCoord0;\n"
              "uniform vec2 in_pos;\n"

              "out vec2 coordTexture;\n"
              "void main()\n"
              "{\n"
              "gl_Position.xy = vec2(in_Vertex.x + in_pos.x, in_Vertex.y + in_pos.y);\n"
              "coordTexture = in_TexCoord0;\n"
              "}";
        } else {
          vert =
              "#version 330\n"

              "in vec2 in_Vertex;\n"
              "in vec3 in_Color;\n"
              "uniform vec2 in_pos;\n"

              "out vec3 color;\n"
              "void main()\n"
              "{\n"
              "gl_Position.xy = vec2(in_Vertex.x + in_pos.x, in_Vertex.y + in_pos.y);\n"
              "color = in_Color;\n"
              "}";
        }
      if ((m_shader = ShaderHandler::createShader(vert, Shader::getStandard2DFragmentShader(m_hasTexture)))) {
          m_uniloc_pos = m_shader->getUniform("in_pos");
        }
    }
  if (!m_shader){
      HandleError::showError("Shader didn't load in Plane");
      exit(-1);
    }
  m_uniLoc_projection = glGetUniformLocation(m_shader->getProgramID(), "projection");
  m_uniLoc_modelView = glGetUniformLocation(m_shader->getProgramID(), "modelview");

  if (!m_render2D) {
      GLfloat verticesTmp[] = {
        this->width / -2.f, this->height / 2.f, 0.f,
        this->width / 2.f, this->height / 2.f, 0.f,
        this->width / -2.f, this->height / -2.f, 0.f,
        this->width / 2.f, this->height / -2.f, 0.f
      };
      for (unsigned int i(0); i < sizeof(verticesTmp) / sizeof(verticesTmp[0]); ++i){
          m_vertices.push_back(verticesTmp[i]);
        }
    } else {
      GLfloat verticesTmp[] = {
        this->width / -2.f, this->height / 2.f,
        this->width / 2.f, this->height / 2.f,
        this->width / -2.f, this->height / -2.f,
        this->width / 2.f, this->height / -2.f
      };
      for (unsigned int i(0); i < sizeof(verticesTmp) / sizeof(verticesTmp[0]); ++i){
          m_vertices.push_back(verticesTmp[i]);
        }
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
      GLfloat col[] = {m_color.red(), m_color.green(), m_color.blue(),
                       m_color.red(), m_color.green(), m_color.blue(),
                       m_color.red(), m_color.green(), m_color.blue(),
                       m_color.red(), m_color.green(), m_color.blue()};

      for (unsigned int i = 0; i < sizeof(col) / sizeof(col[0]); ++i){
          m_colors.push_back(col[i]);
        }
    }

  m_pointsNumber = m_vertices.size() / (m_render2D ? 2 : 3);

  this->initVertexBufferObject();
  this->initVertexArrayObject();
}

void  Plane::paintGL(const glm::mat4& view_matrix, const glm::mat4& proj_matrix)
{
  // Activation du shader
  m_shader->bind();

  glBindVertexArray(m_vaoID);

  // Envoi des matrices
  if (!m_render2D) {
      glUniformMatrix4fv(m_uniLoc_projection, 1, GL_FALSE, glm::value_ptr(proj_matrix));

      glm::mat4 tmp = glm::translate(view_matrix, glm::vec3(m_pos.x(), m_pos.y(), m_pos.z()));

      if (m_rot.rotation() != 0.f && (m_rot.x() != 0.f || m_rot.y() != 0.f || m_rot.z() != 0.f))
        tmp = glm::rotate(tmp, m_rot.rotation(), glm::vec3(m_rot.x(), m_rot.y(), m_rot.z()));
      glUniformMatrix4fv(m_uniLoc_modelView, 1, GL_FALSE, glm::value_ptr(tmp));
    } else {
      glUniform2fv(m_uniloc_pos, 1, m_pos.getDatas());
    }

  // Rendu
  if (m_hasTexture) {
      m_texture.bind();
      glDrawArrays(GL_TRIANGLE_STRIP, 0, m_pointsNumber);
      m_texture.unbind();
    } else {
      glDrawArrays(GL_TRIANGLE_STRIP, 0, m_pointsNumber);
    }

  glBindVertexArray(0);
  m_shader->unbind();
}

float const &Plane::getWidth() const
{
  return width;
}

float const &Plane::getHeight() const
{
  return height;
}
