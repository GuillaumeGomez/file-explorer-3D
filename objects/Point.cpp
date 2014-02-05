#include "Point.hpp"
#include "shaders/ShaderHandler.hpp"
#include "HandleError.hpp"

using namespace Object;

Point::Point(Vector3D p, Color c) : myGLWidget(p, Rotation(), c)
{
}

Point::~Point()
{
}

void  Point::initializeGL()
{
  std::string vert =
      "#version 330\n"

      "in vec3 in_Vertex;\n"
      "in vec3 in_Color;\n"

      "uniform mat4 projection;\n"
      "uniform mat4 modelview;\n"
      "uniform vec3 _pos;\n"

      "mat4 my_translate(mat4 ori, vec3 t_pos){\n"
      "mat4 tmp = ori;\n"
      "tmp[3] = ori[0] * t_pos.x + ori[1] * t_pos.y + ori[2] * t_pos.z + ori[3];\n"
      "return tmp;\n}\n"

      "out vec3 color;\n"
      "void main(){\n"
      "gl_Position = projection * my_translate(modelview, in_Vertex) * vec4(in_Vertex, 1.0);\n"

      "color = in_Color;\n"
      "}";
  if (!(m_shader = ShaderHandler::getInstance()->createShader(vert, Shader::getStandardFragmentShader(false)))) {
      HandleError::showError("Shader didn't load in Point class");
      exit(-1);
    }

  m_uniLoc_projection = glGetUniformLocation(m_shader->getProgramID(), "projection");
  m_uniLoc_modelView = glGetUniformLocation(m_shader->getProgramID(), "modelview");
  m_uniloc_pos = glGetUniformLocation(m_shader->getProgramID(), "_pos");

  m_vertices.push_back(0.f);
  m_vertices.push_back(0.f);
  m_vertices.push_back(0.f);

  m_couleurs.push_back(m_color.red());
  m_couleurs.push_back(m_color.green());
  m_couleurs.push_back(m_color.blue());

  m_pointsNumber = 1;

  this->initVertexBufferObject();
  this->initVertexArrayObject();
}

void  Point::paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix)
{
  glUseProgram(m_shader->getProgramID());

  glBindVertexArray(m_vaoID);

  glUniformMatrix4fv(m_uniLoc_projection, 1, GL_FALSE, glm::value_ptr(proj_matrix));
  glUniformMatrix4fv(m_uniLoc_modelView, 1, GL_FALSE, glm::value_ptr(view_matrix));
  glUniform3fv(m_uniloc_pos, 1, glm::value_ptr(glm::vec3(m_pos.x(), m_pos.y(), m_pos.z())));

  glDrawArrays(GL_POINTS, 0, m_pointsNumber);

  glBindVertexArray(0);
  glUseProgram(0);
}
