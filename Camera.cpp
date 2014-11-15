#include "SkyBox.hpp"
#include "Model.hpp"
#include "Camera.hpp"
#include "MyWindow.hpp"
#include "Utils/HandleMutex.hpp"
#include "Utils/MyMutexLocker.hpp"
#include "Utils/HandleMutex.hpp"
#include "String_utils.hpp"
#include "HandleSDL.hpp"

#include <cmath>
#include <SDL/SDL.h>

#include <iostream>

#include "objects/Text.hpp"

#include "HandleSDL.hpp"

using namespace std;
using namespace Object;

glm::mat4 Camera::m_view;
glm::mat4 Camera::m_proj;
glm::mat4 Camera::m_2Dproj;
glm::vec3 Camera::m_vecPos;
float     Camera::distanceView = 10000.f;
float     Camera::m_ratio = 1.f;
glm::vec3 Camera::m_target(-1.f, -1.f, -1.f);
glm::vec3 Camera::m_up(0.f, 1.f, 0.f);

Camera::Camera()
  : m_speed(.6f), m_sensivity(0.1f), m_phi(24.f), m_theta(60.f), m_position(0.f, 1.f, -15.f),
    m_win(0), m_oldX(0), m_oldY(0), m_y(m_position.y()), tmpSpeed(1.f), m_skybox(0)
{
  m_mutex = new HandleMutex;
  vectorsFromAngles();
}

Camera::~Camera()
{
  if (m_skybox)
    delete m_skybox;
  if (m_mutex)
    delete m_mutex;
  /*if (m_character)
    delete m_character;*/
}

float const &Camera::getDistanceView()
{
  return distanceView;
}

void Camera::resize(int w, int h)
{
  MyMutexLocker l(m_mutex);

  (void)l;
  if (h == 0)
    h = 1;
  m_ratio = (float)w / (float)h;
  m_curX = w / 2;
  m_curY = h / 2;
  m_oldX = m_curX;
  m_oldY = m_curY;
  glViewport(0, 0, w, h);
  m_proj = glm::perspective(70.f, m_ratio, 0.1f, distanceView);
  m_2Dproj = glm::ortho(0, w, h, 0);
}

void  Camera::lookAt()
{
  m_view = glm::lookAt(glm::vec3(m_position.x(), m_position.y(), m_position.z()),
                       m_target, m_up);
}

void Camera::look()
{
  MyMutexLocker l(m_mutex);

  (void)l;
  //m_proj = glm::perspective(70.f, m_ratio, 0.1f, distanceView);

  this->lookAt();

  if (m_skybox)
    m_skybox->paintGL(m_phi, m_theta);
  /*if (m_character)
    m_character->paintGL(m_view, m_proj);*/
}

void  Camera::update()
{
  vectorsFromAngles();
}

void Camera::mouseMoveEvent(int x, int y)
{
  MyMutexLocker l(m_mutex);

  (void)l;
  //m_oldX = (m_oldX - x);
  //m_oldY = (y - m_oldY);
  m_oldX = -x;
  m_oldY = y;
  if (m_oldX > 100 || m_oldX < -100 ||
      m_oldY > 100 || m_oldY < -100)
    {
      m_oldX = x;
      m_oldY = y;
      return;
    }
  m_theta -= m_oldX * m_sensivity;
  m_phi -= m_oldY * m_sensivity;
  //vectorsFromAngles();
  m_oldX = x;
  m_oldY = y;

  if (m_phi > 89.f)
    m_phi = 89.f;
  else if (m_phi < -89.f)
    m_phi = -89.f;
  tmp1 = m_phi * M_PI / 180.f;
  tmp2 = m_theta * M_PI / 180.f;
}

void Camera::keyPressEvent(int ev)
{
  MyMutexLocker l(m_mutex);

  (void)l;
  switch (ev)
    {
    case SDLK_a:
      m_position = m_position + (m_left * m_speed * tmpSpeed);
      m_target = glm::vec3(m_position.x(), m_position.y(), m_position.z()) + glm::vec3(m_forward.x(), m_forward.y(), m_forward.z());
      break;
    case SDLK_d:
      m_position = m_position - (m_left * m_speed * tmpSpeed);
      m_target = glm::vec3(m_position.x(), m_position.y(), m_position.z()) + glm::vec3(m_forward.x(), m_forward.y(), m_forward.z());
      break;
    case SDLK_w:
      m_position += (m_forward * m_speed * tmpSpeed);
      m_position.setY(m_y);
      break;
    case SDLK_s:
      m_position -= (m_forward * m_speed * tmpSpeed);
      m_position.setY(m_y);
      break;
    case SDLK_SPACE:
      m_y += (m_speed * tmpSpeed);
      m_position.setY(m_y);
      break;
    case SDLK_LCTRL:
      m_y -= (m_speed * tmpSpeed);
      m_position.setY(m_y);
      break;
    case SDLK_LSHIFT:
      if (tmpSpeed < 3.f)
        tmpSpeed = 3.f;
      break;
    default:
      return;
    }
  vectorsFromAngles(false);
}

