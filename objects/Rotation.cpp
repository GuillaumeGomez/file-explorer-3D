#include "Rotation.hpp"

Rotation::Rotation(float rot_x, float speed_x, float rot_y, float speed_y, float rot_z, float speed_z)
    : m_rotX(rot_y), m_speedX(speed_y), m_rotY(rot_x), m_speedY(speed_x), m_rotZ(rot_z), m_speedZ(speed_z)
{
}

Rotation::Rotation(Rotation const &r)
    : m_rotX(r.m_rotX), m_speedX(r.m_speedX), m_rotY(r.m_rotY), m_speedY(r.m_speedY), m_rotZ(r.m_rotZ), m_speedZ(r.m_speedZ)
{
}

float &Rotation::x()
{
    return m_rotX;
}

float &Rotation::y()
{
    return m_rotY;
}

float &Rotation::z()
{
    return m_rotZ;
}

float &Rotation::speedX()
{
    return m_speedX;
}

float &Rotation::speedY()
{
    return m_speedY;
}

float &Rotation::speedZ()
{
    return m_speedZ;
}

Vector3D  Rotation::toVector3D() const
{
    return Vector3D(m_rotX, m_rotY, m_rotZ);
}

void    Rotation::update(float const &e)
{
    m_rotX += (m_speedX * e);
    m_rotY += (m_speedY * e);
    m_rotZ += (m_speedZ * e);
}
