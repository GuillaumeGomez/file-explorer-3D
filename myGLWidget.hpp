#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include "GL/glew.h"
#include "Rotation.hpp"
#include "Color.hpp"
#include "Vector3D.hpp"
#include "Texture.hpp"

#include "MyWindow.hpp"

#include <iostream>
#include <string>
#include <vector>
#include "shaders/Shader.hpp"
#include "../glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace std;

#define BUFFER_OFFSET(offset) ((char*)NULL + (offset))
#ifndef M_PI
#define M_PI 3.1415926535897932385
#endif

enum {
  VERTEX_COORD,
  COLOR_COORD,
  TEXTURE_COORD
};

class myGLWidget
{
public:
  explicit myGLWidget(Vector3D, Rotation);
  explicit myGLWidget(Vector3D, Rotation, Color);
  explicit myGLWidget(Vector3D, Rotation, const std::string texture);
  virtual ~myGLWidget();
  virtual void    initializeGL() = 0;
  virtual void    paintGL(const glm::mat4& view_matrix, const glm::mat4& proj_matrix) = 0;

  //deprecated
  virtual void    initializeGLNoList();

  virtual bool    loadTexture();
  virtual void    update(float const&);
  bool            hasTexture() const;
  void            setTexture(const std::string);
  void            setTexture(GLuint);
  GLuint          getTextureId() const;
  void            setColor(Color);
  Color const     &getColor() const;
  void            setVector3D(Vector3D);
  Vector3D const  &getVector3D();
  Color const     &getPikingColor() const;
  void            setPikingColor(Color);
  Vector3D        &getPosition();
  void            setPosition(Vector3D);
  virtual void    resize(int width, int height);
  virtual void    setSelected(bool);
  bool            isSelected() const;
  virtual void    pick(int&, const glm::mat4& view_matrix, const glm::mat4& proj_matrix);
  void            setPickingAllowed(bool);
  bool            isPickingAllowed();
  virtual bool    hasBeenPicked(int);
  virtual int     objectSize();
  virtual void    setMainWindow(MyWindow*);
  MyWindow        *getMainWindow() const;
  Rotation        &getRotation();
  Texture         &getTexture();
  void            setRender2D(bool);
  std::vector<GLfloat>    &getColors();
  std::vector<GLfloat>    &getPositions();
  std::vector<GLfloat>    &getTextures();
  std::string const &getTextureName() const;
  virtual void    initVertexBufferObject(GLenum = GL_STATIC_DRAW);
  virtual void    bindVertexBufferObject();
  virtual void    updateVertexBufferObject(void *donnees, int tailleBytes, int decalage);
  virtual void    initVertexArrayObject();
  Color const     &getPrimaryColor() const;

protected:
  bool                    m_hasTexture;
  Vector3D                m_pos;
  Rotation                m_rot;
  Color                   m_color;
  int                     tex_width;
  int                     tex_height;
  GLuint                  m_glObject;
  bool                    m_selected;
  bool                    m_pickAllow;
  MyWindow                *mainWindow;
  Shader                  *m_shader;
  GLuint                  m_vboID;
  GLuint                  m_vaoID;
  GLuint                  m_verticesSize;
  GLuint                  m_colorsSize;
  GLuint                  m_texturesSize;
  GLuint                  m_pointsNumber;
  bool                    m_render2D;
  Texture                 m_texture;
  Color                   m_pickingColor;
  GLint                   m_uniLoc_modelView;
  GLint                   m_uniLoc_projection;

  std::vector<GLfloat>  m_vertices;
  std::vector<GLfloat>  m_couleurs;
  std::vector<GLfloat>  m_textures;

  Color                 m_primaryColor;
  Shader                *m_primaryShader;
  GLint                 m_primaryLoc_modelView;
  GLint                 m_primaryLoc_projection;
  GLuint                m_primary_vboID;
  GLuint                m_primary_vaoID;

  static Color  pick_color;
  static Color  &getStaticPickColor();
};

#endif // MYGLWIDGET_H
