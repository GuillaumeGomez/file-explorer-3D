#include "Box.hpp"
#include "../Camera.hpp"
#include "../HandleError.hpp"

Box::Box(Vector3D v, Rotation r, Color c, float size) : myGLWidget(v, r, c), m_size(size)
{
  m_shader = new Shader;
}

Box::Box(Vector3D v, Rotation r, std::string t, float size) : myGLWidget(v, r, t), m_size(size)
{
  m_shader = new Shader;
}

Box::~Box()
{
}

void  Box::initializeGLNoList()
{

}

void  Box::initializeGL()
{
  loadTexture();

  std::string vert =
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

  std::string frag =
      "#version 150 core\n"

      "in vec2 coordTexture;\n"

      "uniform sampler2D tex;\n"

      "out vec4 out_Color;\n"

      "void main()\n"
      "{\n"
      "out_Color = texture(tex, coordTexture);\n"
      "}";


  m_shader->setVertexSource(vert);
  m_shader->setFragmentSource(frag);
  if (!m_shader->load()){
      HandleError::showError("Shader didn't load");
      exit(-1);
    }

  m_size /= 2.f;
  float verticesTmp[] = {-m_size, -m_size, -m_size,   m_size, -m_size, -m_size,   m_size, m_size, -m_size,     // Face 1
                         -m_size, -m_size, -m_size,   -m_size, m_size, -m_size,   m_size, m_size, -m_size,     // Face 1

                         m_size, -m_size, m_size,   m_size, -m_size, -m_size,   m_size, m_size, -m_size,       // Face 2
                         m_size, -m_size, m_size,   m_size, m_size, m_size,   m_size, m_size, -m_size,         // Face 2

                         -m_size, -m_size, m_size,   m_size, -m_size, m_size,   m_size, -m_size, -m_size,      // Face 3
                         -m_size, -m_size, m_size,   -m_size, -m_size, -m_size,   m_size, -m_size, -m_size,    // Face 3

                         -m_size, -m_size, m_size,   m_size, -m_size, m_size,   m_size, m_size, m_size,        // Face 4
                         -m_size, -m_size, m_size,   -m_size, m_size, m_size,   m_size, m_size, m_size,        // Face 4

                         -m_size, -m_size, -m_size,   -m_size, -m_size, m_size,   -m_size, m_size, m_size,     // Face 5
                         -m_size, -m_size, -m_size,   -m_size, m_size, -m_size,   -m_size, m_size, m_size,     // Face 5

                         -m_size, m_size, m_size,   m_size, m_size, m_size,   m_size, m_size, -m_size,         // Face 6
                         -m_size, m_size, m_size,   -m_size, m_size, -m_size,   m_size, m_size, -m_size};      // Face 6

  if (!m_hasTexture){
      float couleursTmp[] = {1.0, 0.0, 0.0,   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,           // Face 1
                             1.0, 0.0, 0.0,   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,           // Face 1

                             0.0, 1.0, 0.0,   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,           // Face 2
                             0.0, 1.0, 0.0,   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,           // Face 2

                             0.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 0.0, 1.0,           // Face 3
                             0.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 0.0, 1.0,           // Face 3

                             1.0, 0.0, 0.0,   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,           // Face 4
                             1.0, 0.0, 0.0,   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,           // Face 4

                             0.0, 1.0, 0.0,   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,           // Face 5
                             0.0, 1.0, 0.0,   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,           // Face 5

                             0.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 0.0, 1.0,           // Face 6
                             0.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 0.0, 1.0};          // Face 6
      for (unsigned int i(0); i < sizeof(couleursTmp) / sizeof(couleursTmp[0]); ++i){
          m_couleurs.push_back(couleursTmp[i]);
        }
    }

  else{
      float coordTextureTmp[] = {0, 0,   1, 0,   1, 1,     // Face 1
                                 0, 0,   0, 1,   1, 1,     // Face 1

                                 0, 0,   1, 0,   1, 1,     // Face 2
                                 0, 0,   0, 1,   1, 1,     // Face 2

                                 0, 0,   1, 0,   1, 1,     // Face 3
                                 0, 0,   0, 1,   1, 1,     // Face 3

                                 0, 0,   1, 0,   1, 1,     // Face 4
                                 0, 0,   0, 1,   1, 1,     // Face 4

                                 0, 0,   1, 0,   1, 1,     // Face 5
                                 0, 0,   0, 1,   1, 1,     // Face 5

                                 0, 0,   1, 0,   1, 1,     // Face 6
                                 0, 0,   0, 1,   1, 1};

      for (unsigned int i(0); i < sizeof(coordTextureTmp) / sizeof(coordTextureTmp[0]); ++i){
          m_textures.push_back(coordTextureTmp[i]);
        }
    }

  for (int i(0); i < 108; ++i){
      m_vertices.push_back(verticesTmp[i]);
    }
}

void Box::paintGL(const glm::mat4& view_matrix, const glm::mat4& proj_matrix)
{
  // Activation du shader
  glUseProgram(m_shader->getProgramID());

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, &m_vertices[0]);
  glEnableVertexAttribArray(0);

  if (m_couleurs.size() > 0){
      // Envoi de la couleur
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, &m_couleurs[0]);
      glEnableVertexAttribArray(1);
    }

  if (m_textures.size() > 0){
      //envoi des coordonnees des textures
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, &m_textures[0]);
      glEnableVertexAttribArray(2);
    }

  // Envoi des matrices
  //glUniformMatrix4fv(glGetUniformLocation(m_shader->getProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(proj_matrix));

  glm::mat4 tmp = glm::translate(view_matrix, glm::vec3(m_pos.x(), m_pos.y(), m_pos.z()));

  if (m_rot.getRotation() != 0.f && (m_rot.getRotX() != 0.f || m_rot.getRotY() != 0.f || m_rot.getRotZ() != 0.f))
    tmp = glm::rotate(tmp, m_rot.getRotation(), glm::vec3(m_rot.getRotX(), m_rot.getRotY(), m_rot.getRotZ()));
  //glUniformMatrix4fv(glGetUniformLocation(m_shader->getProgramID(), "modelview"), 1, GL_FALSE, glm::value_ptr(tmp));

  // Rendu
  m_texture.bind();
  glDrawArrays(GL_TRIANGLES, 0, 36);
  m_texture.unbind();

  // Desactivation des tableaux
  if (m_couleurs.size() > 0)
    glDisableVertexAttribArray(1);
  if (m_textures.size() > 0)
    glDisableVertexAttribArray(2);
  glDisableVertexAttribArray(0);

  // Desactivation du shader
  glUseProgram(0);
}
