#include "myGLWidget.hpp"
#include "../String_utils.hpp"
#include "../HandleError.hpp"
#include <cmath>
#include <cstring>

Color myGLWidget::pick_color = Color(0.f, 0.f, 0.f);

//TODO create a way to avoid double values
Color &myGLWidget::getStaticPickColor()
{
  static float  add_value(1.f / 255.f);
  float         tmp;

  tmp = pick_color.red();
  tmp += add_value;
  if (tmp >= 1.f) {
      pick_color.setRed(0.f);
      tmp = pick_color.green();
      tmp += add_value;
      if (tmp >= 1.f) {
          pick_color.setGreen(0.f);
          tmp = pick_color.blue();
          tmp += add_value;
          if (tmp >= 1.f) {
              tmp = 0.f;
            }
          pick_color.setBlue(tmp);
          return pick_color;
        }
      pick_color.setGreen(tmp);
      return pick_color;
    }
  pick_color.setRed(tmp);
  return pick_color;
}

myGLWidget::myGLWidget(Vector3D p, Rotation rot)
  : m_hasTexture(false), m_pos(p), m_rot(rot), m_color(Color()), m_selected(false),
    m_pickAllow(false), mainWindow(0), m_shader(0), m_vboID(0), m_vaoID(0),
    m_verticesSize(0), m_colorsSize(0), m_texturesSize(0), m_pointsNumber(0), m_render2D(false),
    m_drawMode(GL_TRIANGLES)
{
  m_primaryColor = myGLWidget::getStaticPickColor();
  m_primaryShader = 0;
}

myGLWidget::myGLWidget(Vector3D p, Rotation rot, Color co)
  : m_hasTexture(false), m_pos(p), m_rot(rot), m_color(co), m_selected(false),
    m_pickAllow(false), mainWindow(0), m_shader(0), m_vboID(0), m_vaoID(0),
    m_verticesSize(0), m_colorsSize(0), m_texturesSize(0), m_pointsNumber(0), m_render2D(false),
    m_drawMode(GL_TRIANGLES)
{
  m_primaryColor = myGLWidget::getStaticPickColor();
  m_primaryShader = 0;
}

myGLWidget::myGLWidget(Vector3D p, Rotation rot, const string tex)
  : m_hasTexture(false), m_pos(p), m_rot(rot), m_color(Color()), m_selected(false),
    m_pickAllow(false), mainWindow(0), m_shader(0), m_vboID(0), m_vaoID(0),
    m_verticesSize(0), m_colorsSize(0), m_texturesSize(0), m_pointsNumber(0), m_render2D(false), m_texture(tex),
    m_drawMode(GL_TRIANGLES)
{
  m_primaryColor = myGLWidget::getStaticPickColor();
  m_primaryShader = 0;
}

myGLWidget::myGLWidget(Vector3D p, Rotation rot, Texture const &tex)
  : m_hasTexture(false), m_pos(p), m_rot(rot), m_color(Color()), m_selected(false),
    m_pickAllow(false), mainWindow(0), m_shader(0), m_vboID(0), m_vaoID(0),
    m_verticesSize(0), m_colorsSize(0), m_texturesSize(0), m_pointsNumber(0), m_render2D(false),
    m_texture(tex), m_drawMode(GL_TRIANGLES)
{
  m_primaryColor = myGLWidget::getStaticPickColor();
  m_primaryShader = 0;
}

myGLWidget::~myGLWidget()
{
  if (m_shader)
    delete m_shader;
  if (m_vboID)
    glDeleteBuffers(1, &m_vboID);
  if (m_vaoID)
    glDeleteVertexArrays(1, &m_vaoID);
  if (m_primaryShader)
    delete m_primaryShader;
}

void myGLWidget::setTexture(const string s)
{
  /*if (s == m_texture.getTextureName())
    return;*/
  m_texture.setTexture(s);
}

void myGLWidget::setTexture(GLuint id)
{
  m_texture.setTextureID(id);
  m_hasTexture = (id != 0);
}

void  myGLWidget::setTexture(Texture const &t)
{
  m_texture = t;
}

void    myGLWidget::setRender2D(bool b)
{
  m_render2D = b;
}

bool  myGLWidget::is2D() const
{
  return m_render2D;
}

GLuint myGLWidget::getTextureId() const
{
  return m_texture.getTextureID();
}

bool  myGLWidget::hasTexture() const
{
  return m_hasTexture;
}

void  myGLWidget::resize(int width, int height)
{
  (void)width;
  (void)height;
}

Texture &myGLWidget::getTexture()
{
  return m_texture;
}

bool myGLWidget::loadTexture()
{
  if (m_hasTexture)
    return false;
  m_hasTexture = false;
  m_hasTexture = m_texture.load();
  return m_hasTexture;
}

