#include "myGLWidget.hpp"
#include <cmath>
#include <cstring>

myGLWidget::myGLWidget(Vector3D p, Rotation rot)
  : m_hasTexture(false), m_pos(p), m_rot(rot), m_color(Color()),
    tex_width(0), tex_height(0), m_glObject(0), m_selected(false),
    m_pickAllow(false), mainWindow(0), m_shader(0), m_vboID(0), m_vaoID(0),
    m_verticesSize(0), m_colorsSize(0), m_texturesSize(0), m_pointsNumber(0), m_render2D(false)
{
}

myGLWidget::myGLWidget(Vector3D p, Rotation rot, Color co)
  : m_hasTexture(false), m_pos(p), m_rot(rot), m_color(co),
    tex_width(0), tex_height(0), m_glObject(0), m_selected(false),
    m_pickAllow(false), mainWindow(0), m_shader(0), m_vboID(0), m_vaoID(0),
    m_verticesSize(0), m_colorsSize(0), m_texturesSize(0), m_pointsNumber(0), m_render2D(false)
{
}

myGLWidget::myGLWidget(Vector3D p, Rotation rot, const string tex)
  : m_hasTexture(false), m_pos(p), m_rot(rot), m_color(Color()),
    tex_width(0), tex_height(0), m_glObject(0), m_selected(false),
    m_pickAllow(false), mainWindow(0), m_shader(0), m_vboID(0), m_vaoID(0),
    m_verticesSize(0), m_colorsSize(0), m_texturesSize(0), m_pointsNumber(0), m_render2D(false), m_texture(tex)
{
}

myGLWidget::~myGLWidget()
{
  if (m_glObject > 0)
    glDeleteLists(m_glObject, 1);
  if (m_shader)
    delete m_shader;
  if (m_vboID)
    glDeleteBuffers(1, &m_vboID);
  if (m_vaoID)
    glDeleteVertexArrays(1, &m_vaoID);
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

void    myGLWidget::setRender2D(bool b)
{
  m_render2D = b;
}

GLuint myGLWidget::getTextureId() const
{
  return m_texture.getTextureID();
}

bool  myGLWidget::hasTexture() const
{
  return m_hasTexture;
}

void  myGLWidget::resize()
{
  /*if (!m_hasTexture)
    return;
  if (m_glObject > 0)
    glDeleteLists(m_glObject, 1);
  glDeleteTextures(1, texture);
  m_hasTexture = false;
  initializeGL();*/
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

Vector3D const  &myGLWidget::getVector3D()
{
  return m_pos;
}

void  myGLWidget::setVector3D(Vector3D p)
{
  m_pos = p;
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

void  myGLWidget::pick(int &x, const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix)
{
  glLoadName(x++);
  this->paintGL(view_matrix, proj_matrix);
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

Rotation &myGLWidget::getRotation()
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
}

void  myGLWidget::bindVertexBufferObject()
{
  // Verrouillage du VBO
  glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

  // Acces aux vertices dans la memoire video
  if (m_render2D)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  else
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(0);


  if (m_textures.size() > 0){
      //envoi des coordonnees des textures
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(m_verticesSize));
      glEnableVertexAttribArray(2);
    }
  if (m_couleurs.size() > 0){
      // Envoi de la couleur
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(m_verticesSize + m_texturesSize));
      glEnableVertexAttribArray(1);
    }
  // Deverrouillage du VBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void  myGLWidget::updateVertexBufferObject(void *donnees, int tailleBytes, int decalage)
{
  // Verrouillage du VBO
  glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

  // Recuperation de l'adresse du VBO
  void *adresseVBO = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

  // Si l'adresse retournee est nulle alors on arrete le transfert
  if(adresseVBO == NULL)
    {
      std::cout << "Erreur au niveau de la recuperation du VBO" << std::endl;
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
}

std::vector<GLfloat>    &myGLWidget::getColors()
{
  return m_couleurs;
}

std::vector<GLfloat>    &myGLWidget::getTextures()
{
  return m_textures;
}

std::vector<GLfloat>    &myGLWidget::getPositions()
{
  return m_vertices;
}

void    myGLWidget::update(const float &n)
{
  m_rot.update(n);
}

void    myGLWidget::initializeGLNoList()
{
}
