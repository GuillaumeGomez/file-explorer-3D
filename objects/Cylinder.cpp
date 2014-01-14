#include "Cylinder.hpp"
#include "../HandleError.hpp"
#include "../Camera.hpp"
#include "../String_utils.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>

using namespace Object;

#define CACHE_SIZE	240

Cylinder::Cylinder(Vector3D p, Rotation r, Color c, float rayon, float height)
  : myGLWidget(p, r, c), m_rayon(rayon), m_height(height), m_uSlices(32)
{
  m_shader = new Shader;
  m_pointsNumber = 0;
}

Cylinder::Cylinder(Vector3D p, Rotation r, std::string tex, float rayon, float height)
  : myGLWidget(p, r, tex), m_rayon(rayon), m_height(height), m_uSlices(32)
{
  m_shader = new Shader;
  m_pointsNumber = 0;
}

void Cylinder::initializeGL()
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

  GLint i,j;
  GLfloat sinCache[CACHE_SIZE];
  GLfloat cosCache[CACHE_SIZE];
  GLfloat sinCache2[CACHE_SIZE];
  GLfloat cosCache2[CACHE_SIZE];
  GLfloat angle;
  GLfloat zLow, zHigh;
  GLfloat length;
  GLfloat deltaRadius;
  GLfloat zNormal;
  GLfloat xyNormalRatio;
  GLfloat radiusLow, radiusHigh;
  GLfloat baseRadius(m_rayon), topRadius(m_rayon);
  GLint  slices(32), stacks(32);
  std::vector<Vector3D>   tmp_v, tmp_t;

  slices = m_uSlices;

  if (slices >= CACHE_SIZE)
    slices = CACHE_SIZE - 1;

  if (slices < 2 || stacks < 1 || baseRadius < 0.0 || topRadius < 0.0 ||
      m_height < 0.0) {
      HandleError::showError("Invalid cylinder parameter");
      exit(0);
    }

  deltaRadius = 0; // top_radius - bottom_radius
  length = sqrtf(deltaRadius * deltaRadius + m_height * m_height);
  if (length == 0.0) {
      HandleError::showError("Invalid length return value in cylinder");
      exit(0);
    }

  zNormal = deltaRadius / length;
  xyNormalRatio = m_height / length;

  for (i = 0; i < slices; i++) {
      angle = 2 * M_PI * i / slices;
      sinCache2[i] = xyNormalRatio * sinf(angle);
      cosCache2[i] = xyNormalRatio * cosf(angle);
      sinCache[i] = sinf(angle);
      cosCache[i] = cosf(angle);
    }

  sinCache[slices] = sinCache[0];
  cosCache[slices] = cosCache[0];
  sinCache2[slices] = sinCache2[0];
  cosCache2[slices] = cosCache2[0];

  for (j = 0; j < stacks; j++) {
      zLow = j * m_height / stacks;
      zHigh = (j + 1) * m_height / stacks;
      radiusLow = baseRadius - deltaRadius * ((float) j / stacks);
      radiusHigh = baseRadius - deltaRadius * ((float) (j + 1) / stacks);

      for (i = 0; i <= slices; i++) {
          if (m_hasTexture){
              tmp_t.push_back(Vector3D(1 - (float) i / slices, (float) j / stacks));
            }
          tmp_v.push_back(Vector3D(radiusLow * sinCache[i], radiusLow * cosCache[i], zLow));

          if (m_hasTexture){
              tmp_t.push_back(Vector3D(1 - (float) i / slices, (float) (j+1) / stacks));
            }
          tmp_v.push_back(Vector3D(radiusHigh * sinCache[i], radiusHigh * cosCache[i], zHigh));
        }
    }
  m_hasTexture ? this->convertTRIANGLE_STRIP_To_TRIANGLES(tmp_v, tmp_t) : this->convertTRIANGLE_STRIP_To_TRIANGLES(tmp_v);

  m_pointsNumber = m_vertices.size() / 3;

  this->initVertexBufferObject();
  this->initVertexArrayObject();
}

void  Cylinder::initializeGLNoList()
{

}

void Cylinder::paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix)
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

std::string Cylinder::getClassName() const
{
  return std::string("Cylinder");
}