Color const &myGLWidget::getColor() const
{
  return m_color;
}

void  myGLWidget::setColor(Color c)
{
  m_color = c;
}

Color const     &myGLWidget::getPikingColor() const
{
  return m_pickingColor;
}

void  myGLWidget::setPikingColor(Color c)
{
  m_pickingColor = c;
}

Vector3D &myGLWidget::getPosition()
{
  return m_pos;
}

void  myGLWidget::setPosition(Vector3D v)
{
  m_pos = v;
}

void  myGLWidget::setSelected(bool b)
{
  m_selected = b;
}

bool  myGLWidget::isSelected() const
{
  return m_selected;
}

bool  myGLWidget::isPickingAllowed()
{
  return m_pickAllow;
}

void  myGLWidget::setPickingAllowed(bool b)
{
  m_pickAllow = b;
}

bool  myGLWidget::hasBeenPicked(int c)
{
  (void)c;
  return false;
}

void  myGLWidget::pick(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix)
{
  if (!m_pickAllow)
    return;
  //glLoadName(x++);
  //this->paintGL(view_matrix, proj_matrix);
  glUseProgram(m_primaryShader->getProgramID());

  glBindVertexArray(m_primary_vaoID);

  // Envoi des matrices
  glUniformMatrix4fv(m_primaryLoc_projection, 1, GL_FALSE, glm::value_ptr(proj_matrix));

  glm::mat4 tmp = glm::translate(view_matrix, glm::vec3(m_pos.x(), m_pos.y(), m_pos.z()));

  if (m_rot.rotation() != 0.f && (m_rot.x() != 0.f || m_rot.y() != 0.f || m_rot.z() != 0.f))
    tmp = glm::rotate(tmp, m_rot.rotation(), glm::vec3(m_rot.x(), m_rot.y(), m_rot.z()));
  glUniformMatrix4fv(m_primaryLoc_modelView, 1, GL_FALSE, glm::value_ptr(tmp));

  glDrawArrays(GL_TRIANGLES, 0, m_pointsNumber);

  glBindVertexArray(0);
  glUseProgram(0);
}

int myGLWidget::objectSize()
{
  return 1;
}

void    myGLWidget::setMainWindow(MyWindow *w)
{
  mainWindow = w;
}

MyWindow   *myGLWidget::getMainWindow() const
{
  return mainWindow;
}

Rotation &myGLWidget::rotation()
{
  return m_rot;
}

std::string const &myGLWidget::getTextureName() const
{
  return m_texture.getTextureName();
}

