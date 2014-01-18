#include "Door.hpp"
#include "../MyWindow.hpp"
#include "../Utils/MyException.hpp"
#include <cstring>
#include "Color.hpp"

using namespace Object;

Door::Door(Vector3D p, Rotation r, const char *name) : myGLWidget(p, r)
{
  if (!name || strlen(name) < 1)
    throw MyException("Door: name parameter cannot be empty or null");
  m_name = name;
  if (m_name.length() > 15) {
      m_name.erase(12);
      m_name += "...";
    }
  m_size = 5 * m_name.length();
  m_height = 130.f;

  m_texture = (*MyWindow::getLib()->createTextTexture(m_name.c_str(), &m_texture, RED));
  //m_texture.setTextureID(tex);
}

Door::~Door()
{
}

float const &Door::getSize() const
{
  return m_size;
}

std::string const &Door::getName() const
{
  return m_name;
}

void  Door::initializeGL()
{
  //m_glObject = glGenLists(1);

  //glNewList(m_glObject, GL_COMPILE);

  this->initializeGLNoList();

  glEndList();
}

void  Door::initializeGLNoList()
{
  // Activation de la transparence
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_TEXTURE_2D);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  m_texture.bind();

  glBegin(GL_QUADS);
  glTexCoord2f(0.01f, 0.01f); glVertex3f(-m_size, 30, 0.0f);
  glTexCoord2f(0.01f, 0.99f); glVertex3f(-m_size, -30, 0.0f);
  glTexCoord2f(0.99f, 0.99f); glVertex3f(m_size, -30, 0.0f);
  glTexCoord2f(0.99f, 0.01f); glVertex3f(m_size, 30, 0.0f);
  glEnd();

  m_texture.unbind();
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
}

void    Door::initializeGL(bool invert)
{
  Vector3D  tmp(m_pos);

  glPushMatrix();

  glColor3f(1.f, 1.f, 1.f);
  //pancarte
  glBegin(GL_QUADS);
  glVertex3f(tmp.x(), tmp.y() + m_height, tmp.z());
  glVertex3f(150.f + tmp.x(), tmp.y() + m_height, tmp.z());
  glVertex3f(150.f + tmp.x(), tmp.y() + (m_height - 60.f), tmp.z());
  glVertex3f(tmp.x(), tmp.y() + (m_height - 60.f), tmp.z());
  glEnd();
  //partie gauche
  glBegin(GL_QUADS);
  glVertex3f(tmp.x(), tmp.y() + (m_height - 60.f), tmp.z());
  glVertex3f(40.f + tmp.x(), tmp.y() + (m_height - 60.f), tmp.z());
  glVertex3f(40.f + tmp.x(), tmp.y(), tmp.z());
  glVertex3f(tmp.x(), tmp.y(), tmp.z());
  glEnd();
  //partie droite
  glBegin(GL_QUADS);
  glVertex3f(105.f + tmp.x(), tmp.y() + (m_height - 60.f), tmp.z());
  glVertex3f(150.f + tmp.x(), tmp.y() + (m_height - 60.f), tmp.z());
  glVertex3f(150.f + tmp.x(), tmp.y(), tmp.z());
  glVertex3f(105.f + tmp.x(), tmp.y(), tmp.z());
  glEnd();

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  m_texture.bind();
  glBegin(GL_QUADS);

  float x_val = m_size * 2.f;
  float spacer = (150.f - x_val);
  if (spacer < 0.f)
    spacer = 0.f;
  else
    spacer /= 2.f;
  if (!invert){
      glTexCoord2f(0.01f, 0.01f); glVertex3f(tmp.x() + spacer, tmp.y() + m_height, tmp.z() + 1.f);
      glTexCoord2f(0.01f, 0.99f); glVertex3f(tmp.x() + spacer, (m_height - 60.f) + tmp.y(), tmp.z() + 1.f);
      glTexCoord2f(0.99f, 0.99f); glVertex3f(m_size * 2.f + tmp.x() + spacer, (m_height - 60.f) + tmp.y(), tmp.z() + 1.f);
      glTexCoord2f(0.99f, 0.01f); glVertex3f(m_size * 2.f + tmp.x() + spacer, tmp.y() + m_height, tmp.z() + 1.f);
    }
  else {
      glTexCoord2f(0.99f, 0.01f); glVertex3f(tmp.x() + spacer, tmp.y() + m_height, tmp.z() - 1.f);
      glTexCoord2f(0.99f, 0.99f); glVertex3f(tmp.x() + spacer, (m_height - 60.f) + tmp.y(), tmp.z() - 1.f);
      glTexCoord2f(0.01f, 0.99f); glVertex3f(m_size * 2.f + tmp.x() + spacer, (m_height - 60.f) + tmp.y(), tmp.z() - 1.f);
      glTexCoord2f(0.01f, 0.01f); glVertex3f(m_size * 2.f + tmp.x() + spacer, tmp.y() + m_height, tmp.z() - 1.f);
    }
  glEnd();
  m_texture.unbind();

  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
}

void    Door::paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix)
{
  //glLoadIdentity();
  glPushMatrix();
  glTranslatef(m_pos.x(), m_pos.y(), m_pos.z());
  glRotatef(m_rot.rotation(), m_rot.x(), m_rot.y(), 0.0f);
  //glCallList(m_glObject);
  glPopMatrix();
}

string Door::getClassName() const
{
  return std::string("Door");
}
