#include "GraphicFile.hpp"
#include "../Utils/MyException.hpp"
#include "../Utils/Utils.hpp"
#include "Text.hpp"
#include "../HandleError.hpp"
#include "../shaders/ShaderHandler.hpp"

#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

using namespace Object;

GraphicFile::GraphicFile(Vector3D v, Rotation r, Color c, const char *fileName)
  : Rectangle(v, r, c, 1.5f, 2.f, 0.3f), tmpRotation(m_rot), fileName(fileName ? fileName : "")
{
  if (this->fileName.empty())
    throw MyException("GraphicFile: File name can't be empty");

  tmpRotation = m_rot;
  tmpRotation.setSpeed(0.f);
  tmpRotation.setRotX(1.f);
}

GraphicFile::GraphicFile(Vector3D v, Rotation r, Color c, std::string fileName)
  : Rectangle(v, r, c, 15.f, 20.f, 2.f), tmpRotation(m_rot), fileName(fileName)
{
  if (this->fileName.empty())
    throw MyException("GraphicFile: File name can't be empty");

  tmpRotation = m_rot;
  tmpRotation.setSpeed(10.f);
  tmpRotation.setRotX(1.f);
}

GraphicFile::~GraphicFile()
{
  ShaderHandler::getInstance()->destroyShader(m_shader_color);
}

std::string const &GraphicFile::getFileName() const
{
  return fileName;
}

void  GraphicFile::initializeGL()
{
  GLuint  tex;

  if ((tex = mainWindow->getLib()->loadIconFile(fileName)) > 0){
      m_texture.setTextureID(tex);
      m_hasTexture = true;
    }
  std::string vert;
  std::string frag;

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
  m_shader = ShaderHandler::getInstance()->createShader(vert, frag);
  if (!m_shader){
      HandleError::showError("Shader didn't load in GraphicFile");
      exit(-1);
    }
  m_uniLoc_projection = glGetUniformLocation(m_shader->getProgramID(), "projection");
  m_uniLoc_modelView = glGetUniformLocation(m_shader->getProgramID(), "modelview");
  vert = "#version 150 core\n"
      "in vec3 in_Vertex;\n"
      "in vec3 in_Color;\n"

      "uniform mat4 projection;\n"
      "uniform mat4 modelview;\n"

      "out vec3 color;\n"
      "void main()\n"
      "{\n"
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
  m_shader_color = ShaderHandler::getInstance()->createShader(vert, frag);
  if (!m_shader_color){
      HandleError::showError("Shader didn't load in GraphicFile");
      exit(-1);
    }
  m_uniLoc_projection2 = glGetUniformLocation(m_shader_color->getProgramID(), "projection");
  m_uniLoc_modelView2 = glGetUniformLocation(m_shader_color->getProgramID(), "modelview");
  //MyWindow::createTextTexture(this->fileName.c_str(), &text, 0, RED);
  //m_glObject = glGenLists(1);
  //glNewList(m_glObject, GL_COMPILE);
  //this->initializeGLNoList();
  //glEndList();

  sizeX /= 2.f;
  sizeY /= 2.f;
  sizeZ /= 2.f;

  GLfloat tmpVertices[] = {-sizeX, -sizeY, -sizeZ, sizeX, -sizeY, -sizeZ, sizeX, -sizeY, sizeZ,
                           sizeX, -sizeY, sizeZ, -sizeX, -sizeY, sizeZ, -sizeX, -sizeY, -sizeZ,

                           -sizeX, sizeY, -sizeZ, sizeX, sizeY, -sizeZ,  sizeX, sizeY, sizeZ,
                           sizeX, sizeY, sizeZ, -sizeX, sizeY, sizeZ, -sizeX, sizeY, -sizeZ,

                           -sizeX, sizeY, sizeZ, -sizeX, sizeY, -sizeZ, -sizeX, -sizeY, -sizeZ,
                           -sizeX, -sizeY, -sizeZ, -sizeX, -sizeY, sizeZ, -sizeX, sizeY, sizeZ,

                           sizeX, sizeY, sizeZ, sizeX, sizeY, -sizeZ, sizeX, -sizeY, -sizeZ,
                           sizeX, -sizeY, -sizeZ, sizeX, -sizeY, sizeZ, sizeX, sizeY, sizeZ,

                           -sizeX, -sizeY, -sizeZ, sizeX, -sizeY, -sizeZ, sizeX, sizeY, -sizeZ,
                           sizeX, sizeY, -sizeZ, -sizeX, sizeY, -sizeZ, -sizeX, -sizeY, -sizeZ,

                           -sizeX, -sizeY, sizeZ, sizeX, -sizeY, sizeZ, sizeX, sizeY, sizeZ,
                           sizeX, sizeY, sizeZ, -sizeX, sizeY, sizeZ, -sizeX, -sizeY, sizeZ};

  GLfloat tmpTex[] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
                      0.f, 0.f, 0.f, 0.f, 0.f, 0.f,

                      0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
                      0.f, 0.f, 0.f, 0.f, 0.f, 0.f,

                      0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
                      0.f, 0.f, 0.f, 0.f, 0.f, 0.f,

                      0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
                      0.f, 0.f, 0.f, 0.f, 0.f, 0.f,

                      0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                      1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

                      0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                      1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f};

  GLfloat tmpCol[] = {m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(),
                      m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(),

                      m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(),
                      m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(),

                      m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(),
                      m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(),

                      m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(),
                      m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue(), m_color.red(), m_color.green(), m_color.blue()};

  for (unsigned int i(0); i < sizeof(tmpVertices) / sizeof(tmpVertices[0]); ++i) {
      m_vertices.push_back(tmpVertices[i]);
    }
  for (unsigned int i(0); i < sizeof(tmpTex) / sizeof(tmpTex[0]); ++i) {
      m_textures.push_back(tmpTex[i]);
    }
  for (unsigned int i(0); i < sizeof(tmpCol) / sizeof(tmpCol[0]); ++i) {
      m_colors.push_back(tmpCol[i]);
    }
  while (m_tmpColor.size() < m_colors.size()) {
      m_tmpColor.push_back(m_pickingColor.red());
      m_tmpColor.push_back(m_pickingColor.green());
      m_tmpColor.push_back(m_pickingColor.blue());
    }
  m_pointsNumber = m_vertices.size() / 3;

  this->initVertexBufferObject();
  this->initVertexArrayObject();
}

