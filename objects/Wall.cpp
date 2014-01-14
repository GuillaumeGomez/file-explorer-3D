#include "Wall.hpp"

using namespace Object;

Wall::Wall(Vector3D v, Rotation r, Color c, float width, float height) : Plane(v, r, c, width, height)
{
}

Wall::Wall(Vector3D v, Rotation r, std::string tex, float width, float height) : Plane(v, r, tex, width, height)
{
}

void Wall::initializeGL()
{
  //m_glObject = glGenLists(1);
  //glNewList(m_glObject, GL_COMPILE);
  this->initializeGLNoList();
  glEndList();
}

void  Wall::initializeGLNoList()
{
  glPushMatrix();
  glTranslatef(m_pos.x(), m_pos.y(), m_pos.z());
  glRotatef(m_rot.rotation(), m_rot.x(), m_rot.y(), 0.0f);
  //glTranslatef(m_pos.x(), m_pos.y(), m_pos.z());
  //glRotatef(m_rot.rotation(), m_rot.x(), m_rot.y(), 0.0f);
  if (!m_hasTexture)
    {
      //partie du haut
      glBegin(GL_QUADS);
      glColor3f(m_color.red(), m_color.green(), m_color.blue());
      glVertex3f(0.f, 0.f, 0.f);
      glVertex3f(this->width, 0.f, 0.f);
      glVertex3f(this->width, this->height / 3.f, 0.f);
      glVertex3f(0.f, this->height / 3.f, 0.f);
      glEnd();

      //partie du bas
      glBegin(GL_QUADS);
      glColor3f(m_color.red(), m_color.green(), m_color.blue());
      glVertex3f(0.f, this->height / 3.f * 2.f, 0.f);
      glVertex3f(this->width, this->height / 3.f * 2.f, 0.f);
      glVertex3f(this->width, this->height, 0.f);
      glVertex3f(0.f, this->height, 0.f);
      glEnd();

      //fenetre
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glBegin(GL_QUADS);
      //GLfloat t[] = {1.f, 1.f, 1.f, 0.4f};
      GLfloat t[] = {0.f, 0.f, 0.f, 0.1f};
      glColor4fv(t);
      glVertex3f(this->width / 4.f, this->height / 3.f, 0.f);
      glVertex3f(this->width / 4.f * 3.f, this->height / 3.f, 0.f);
      glVertex3f(this->width / 4.f * 3.f, this->height / 3.f * 2.f, 0.f);
      glVertex3f(this->width / 4.f, this->height / 3.f * 2.f, 0.f);
      glEnd();
      glDisable(GL_BLEND);

      //milieu gauche
      glBegin(GL_QUADS);
      glColor3f(m_color.red(), m_color.green(), m_color.blue());
      glVertex3f(0.f, this->height / 3.f, 0.f);
      glVertex3f(this->width / 4.f, this->height / 3.f, 0.f);
      glVertex3f(this->width / 4.f, this->height, 0.f);
      glVertex3f(0.f, this->height, 0.f);
      glEnd();

      //milieu droite
      glBegin(GL_QUADS);
      glColor3f(m_color.red(), m_color.green(), m_color.blue());
      glVertex3f(this->width / 4.f * 3.f, this->height / 3.f, 0.f);
      glVertex3f(this->width, this->height / 3.f, 0.f);
      glVertex3f(this->width, this->height, 0.f);
      glVertex3f(this->width / 4.f * 3.f, this->height, 0.f);
      glEnd();
    }
  else
    {
      glEnable(GL_TEXTURE_2D);
      m_texture.bind();
      glBegin(GL_QUADS);
      glColor3f(m_color.red(), m_color.green(), m_color.blue());
      glTexCoord2f(0.f, 0.f);                                glVertex3f(0.f, 0.f, this->width / -6.f);
      glTexCoord2f(this->width / 25.f, 0.f);                 glVertex3f(0.f, 0.f, this->width / 6.f);
      glTexCoord2f(this->width / 25.f, this->height / 25.f); glVertex3f(this->height / 3.f, 0.f, this->width / 6.f);
      glTexCoord2f(0.f, this->height / 25.f);                glVertex3f(this->height / 3.f, 0.f, this->width / -6.f);
      glEnd();
      m_texture.unbind();
      glDisable(GL_TEXTURE_2D);
    }
  glPopMatrix();
}

void  Wall::paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix)
{
  glPushMatrix();
  glTranslatef(m_pos.x(), m_pos.y(), m_pos.z());
  glRotatef(m_rot.rotation(), m_rot.x(), m_rot.y(), 0.0f);
  //glCallList(m_glObject);
  glPopMatrix();
}

string Wall::getClassName() const
{
  return string("Wall");
}