void Camera::keyReleaseEvent(int e)
{
  MyMutexLocker l(m_mutex);

  (void)l;
  switch (e)
    {
    case SDLK_LSHIFT: //42
      if (tmpSpeed > 1.f)
        tmpSpeed = 1.f;
      break;
    }
}

void Camera::setSpeed(float s)
{
  MyMutexLocker l(m_mutex);

  (void)l;
  m_speed = s;
}

const float &Camera::speed() const
{
  return m_speed;
}

void Camera::setsensivity(float s)
{
  MyMutexLocker l(m_mutex);

  (void)l;
  m_sensivity = s;
}

void Camera::setPosition(const Vector3D &position)
{
  MyMutexLocker l(m_mutex);

  (void)l;
  m_position = position;
  m_target = glm::vec3(m_position.x(), m_position.y(), m_position.z()) + glm::vec3(m_forward.x(), m_forward.y(), m_forward.z());
}

void  Camera::vectorsFromAngles(bool phiChanged)
{
  if (phiChanged) {
      if (m_phi > 89.f)
        m_phi = 89.f;
      else if (m_phi < -89.f)
        m_phi = -89.f;
      tmp1 = m_phi * M_PI / 180.f;
      tmp2 = m_theta * M_PI / 180.f;
    }
  r_temp = cosf(tmp1);
  m_forward.setY(tmp1);
  m_forward.setX(r_temp * cosf(tmp2));
  m_forward.setZ(r_temp * sinf(tmp2));
  //m_forward.setY(0.f);

  /*m_left = Vector3D::crossProduct(m_up, m_forward);
  m_left.normalize();*/
  m_target = glm::vec3(m_position.x(), m_position.y(), m_position.z()) + glm::vec3(m_forward.x(), m_forward.y(), m_forward.z());

  float _tmp1 = 45.f * M_PI / 180.f;
  float _tmp2 = m_theta * M_PI / 180.f;
  r_temp = cosf(_tmp1);
  m_forward.setY(_tmp1);
  m_forward.setX(r_temp * cosf(_tmp2));
  m_forward.setZ(r_temp * sinf(_tmp2));
  //m_forward.setY(0.f);

  m_left = Vector3D::crossProduct(Vector3D(m_up.x, m_up.y, m_up.z), m_forward);
  m_left.normalize();

  m_vecPos.x = m_position.x();
  m_vecPos.y = m_position.y();
  m_vecPos.z = m_position.z();

  /*if (m_character) {
      //m_character->setPosition(m_target);
      m_character->setRotation(Rotation(1.f, 0.f, m_theta));
    }*/
}

Vector3D &Camera::getPosition()
{
  return m_position;
}

glm::vec3 &Camera::getVecPosition()
{
  return m_vecPos;
}

void  Camera::setView(MyWindow *v)
{
  if (!m_skybox){
      m_skybox = new SkyBox;
      m_skybox->initializeGL();
    }
  resize(v->getLib()->width(), v->getLib()->height());
}

int Camera::mouseX() const
{
  return m_oldX;
}

int Camera::mouseY() const
{
  return m_oldY;
}

glm::mat4 const &Camera::getViewMatrix()
{
  return m_view;
}

glm::mat4 const &Camera::getProjectionMatrix()
{
  return m_proj;
}

glm::mat4 const &Camera::get2DProjectionMatrix()
{
  return m_2Dproj;
}

float const &Camera::getRatio()
{
  return m_ratio;
}

glm::vec3 &Camera::getTarget()
{
  return m_target;
}

glm::vec3 &Camera::getUpVector()
{
  return m_up;
}

void  Camera::setCharacter(myGLWidget *w)
{
  if (!w)
    return;
  //m_character->setRotation(Rotation());
}