void  myGLWidget::initVertexBufferObject(GLenum option)
{
  m_verticesSize = m_vertices.size() * sizeof(GLfloat);
  m_colorsSize = m_couleurs.size() * sizeof(GLfloat);
  m_texturesSize = m_textures.size() * sizeof(GLfloat);

  glGenBuffers(1, &m_vboID);
  //verrouillage de l'objet
  glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

  // Allocation de la memoire video
  glBufferData(GL_ARRAY_BUFFER, m_verticesSize + m_texturesSize + m_colorsSize, 0, option);

  // Transfert des donnees
  glBufferSubData(GL_ARRAY_BUFFER, 0, m_verticesSize, &m_vertices[0]);
  if (m_texturesSize > 0)
    glBufferSubData(GL_ARRAY_BUFFER, m_verticesSize, m_texturesSize, &m_textures[0]);
  if (m_colorsSize > 0)
    glBufferSubData(GL_ARRAY_BUFFER, m_verticesSize + m_texturesSize, m_colorsSize, &m_couleurs[0]);

  // Deverrouillage de l'objet
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  std::string vert = "#version 150 core\n"
      "in vec3 in_Vertex;\n"

      "uniform mat4 projection;\n"
      "uniform mat4 modelview;\n"

      "out vec3 color;\n"
      "void main()\n"
      "{\n"
      "gl_Position = projection * modelview * vec4(in_Vertex, 1.0);\n"
      "color = vec3("
      + Utility::toString<float>(m_primaryColor.red()) + ","
      + Utility::toString<float>(m_primaryColor.green()) + ","
      + Utility::toString<float>(m_primaryColor.blue()) + ");\n"
      "}";
  std::string frag =
      "#version 150 core\n"

      "in vec3 color;\n"

      "out vec4 out_Color;\n"
      "void main()\n"
      "{\n"
      "out_Color = vec4(color, 1.0);\n"
      "}";

  if (!m_primaryShader)
    m_primaryShader = new Shader;
  m_primaryShader->setVertexSource(vert);
  m_primaryShader->setFragmentSource(frag);
  if (!m_primaryShader->load()){
      HandleError::showError("Primary shader didn't load");
      exit(-1);
    }
  m_primaryLoc_projection = glGetUniformLocation(m_primaryShader->getProgramID(), "projection");
  m_primaryLoc_modelView = glGetUniformLocation(m_primaryShader->getProgramID(), "modelview");

  glGenBuffers(1, &m_primary_vboID);
  glBindBuffer(GL_ARRAY_BUFFER, m_primary_vboID);
  glBufferData(GL_ARRAY_BUFFER, m_verticesSize, 0, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, m_verticesSize, &m_vertices[0]);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void  myGLWidget::bindVertexBufferObject()
{
  // Verrouillage du VBO
  glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

  // Acces aux vertices dans la memoire video
  if (m_render2D)
    glVertexAttribPointer(VERTEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  else
    glVertexAttribPointer(VERTEX_COORD, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(VERTEX_COORD);


  if (m_textures.size() > 0){
      //envoi des coordonnees des textures
      glVertexAttribPointer(TEXTURE_COORD, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(m_verticesSize));
      glEnableVertexAttribArray(TEXTURE_COORD);
    }
  if (m_couleurs.size() > 0){
      // Envoi de la couleur
      glVertexAttribPointer(COLOR_COORD, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(m_verticesSize + m_texturesSize));
      glEnableVertexAttribArray(COLOR_COORD);
    }

  // Deverrouillage du VBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void  myGLWidget::updateVertexBufferObject(void *donnees, int tailleBytes, int decalage)
{
  // Verrouillage du VBO
  glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

  // Recuperation de l'adresse du VBO

  // remplace glMapBuffer
  void *adresseVBO = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

  // Si l'adresse retournee est nulle alors on arrete le transfert
  if (!adresseVBO)
    {
      std::cout << "Error: cannot get VBO" << std::endl;
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      return;
    }

  // Mise a jour des donnees
  memcpy((char*)adresseVBO + decalage, donnees, tailleBytes);

  // Annulation du pointeur
  glUnmapBuffer(GL_ARRAY_BUFFER);
  adresseVBO = 0;

  // Deverrouillage du VBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void  myGLWidget::initVertexArrayObject()
{
  glGenVertexArrays(1, &m_vaoID);
  glBindVertexArray(m_vaoID);

  this->bindVertexBufferObject();

  glBindVertexArray(0);

  glGenVertexArrays(1, &m_primary_vaoID);
  glBindVertexArray(m_primary_vaoID);
  glBindBuffer(GL_ARRAY_BUFFER, m_primary_vboID);
  if (m_render2D)
    glVertexAttribPointer(VERTEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  else
    glVertexAttribPointer(VERTEX_COORD, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(VERTEX_COORD);
  glBindVertexArray(0);
}

std::vector<GLfloat> &myGLWidget::getColors()
{
  return m_couleurs;
}

void  myGLWidget::updateColors(const std::vector<GLfloat> &c)
{
  if (m_hasTexture)
    return;
  for (unsigned int i = 0; i < m_couleurs.size() && i < c.size(); ++i)
    m_couleurs[i] = c[i];
  this->updateVertexBufferObject(&m_couleurs[0], m_colorsSize, m_verticesSize);
}

std::vector<GLfloat> &myGLWidget::getTextures()
{
  return m_textures;
}

void  myGLWidget::updateTextures(const std::vector<GLfloat> &t)
{
  if (!m_hasTexture)
    return;
  for (unsigned int i = 0; i < m_textures.size() && i < t.size(); ++i)
    m_textures[i] = t[i];
  this->updateVertexBufferObject(&m_textures[0], m_texturesSize, m_verticesSize);
}

std::vector<GLfloat>   &myGLWidget::getVertices()
{
  return m_vertices;
}

void  myGLWidget::updateVertices(const std::vector<GLfloat> &v)
{
  for (unsigned int i = 0; i < m_vertices.size() && i < v.size(); ++i)
    m_vertices[i] = v[i];
  this->updateVertexBufferObject(&m_vertices[0], m_verticesSize, 0);
}

void    myGLWidget::update(const float &n)
{
  m_rot.update(n);
}

void    myGLWidget::initializeGLNoList()
{
}

Color const &myGLWidget::getPrimaryColor() const
{
  return m_primaryColor;
}

void  myGLWidget::convertTRIANGLE_STRIP_To_TRIANGLES(std::vector<Vector3D> const &tmp_vertices, std::vector<Vector3D> const &tmp_textures)
{
  unsigned int pos = 0;

  for (; pos < 3; ++pos) {
      m_vertices.push_back(tmp_vertices[pos].x());
      m_vertices.push_back(tmp_vertices[pos].y());
      m_vertices.push_back(tmp_vertices[pos].z());

      m_textures.push_back(tmp_textures[pos].x());
      m_textures.push_back(tmp_textures[pos].y());
    }

  for (; pos < tmp_vertices.size(); ++pos) {
      if (!(pos & 1)) {
          m_vertices.push_back(tmp_vertices[pos - 1].x());
          m_vertices.push_back(tmp_vertices[pos - 1].y());
          m_vertices.push_back(tmp_vertices[pos - 1].z());

          m_vertices.push_back(tmp_vertices[pos - 2].x());
          m_vertices.push_back(tmp_vertices[pos - 2].y());
          m_vertices.push_back(tmp_vertices[pos - 2].z());

          m_vertices.push_back(tmp_vertices[pos].x());
          m_vertices.push_back(tmp_vertices[pos].y());
          m_vertices.push_back(tmp_vertices[pos].z());

          m_textures.push_back(tmp_textures[pos - 1].x());
          m_textures.push_back(tmp_textures[pos - 1].y());
          m_textures.push_back(tmp_textures[pos - 2].x());
          m_textures.push_back(tmp_textures[pos - 2].y());
          m_textures.push_back(tmp_textures[pos].x());
          m_textures.push_back(tmp_textures[pos].y());
        } else {
          m_vertices.push_back(tmp_vertices[pos - 2].x());
          m_vertices.push_back(tmp_vertices[pos - 2].y());
          m_vertices.push_back(tmp_vertices[pos - 2].z());

          m_vertices.push_back(tmp_vertices[pos - 1].x());
          m_vertices.push_back(tmp_vertices[pos - 1].y());
          m_vertices.push_back(tmp_vertices[pos - 1].z());

          m_vertices.push_back(tmp_vertices[pos].x());
          m_vertices.push_back(tmp_vertices[pos].y());
          m_vertices.push_back(tmp_vertices[pos].z());

          m_textures.push_back(tmp_textures[pos - 2].x());
          m_textures.push_back(tmp_textures[pos - 2].y());
          m_textures.push_back(tmp_textures[pos - 1].x());
          m_textures.push_back(tmp_textures[pos - 1].y());
          m_textures.push_back(tmp_textures[pos].x());
          m_textures.push_back(tmp_textures[pos].y());
        }
    }
}

void  myGLWidget::convertTRIANGLE_STRIP_To_TRIANGLES(std::vector<Vector3D> const &tmp_vertices)
{
  unsigned int pos = 0;

  for (; pos < 3; ++pos) {
      m_vertices.push_back(tmp_vertices[pos].x());
      m_vertices.push_back(tmp_vertices[pos].y());
      m_vertices.push_back(tmp_vertices[pos].z());

      m_couleurs.push_back(m_color.red());
      m_couleurs.push_back(m_color.green());
      m_couleurs.push_back(m_color.blue());
    }

  for (; pos < tmp_vertices.size(); ++pos) {
      if (!(pos & 1)) {
          m_vertices.push_back(tmp_vertices[pos - 1].x());
          m_vertices.push_back(tmp_vertices[pos - 1].y());
          m_vertices.push_back(tmp_vertices[pos - 1].z());

          m_vertices.push_back(tmp_vertices[pos - 2].x());
          m_vertices.push_back(tmp_vertices[pos - 2].y());
          m_vertices.push_back(tmp_vertices[pos - 2].z());

          m_vertices.push_back(tmp_vertices[pos].x());
          m_vertices.push_back(tmp_vertices[pos].y());
          m_vertices.push_back(tmp_vertices[pos].z());
        } else {
          m_vertices.push_back(tmp_vertices[pos - 2].x());
          m_vertices.push_back(tmp_vertices[pos - 2].y());
          m_vertices.push_back(tmp_vertices[pos - 2].z());

          m_vertices.push_back(tmp_vertices[pos - 1].x());
          m_vertices.push_back(tmp_vertices[pos - 1].y());
          m_vertices.push_back(tmp_vertices[pos - 1].z());

          m_vertices.push_back(tmp_vertices[pos].x());
          m_vertices.push_back(tmp_vertices[pos].y());
          m_vertices.push_back(tmp_vertices[pos].z());
        }
      for (int i = 0; i < 3; ++i) {
          m_couleurs.push_back(m_color.red());
          m_couleurs.push_back(m_color.green());
          m_couleurs.push_back(m_color.blue());
        }
    }
}

std::string myGLWidget::getClassName() const
{
  return std::string("myGLWidget");
}

void  myGLWidget::setDrawMode(GLenum e)
{
  m_drawMode = e;
}

GLenum  myGLWidget::drawMode() const
{
  return m_drawMode;
}