void  GraphicFile::initializeGLNoList()
{
}

void  GraphicFile::pick(const glm::mat4& view_matrix, const glm::mat4& proj_matrix)
{
  if (isPickingAllowed()) {
      Rotation tmp(m_rot);
      m_rot = tmpRotation;
      myGLWidget::pick(view_matrix, proj_matrix);
      m_rot = tmp;
    }
}

void  GraphicFile::paintGL(const glm::mat4& view_matrix, const glm::mat4& proj_matrix)
{
  if (!isSelected()){
      if (tmpRotation.rotation() > m_rot.rotation()){
          if (tmpRotation.rotation() > 360.f)
            tmpRotation.setRotation(int(tmpRotation.rotation()) % 360);
          tmpRotation.setSpeed(-40.f);
        }
      else {
          tmpRotation.setRotation(m_rot.x());
          tmpRotation.setSpeed(0.f);
        }
    } else {
      tmpRotation.setSpeed(10.f);
    }
  glm::mat4 tmp = glm::translate(view_matrix, glm::vec3(m_pos.x(), m_pos.y(), m_pos.z()));

  if (tmpRotation.rotation() != 0.f && (tmpRotation.x() != 0.f || tmpRotation.y() != 0.f || tmpRotation.z() != 0.f))
    tmp = glm::rotate(tmp, tmpRotation.rotation(), glm::vec3(tmpRotation.x(), tmpRotation.y(), tmpRotation.z()));

  glUseProgram(m_shader_color->getProgramID());

  glBindVertexArray(m_vaoID);

  // Envoi des matrices
  glUniformMatrix4fv(m_uniLoc_projection2, 1, GL_FALSE, glm::value_ptr(proj_matrix));
  glUniformMatrix4fv(m_uniLoc_modelView2, 1, GL_FALSE, glm::value_ptr(tmp));

  glDrawArrays(GL_TRIANGLES, 0, 24);

  glUseProgram(m_shader->getProgramID());
  glBindVertexArray(m_vaoID);

  glUniformMatrix4fv(m_uniLoc_projection, 1, GL_FALSE, glm::value_ptr(proj_matrix));
  glUniformMatrix4fv(m_uniLoc_modelView, 1, GL_FALSE, glm::value_ptr(tmp));

  m_texture.bind();
  glDrawArrays(GL_TRIANGLES, 24, 12);
  m_texture.unbind();

  glBindVertexArray(0);
  glUseProgram(0);
}

void    GraphicFile::update(const float &n)
{
  if (tmpRotation.rotation() != m_rot.rotation() || tmpRotation.speed() != 0.f)
    tmpRotation.update(n);
}

void  GraphicFile::setSelected(bool b)
{
  if (b == m_selected)
    return;
  if (b)
    this->updateVertexBufferObject(&m_tmpColor[0], m_colorsSize, m_verticesSize + m_texturesSize);
  else
    this->updateVertexBufferObject(&m_colors[0], m_colorsSize, m_verticesSize + m_texturesSize);
  m_selected = b;
}

std::string GraphicFile::getClassName() const
{
  return std::string("GraphicFile");
}
