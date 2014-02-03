#include "Line.hpp"
#include "../HandleError.hpp"
#include "../shaders/ShaderHandler.hpp"

using namespace Object;

Line::Line(Vector3D from, Vector3D to, Color c) : myGLWidget(from, Rotation(), c), m_to(to)
{
}

Line::~Line()
{
}

void Line::initializeGL()
{
  std::string vert = "#version 150 core\n"
      "in vec3 in_Vertex;\n"
      "in vec3 in_Color;\n"

      "uniform mat4 projection;\n"
      "uniform mat4 modelview;\n"

      "out vec3 color;\n"
      "void main(){\n"
      "gl_Position = projection * modelview * vec4(in_Vertex, 1.0);\n"

      "color = in_Color;\n"
      "}";
  m_shader = ShaderHandler::getInstance()->createShader(vert, Shader::getStandardFragmentShader(false));
  if (!m_shader){
      HandleError::showError("Shader didn't load in Line");
      exit(-1);
    }
  m_uniLoc_projection = glGetUniformLocation(m_shader->getProgramID(), "projection");
  m_uniLoc_modelView = glGetUniformLocation(m_shader->getProgramID(), "modelview");

  m_vertices.push_back(m_pos.x());
  m_vertices.push_back(m_pos.y());
  m_vertices.push_back(m_pos.z());
  m_vertices.push_back(m_to.x());
  m_vertices.push_back(m_to.y());
  m_vertices.push_back(m_to.z());

  for (unsigned int i = 0; i < m_vertices.size(); ++i){
      m_couleurs.push_back(m_color.red());
      m_couleurs.push_back(m_color.green());
      m_couleurs.push_back(m_color.blue());
    }

  m_pointsNumber = m_vertices.size() / 3.f;

  this->initVertexBufferObject();
  this->initVertexArrayObject();
}

void  Line::paintGL(const glm::mat4& view_matrix, const glm::mat4& proj_matrix)
{
  // Activation du shader
  glUseProgram(m_shader->getProgramID());

  glBindVertexArray(m_vaoID);

  glUniformMatrix4fv(m_uniLoc_projection, 1, GL_FALSE, glm::value_ptr(proj_matrix));
  glUniformMatrix4fv(m_uniLoc_modelView, 1, GL_FALSE, glm::value_ptr(view_matrix));

  glDrawArrays(GL_LINES, 0, m_pointsNumber);

  glBindVertexArray(0);
  glUseProgram(0);
}

std::string Line::getClassName() const
{
  return std::string("Line");
}
