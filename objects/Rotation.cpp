#include "Rotation.hpp"

Rotation::Rotation(float rot, float speed, float x, float y, float z)
  : m_rotation(rot), m_acSpeed(speed), m_rotX(y), m_rotY(x), m_rotZ(z)
{
}

Rotation::Rotation(Rotation const &r)
  : m_rotation(r.m_rotation), m_acSpeed(r.m_acSpeed), m_rotX(r.m_rotX), m_rotY(r.m_rotY), m_rotZ(r.m_rotZ)
{
}

float Rotation::getSpeed() const
{
  return m_acSpeed;
}

void Rotation::setSpeed(float s)
{
  m_acSpeed = s;
}

float Rotation::getRotation()
{
  return m_rotation;
}

void  Rotation::setRotation(float s)
{
  m_rotation = s;
}

float Rotation::getRotX()
{
  return m_rotX;
}

float Rotation::getRotY()
{
  return m_rotY;
}

float Rotation::getRotZ()
{
  return m_rotZ;
}

void  Rotation::setRotX(float s)
{
  //vecteur up oblige...
  m_rotY = s;
}

void  Rotation::setRotY(float s)
{
  m_rotX = s;
}

void  Rotation::setRotZ(float s)
{
  m_rotZ = s;
}

Vector3D  Rotation::toVector3D() const
{
  return Vector3D(m_rotX, m_rotY, m_rotZ);
}

void    Rotation::update(float const &e)
{
    m_rotation += (m_acSpeed * e);
}
