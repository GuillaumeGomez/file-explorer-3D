#include "Sphere.hpp"
#include "../HandleError.hpp"
#include <cmath>
#include <cstdlib>
#include "../Camera.hpp"

using namespace Object;

#define CACHE_SIZE  240

Sphere::Sphere(Vector3D p, Rotation r, Color c, float rayon) : myGLWidget(p, r, c), m_rayon(rayon)
{
  m_pickAllow = true;
  m_shader = new Shader;
  m_pointsNumber = 0;
}

Sphere::Sphere(Vector3D p, Rotation r, std::string tex, float rayon) : myGLWidget(p, r, tex), m_rayon(rayon)
{
  m_pickAllow = true;
  m_shader = new Shader;
  m_pointsNumber = 0;
}

void Sphere::initializeGL()
{
  loadTexture();

  std::string vert;
  std::string frag;

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
          "void main()\n"
          "{\n"
          "gl_Position = projection * modelview * vec4(in_Vertex, 1.0);\n"
          // Envoi de la couleur au Fragment Shader

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

  m_shader->setVertexSource(vert);
  m_shader->setFragmentSource(frag);
  if (!m_shader->load()){
      HandleError::showError("Shader didn't load in Cylinder");
      exit(-1);
    }
  m_uniLoc_projection = glGetUniformLocation(m_shader->getProgramID(), "projection");
  m_uniLoc_modelView = glGetUniformLocation(m_shader->getProgramID(), "modelview");

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
  m_hasTexture ? this->convertTRIANGLE_STRIP_To_TRIANGLES(tmp_v, tmp_t) : this->convertTRIANGLE_STRIP_To_TRIANGLES(tmp_v);
  m_pointsNumber = m_vertices.size() / 3;

  this->initVertexBufferObject();
  this->initVertexArrayObject();
}

void  Sphere::initializeGLNoList()
{
}

void  Sphere::paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix)
{
  // Activation du shader
  glUseProgram(m_shader->getProgramID());

  glBindVertexArray(m_vaoID);

  // Envoi des matrices
  glUniformMatrix4fv(m_uniLoc_projection, 1, GL_FALSE, glm::value_ptr(proj_matrix));

  glm::mat4 tmp = glm::translate(view_matrix, glm::vec3(m_pos.x(), m_pos.y(), m_pos.z()));

  if (m_rot.rotation() != 0.f && (m_rot.x() != 0.f || m_rot.y() != 0.f || m_rot.z() != 0.f))
    tmp = glm::rotate(tmp, m_rot.rotation(), glm::vec3(m_rot.x(), m_rot.y(), m_rot.z()));
  glUniformMatrix4fv(m_uniLoc_modelView, 1, GL_FALSE, glm::value_ptr(tmp));

  // Rendu
  if (m_hasTexture){
      m_texture.bind();
      glDrawArrays(GL_TRIANGLES, 0, m_pointsNumber);
      m_texture.unbind();
    }
  else{
      glDrawArrays(GL_TRIANGLES, 0, m_pointsNumber);
    }

  glBindVertexArray(0);

  // Desactivation du shader
  glUseProgram(0);
}

string Sphere::getClassName() const
{
  return std::string("Sphere");
}
