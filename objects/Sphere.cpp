#include "Sphere.hpp"
#include "../HandleError.hpp"
#include <cmath>
#include <cstdlib>
#include "../Camera.hpp"
#include "../shaders/ShaderHandler.hpp"

using namespace Object;

#define CACHE_SIZE  240

Sphere::Sphere(Vector3D p, Rotation r, Color c, float rayon) : myGLWidget(p, r, c), m_rayon(rayon)
{
  m_pickAllow = true;
  m_pointsNumber = 0;
}

Sphere::Sphere(Vector3D p, Rotation r, std::string tex, float rayon) : myGLWidget(p, r, tex), m_rayon(rayon)
{
  m_pickAllow = true;
  m_pointsNumber = 0;
}

void Sphere::initializeGL()
{
  loadTexture();

  m_shader = ShaderHandler::getInstance()->createShader(Shader::getStandardVertexShader(m_hasTexture),
                                                        Shader::getStandardFragmentShader(m_hasTexture));
  if (!m_shader){
      HandleError::showError("Shader didn't load in Sphere");
      exit(-1);
    }
  m_uniLoc_projection = glGetUniformLocation(m_shader->getProgramID(), "projection");
  m_uniLoc_modelView = glGetUniformLocation(m_shader->getProgramID(), "modelview");
  m_uniloc_rot = glGetUniformLocation(m_shader->getProgramID(), "_rot");
  m_uniloc_pos = glGetUniformLocation(m_shader->getProgramID(), "_pos");

  GLint i,j, slices(32), stacks(32);
  GLfloat sinCache1a[CACHE_SIZE];
  GLfloat cosCache1a[CACHE_SIZE];
  GLfloat sinCache2a[CACHE_SIZE];
  GLfloat cosCache2a[CACHE_SIZE];
  GLfloat sinCache1b[CACHE_SIZE];
  GLfloat cosCache1b[CACHE_SIZE];
  //GLfloat sinCache2b[CACHE_SIZE];
  //GLfloat cosCache2b[CACHE_SIZE];
  GLfloat angle;
  GLfloat zLow, zHigh;
  GLfloat sinftemp1 = 0.0, sinftemp2 = 0.0;
  GLint start, finish;
  std::vector<Vector3D> tmp_v, tmp_t;

  if (slices >= CACHE_SIZE)
    slices = CACHE_SIZE - 1;
  if (stacks >= CACHE_SIZE)
    stacks = CACHE_SIZE - 1;
  if (slices < 2 || stacks < 1 || m_rayon < 0.0) {
      HandleError::showError("Invalid parameter for sphere");
      exit(0);
    }

  for (i = 0; i < slices; i++) {
      angle = 2 * M_PI * i / slices;
      sinCache1a[i] = sinf(angle);
      cosCache1a[i] = cosf(angle);

      sinCache2a[i] = sinCache1a[i];
      cosCache2a[i] = cosCache1a[i];
    }

  for (j = 0; j <= stacks; j++) {
      angle = M_PI * j / stacks;
      //sinCache2b[j] = sinf(angle);
      //cosCache2b[j] = cosf(angle);

      sinCache1b[j] = m_rayon * sinf(angle);
      cosCache1b[j] = m_rayon * cosf(angle);
    }
  /* Make sure it comes to a point */
  sinCache1b[0] = 0;
  sinCache1b[stacks] = 0;

  sinCache1a[slices] = sinCache1a[0];
  cosCache1a[slices] = cosCache1a[0];

  sinCache2a[slices] = sinCache2a[0];
  cosCache2a[slices] = cosCache2a[0];

  start = 0;
  finish = stacks;
  for (j = start; j < finish; j++) {
      zLow = cosCache1b[j];
      zHigh = cosCache1b[j + 1];
      sinftemp1 = sinCache1b[j];
      sinftemp2 = sinCache1b[j + 1];

      for (i = 0; i <= slices; i++) {
          if (m_hasTexture)
            tmp_t.push_back(Vector3D(1 - (float) i / slices, 1 - (float) (j + 1) / stacks));
          tmp_v.push_back(Vector3D(sinftemp2 * sinCache1a[i], sinftemp2 * cosCache1a[i], zHigh));

          if (m_hasTexture)
            tmp_t.push_back(Vector3D(1 - (float) i / slices, 1 - (float) j / stacks));
          tmp_v.push_back(Vector3D(sinftemp1 * sinCache1a[i], sinftemp1 * cosCache1a[i], zLow));
        }
    }
  for (Vector3D &it : tmp_v) {
      m_vertices.push_back(it.x());
      m_vertices.push_back(it.y());
      m_vertices.push_back(it.z());
    }
  if (m_hasTexture)
    for (Vector3D &it : tmp_t) {
        m_textures.push_back(it.x());
        m_textures.push_back(it.y());
      }
  else
    for (Vector3D &it : tmp_v) {
        m_colors.push_back(m_color.red());
        m_colors.push_back(m_color.green());
        m_colors.push_back(m_color.blue());
      }
  //m_hasTexture ? this->convertTRIANGLE_STRIP_To_TRIANGLES(tmp_v, tmp_t) : this->convertTRIANGLE_STRIP_To_TRIANGLES(tmp_v);
  m_pointsNumber = m_vertices.size() / 3;
  m_drawMode = GL_TRIANGLE_STRIP;

  this->initVertexBufferObject();
  this->initVertexArrayObject();
}

void  Sphere::initializeGLNoList()
{
}

void  Sphere::paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix)
{
  glUseProgram(m_shader->getProgramID());
  glBindVertexArray(m_vaoID);

  glUniformMatrix4fv(m_uniLoc_projection, 1, GL_FALSE, glm::value_ptr(proj_matrix));

  glUniformMatrix4fv(m_uniLoc_modelView, 1, GL_FALSE, glm::value_ptr(view_matrix));
  glUniform3fv(m_uniloc_pos, 1, glm::value_ptr(glm::vec3(m_pos.x(), m_pos.y(), m_pos.z())));
  glUniform4fv(m_uniloc_rot, 1, glm::value_ptr(glm::vec4(m_rot.x(), m_rot.y(), m_rot.z(), m_rot.rotation())));

  if (m_hasTexture)
    m_texture.bind();
  glDrawArrays(m_drawMode, 0, m_pointsNumber);

  glBindVertexArray(0);
  glUseProgram(0);
}

float const &Sphere::getRadius() const
{
  return m_rayon;
}

string Sphere::getClassName() const
{
  return std::string("Sphere");
}
