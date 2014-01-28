#include "Rectangle.hpp"
#include <cstring>
#include <cmath>

#include "../String_utils.hpp"
#include "../HandleSDL.hpp"
#include "../HandleError.hpp"
#include "../Camera.hpp"
#include "../shaders/ShaderHandler.hpp"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace Object;

Rectangle::Rectangle(Vector3D p, Rotation r, Color c, float sizex, float sizey, float sizez)
  : myGLWidget(p, r, c), sizeX(sizex), sizeY(sizey), sizeZ(sizez)
{
}

Rectangle::Rectangle(Vector3D p, Rotation r, std::string tex, float sizex, float sizey, float sizez)
  : myGLWidget(p, r, tex), sizeX(sizex), sizeY(sizey), sizeZ(sizez)
{
}

void Rectangle::initializeGL()
{
  loadTexture();

  std::string vert = Shader::getStandardVertexShader(m_hasTexture);
  std::string frag = Shader::getStandardFragmentShader(m_hasTexture);

  m_shader = ShaderHandler::getInstance()->createShader(vert, frag);
  if (!m_shader){
      HandleError::showError("Shader didn't load in Rectangle");
      exit(-1);
    }
  m_uniLoc_projection = glGetUniformLocation(m_shader->getProgramID(), "projection");
  m_uniLoc_modelView = glGetUniformLocation(m_shader->getProgramID(), "modelview");
  m_uniloc_rot = glGetUniformLocation(m_shader->getProgramID(), "_rot");
  m_uniloc_pos = glGetUniformLocation(m_shader->getProgramID(), "_pos");

  sizeX /= 2.f;
  sizeY /= 2.f;
  sizeZ /= 2.f;

  GLfloat verticesTmp[] = {-sizeX, -sizeY, -sizeZ,
                           sizeX, -sizeY, -sizeZ,
                           sizeX, sizeY, -sizeZ,
                           sizeX, sizeY, -sizeZ,
                           -sizeX, sizeY, -sizeZ,
                           -sizeX, -sizeY, -sizeZ,

                           -sizeX, -sizeY, sizeZ,
                           sizeX, -sizeY, sizeZ,
                           sizeX, sizeY, sizeZ,
                           sizeX, sizeY, sizeZ,
                           -sizeX, sizeY, sizeZ,
                           -sizeX, -sizeY, sizeZ,

                           -sizeX, sizeY, sizeZ,
                           -sizeX, sizeY, -sizeZ,
                           -sizeX, -sizeY, -sizeZ,
                           -sizeX, -sizeY, -sizeZ,
                           -sizeX, -sizeY, sizeZ,
                           -sizeX, sizeY, sizeZ,

                           sizeX, sizeY, sizeZ,
                           sizeX, sizeY, -sizeZ,
                           sizeX, -sizeY, -sizeZ,
                           sizeX, -sizeY, -sizeZ,
                           sizeX, -sizeY, sizeZ,
                           sizeX, sizeY, sizeZ,

                           -sizeX, -sizeY, -sizeZ,
                           sizeX, -sizeY, -sizeZ,
                           sizeX, -sizeY, sizeZ,
                           sizeX, -sizeY, sizeZ,
                           -sizeX, -sizeY, sizeZ,
                           -sizeX, -sizeY, -sizeZ,

                           -sizeX, sizeY, -sizeZ,
                           sizeX, sizeY, -sizeZ,
                           sizeX, sizeY, sizeZ,
                           sizeX, sizeY, sizeZ,
                           -sizeX, sizeY, sizeZ,
                           -sizeX, sizeY, -sizeZ
                          };

  for (unsigned int i(0); i < sizeof(verticesTmp) / sizeof(verticesTmp[0]); ++i){
      m_vertices.push_back(verticesTmp[i]);
    }

  if (!m_hasTexture){

      unsigned int size = m_vertices.size() / 3;
      for (unsigned int i(0); i < size; ++i){
          m_couleurs.push_back(m_color.red());
          m_couleurs.push_back(m_color.green());
          m_couleurs.push_back(m_color.blue());
        }
    }

  else{
      GLfloat coordTextureTmp[] = {0.0f, 0.0f,
                                   1.0f, 0.0f,
                                   1.0f, 1.0f,
                                   1.0f, 1.0f,
                                   0.0f, 1.0f,
                                   0.0f, 0.0f,

                                   0.0f, 0.0f,
                                   1.0f, 0.0f,
                                   1.0f, 1.0f,
                                   1.0f, 1.0f,
                                   0.0f, 1.0f,
                                   0.0f, 0.0f,

                                   0.0f, 1.0f,
                                   1.0f, 1.0f,
                                   1.0f, 0.0f,
                                   1.0f, 0.0f,
                                   0.0f, 0.0f,
                                   0.0f, 1.0f,

                                   0.0f, 1.0f,
                                   1.0f, 1.0f,
                                   1.0f, 0.0f,
                                   1.0f, 0.0f,
                                   0.0f, 0.0f,
                                   0.0f, 1.0f,

                                   0.0f, 1.0f,
                                   1.0f, 1.0f,
                                   1.0f, 0.0f,
                                   1.0f, 0.0f,
                                   0.0f, 0.0f,
                                   0.0f, 1.0f,

                                   0.0f, 1.0f,
                                   1.0f, 1.0f,
                                   1.0f, 0.0f,
                                   1.0f, 0.0f,
                                   0.0f, 0.0f,
                                   0.0f, 1.0f
                                  };

      for (unsigned int i(0); i < sizeof(coordTextureTmp) / sizeof(coordTextureTmp[0]); ++i){
          m_textures.push_back(coordTextureTmp[i]);
        }
    }

  m_pointsNumber = m_vertices.size() / 3;

  this->initVertexBufferObject();
  this->initVertexArrayObject();
}

void  Rectangle::paintGL(const glm::mat4& view_matrix, const glm::mat4& proj_matrix)
{
  // Activation du shader
  glUseProgram(m_shader->getProgramID());

  glBindVertexArray(m_vaoID);

  // Envoi des matrices
  glUniformMatrix4fv(m_uniLoc_projection, 1, GL_FALSE, glm::value_ptr(proj_matrix));

  glUniformMatrix4fv(m_uniLoc_modelView, 1, GL_FALSE, glm::value_ptr(view_matrix));
  glUniform3fv(m_uniloc_pos, 1, glm::value_ptr(glm::vec3(m_pos.x(), m_pos.y(), m_pos.z())));
  //if (m_rot.rotation() != 0.f && (m_rot.x() != 0.f || m_rot.y() != 0.f || m_rot.z() != 0.f))
    glUniform4fv(m_uniloc_rot, 1, glm::value_ptr(glm::vec4(m_rot.x(), m_rot.y(), m_rot.z(), m_rot.rotation())));

  if (m_hasTexture){
      m_texture.bind();
      glDrawArrays(m_drawMode, 0, m_pointsNumber);
      m_texture.unbind();
    }
  else{
      glDrawArrays(m_drawMode, 0, m_pointsNumber);
    }

  glBindVertexArray(0);
  glUseProgram(0);
}

string Rectangle::getClassName() const
{
  return string("Rectangle");
}
