#include "VertexBuffer.hpp"
//#include "ShaderHandler.hpp"
#include "../HandleError.hpp"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "../MyWindow.hpp"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../Camera.hpp"
#include "../myGLWidget.hpp"

VertexBuffer::VertexBuffer(std::vector<GLfloat> v, myGLWidget *w)
  : m_vertices(v), m_elements(std::vector<GLuint>()), m_widget(w)
{
  this->init();
}

VertexBuffer::VertexBuffer(std::vector<GLfloat> v, std::vector<GLuint> e, myGLWidget *w)
  : m_vertices(v), m_elements(e), m_widget(w)
{
  this->init();
}

void  VertexBuffer::init()
{
  m_texAttrib = 0;
  /*std::string shaderSource =
      "#version 150\n"
      "in vec3 position;\n"
      "void main()\n"
      "{\n"
      "gl_Position = vec4(position, 1.0);\n"
      "}";
  std::string fragmentSource =
      "#version 150 core\n"
      "out vec4 outColor;"
      "uniform vec3 triangleColor;"
      "void main() {"
      "   outColor = vec4(triangleColor, 1.0);"
      "}";*/

  /*std::string shaderSource =
      "#version 150 core\n"
      "in vec2 position;"
      "in vec3 color;"
      "in vec2 texcoord;"
      "out vec3 Color;"
      "out vec2 Texcoord;"
      "void main() {"
      "   Color = color;"
      "   Texcoord = texcoord;"
      "   gl_Position = vec4(position, 0.0, 1.0);"
      "}";*/
  std::string fragmentSource =
      "#version 150 core\n"
      "in vec3 Color;"
      "in vec2 Texcoord;"
      "out vec4 outColor;"
      "uniform sampler2D tex;"
      "void main() {"
      "   outColor = texture(tex, Texcoord) * vec4(Color, 1.0);"
      "}";
  std::string shaderSource =
      "#version 150\n"
      "in vec2 position;"
      "in vec3 color;"
      "in vec2 texcoord;"
      "out vec3 Color;"
      "out vec2 Texcoord;"
      "uniform mat4 model;"
      "uniform mat4 view;"
      "uniform mat4 proj;"
      "void main() {"
      "Color = color;"
      "Texcoord = texcoord;"
      "gl_Position = proj * view * model * vec4(position, 0.0, 1.0);"
      "}";

  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);

  glGenBuffers(1, &m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(GLfloat), &m_vertices[0], GL_STATIC_DRAW);

  if (m_elements.size() > 0){
      glGenBuffers(1, &m_ebo);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_elements.size() * sizeof(GLuint), &m_elements[0], GL_STATIC_DRAW);
    }

  /*m_shader = new ShaderHandler(shaderSource, fragmentSource);

  if (!m_shader->isWorking()){
      HandleError::showError(m_shader->getLastError());
      exit(-1);
    }
  //GL_STATIC_DRAW: The vertex data will be uploaded once and drawn many times (e.g. the world).
  //GL_DYNAMIC_DRAW: The vertex data will be changed from time to time, but drawn many times more than that.
  //GL_STREAM_DRAW: The vertex data will change almost every time it's drawn (e.g. user interface).

  m_posAttrib = glGetAttribLocation(m_shader->getProgramID(), "position");
  glEnableVertexAttribArray(m_posAttrib);
  glVertexAttribPointer(m_posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);

  m_colAttrib = glGetAttribLocation(m_shader->getProgramID(), "color");
  glEnableVertexAttribArray(m_colAttrib);
  glVertexAttribPointer(m_colAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

  if (m_widget && !m_widget->getTextureName().empty()){
      m_texAttrib = glGetAttribLocation(m_shader->getProgramID(), "texcoord");
      glEnableVertexAttribArray(m_texAttrib);
      glVertexAttribPointer(m_texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

      m_widget->loadTexture();
    }

  //m_tex = MyWindow::loadTexture("./textures/metal.jpg");

  m_uniView = glGetUniformLocation(m_shader->getProgramID(), "view");
  m_uniProj = glGetUniformLocation(m_shader->getProgramID(), "proj");*/
}

VertexBuffer::~VertexBuffer()
{
  /*delete m_shader;
  glDeleteBuffers(1, &m_ebo);
  glDeleteBuffers(1, &m_vbo);
  glDeleteVertexArrays(1, &m_vao);*/
}

void  VertexBuffer::draw()
{
  /*static GLint uniColor = glGetUniformLocation(m_shader->getProgramID(), "triangleColor");

  GLfloat time = (GLfloat)clock() / (GLfloat)CLOCKS_PER_SEC;
  glUniform3f(uniColor, (sin(time * 4.0f) + 1.0f) / 2.0f, 0.0f, 0.0f);*/

  /*m_shader->useProgram();

  glVertexAttribPointer(m_posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
  glEnableVertexAttribArray(m_posAttrib);
  glVertexAttribPointer(m_colAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(m_colAttrib);
  if (m_widget->hasTexture()){
      glVertexAttribPointer(m_texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
      glEnableVertexAttribArray(m_texAttrib);
    }

  GLint uniTrans = glGetUniformLocation(m_shader->getProgramID(), "model");

  glUniformMatrix4fv(m_uniProj, 1, GL_FALSE, glm::value_ptr(Camera::getProjectionMatrix()));
  glUniformMatrix4fv(m_uniView, 1, GL_FALSE, glm::value_ptr(Camera::getViewMatrix()));

  if (m_widget)
    {
      glm::mat4 trans;

      if (m_widget->rotation().x() != 0.f ||
          m_widget->rotation().y() != 0.f ||
          m_widget->rotation().z() != 0.f)
        trans = glm::rotate(trans, m_widget->rotation().rotation(),
                            glm::vec3(m_widget->rotation().x(),
                                      m_widget->rotation().y(),
                                      m_widget->rotation().z()));
      /*trans = glm::rotate(
        trans,
        (float)clock() / (float)CLOCKS_PER_SEC * 180.0f,
        glm::vec3(0.0f, 0.0f, 1.0f)
        );*/
      /*glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(trans));
    }
  else{
      glm::mat4 trans;

      trans = glm::rotate(
            trans,
            (float)clock() / (float)CLOCKS_PER_SEC * 180.0f,
            glm::vec3(0.0f, 0.0f, 1.0f)
            );
      glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(trans));
    }
  if (m_elements.size() > 0){
      //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      glDrawElements(GL_TRIANGLES, m_elements.size(), GL_UNSIGNED_INT, 0);
    }
  else{
      glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
    }
  if (m_widget->hasTexture())
    glDisableVertexAttribArray(m_texAttrib);
  glDisableVertexAttribArray(m_colAttrib);
  glDisableVertexAttribArray(m_posAttrib);
  glUseProgram(0);*/
}
