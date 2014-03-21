#include "Vector3D.hpp"
#include "../String_utils.hpp"
#include <cmath>

using namespace std;

Vector3D::Vector3D(float x, float y, float z)
{
  pos[VEC_X] = x;
  pos[VEC_Y] = y;
  pos[VEC_Z] = z;
}

Vector3D::Vector3D(const Vector3D &vector)
{
  pos[VEC_X] = vector.pos[VEC_X];
  pos[VEC_Y] = vector.pos[VEC_Y];
  pos[VEC_Z] = vector.pos[VEC_Z];
}

float Vector3D::getDistance(const Vector3D &v) const
{
  return sqrtf(((v.pos[VEC_X] - pos[VEC_X]) * (v.pos[VEC_X] - pos[VEC_X])) +
               ((v.pos[VEC_Y] - pos[VEC_Y]) * (v.pos[VEC_Y] - pos[VEC_Y])) +
               ((v.pos[VEC_Z] - pos[VEC_Z]) * (v.pos[VEC_Z] - pos[VEC_Z])));
}

Vector3D Vector3D::normalized() const
{
  float l = this->length();

  if (l == 0.f)
    return Vector3D();
  return Vector3D(pos[VEC_X] / l, pos[VEC_Y] / l, pos[VEC_Z] / l);
}

void Vector3D::normalize()
{
  float l = this->length();

  if (l == 0.f){
      pos[VEC_X] = pos[VEC_Y] = pos[VEC_Z] = 0.f;
    }
  else {
      pos[VEC_X] /= l;
      pos[VEC_Y] /= l;
      pos[VEC_Z] /= l;
    }
}

float Vector3D::dotProduct(const Vector3D& v2)
{
  return pos[VEC_X] * v2.pos[VEC_X] + pos[VEC_Y] * v2.pos[VEC_Y] + pos[VEC_Z] * v2.pos[VEC_Z];
}

float Vector3D::dotProduct(const Vector3D& v1, const Vector3D& v2)
{
  return v1.pos[VEC_X] * v2.pos[VEC_X] + v1.pos[VEC_Y] * v2.pos[VEC_Y] + v1.pos[VEC_Z] * v2.pos[VEC_Z];
}

Vector3D Vector3D::crossProduct(const Vector3D& v2)
{
  return Vector3D(pos[VEC_Y] * v2.pos[VEC_Z] - pos[VEC_Z] * v2.pos[VEC_Y],
                  pos[VEC_Z] * v2.pos[VEC_X] - pos[VEC_X] * v2.pos[VEC_Z],
                  pos[VEC_X] * v2.pos[VEC_Y] - pos[VEC_Y] * v2.pos[VEC_X]);
}

Vector3D Vector3D::crossProduct(const Vector3D& v1, const Vector3D& v2)
{
  return Vector3D(v1.pos[VEC_Y] * v2.pos[VEC_Z] - v1.pos[VEC_Z] * v2.pos[VEC_Y],
                  v1.pos[VEC_Z] * v2.pos[VEC_X] - v1.pos[VEC_X] * v2.pos[VEC_Z],
                  v1.pos[VEC_X] * v2.pos[VEC_Y] - v1.pos[VEC_Y] * v2.pos[VEC_X]);
}

Vector3D Vector3D::normal(const Vector3D& v1, const Vector3D& v2)
{
  return crossProduct(v1, v2).normalized();
}

float Vector3D::length() const
{
  return sqrtf(pos[VEC_X] * pos[VEC_X] + pos[VEC_Y] * pos[VEC_Y] + pos[VEC_Z] * pos[VEC_Z]);
}

float Vector3D::lengthSquared() const
{
  return pos[VEC_X] * pos[VEC_X] + pos[VEC_Y] * pos[VEC_Y] + pos[VEC_Z] * pos[VEC_Z];
}

float &Vector3D::x()
{
  return pos[VEC_X];
}

float &Vector3D::y()
{
  return pos[VEC_Y];
}

float &Vector3D::z()
{
  return pos[VEC_Z];
}

float const &Vector3D::getX() const
{
  return pos[VEC_X];
}

float const &Vector3D::getY() const
{
  return pos[VEC_Y];
}

float const &Vector3D::getZ() const
{
  return pos[VEC_Z];
}

void  Vector3D::setX(float x)
{
  this->pos[VEC_X] = x;
}

void  Vector3D::setY(float y)
{
  this->pos[VEC_Y] = y;
}

void  Vector3D::setZ(float z)
{
  this->pos[VEC_Z] = z;
}

Vector3D  Vector3D::operator*(float s) const
{
  return Vector3D(pos[VEC_X] * s, pos[VEC_Y] * s, pos[VEC_Z] * s);
}

Vector3D  Vector3D::operator*(const Vector3D &v) const
{
  return Vector3D(pos[VEC_X] * v.pos[VEC_X], pos[VEC_Z] * v.pos[VEC_Y], pos[VEC_Z] * v.pos[VEC_Z]);
}

void      Vector3D::operator*=(float s)
{
  pos[VEC_X] *= s;
  pos[VEC_Y] *= s;
  pos[VEC_Z] *= s;
}

void      Vector3D::operator*=(const Vector3D &v)
{
  pos[VEC_X] *= v.pos[VEC_X];
  pos[VEC_Y] *= v.pos[VEC_Y];
  pos[VEC_Z] *= v.pos[VEC_Z];
}

Vector3D  Vector3D::operator/(float s) const
{
  return Vector3D(pos[VEC_X] / s, pos[VEC_Y] / s, pos[VEC_Z] / s);
}

Vector3D  Vector3D::operator/(const Vector3D &v) const
{
  return Vector3D(pos[VEC_X] / v.pos[VEC_X], pos[VEC_Y] / v.pos[VEC_Y], pos[VEC_Z] / v.pos[VEC_Z]);
}

void      Vector3D::operator/=(float s)
{
  pos[VEC_X] /= s;
  pos[VEC_Y] /= s;
  pos[VEC_Z] /= s;
}

void      Vector3D::operator/=(const Vector3D &v)
{
  pos[VEC_X] /= v.pos[VEC_X];
  pos[VEC_Y] /= v.pos[VEC_Y];
  pos[VEC_Z] /= v.pos[VEC_Z];
}

void  Vector3D::operator+=(const Vector3D &v)
{
  pos[VEC_X] += v.pos[VEC_X];
  pos[VEC_Y] += v.pos[VEC_Y];
  pos[VEC_Z] += v.pos[VEC_Z];
}

Vector3D Vector3D::operator+(const Vector3D &v) const
{
  return Vector3D(pos[VEC_X] + v.pos[VEC_X], pos[VEC_Y] + v.pos[VEC_Y], pos[VEC_Z] + v.pos[VEC_Z]);
}

void Vector3D::operator-=(const Vector3D &v)
{
  pos[VEC_X] -= v.pos[VEC_X];
  pos[VEC_Y] -= v.pos[VEC_Y];
  pos[VEC_Z] -= v.pos[VEC_Z];
}

Vector3D Vector3D::operator-(const Vector3D &v) const
{
  return Vector3D(pos[VEC_X] - v.pos[VEC_X], pos[VEC_Y] - v.pos[VEC_Y], pos[VEC_Z] - v.pos[VEC_Z]);
}

void  Vector3D::operator=(const Vector3D &v)
{
  pos[VEC_X] = v.pos[VEC_X];
  pos[VEC_Y] = v.pos[VEC_Y];
  pos[VEC_Z] = v.pos[VEC_Z];
}

bool  Vector3D::operator==(const Vector3D &v) const
{
  return v.pos[VEC_X] == this->pos[VEC_X] && v.pos[VEC_Y] == this->pos[VEC_Y] && v.pos[VEC_Z] == this->pos[VEC_Z];
}

bool  Vector3D::operator!=(const Vector3D &v) const
{
  return v.pos[VEC_X] != pos[VEC_X] || v.pos[VEC_Y] != pos[VEC_Y] || v.pos[VEC_Z] != pos[VEC_Z];
}

std::string Vector3D::toString() const
{
  std::string s;

  s = "x: " + Utility::toString(pos[VEC_X]) + " / y: " + Utility::toString(pos[VEC_Y]) + " / z: " + Utility::toString(pos[VEC_Z]);
  return s;
}

const float *Vector3D::getDatas() const
{
  return pos;
}

std::ostream &operator<<(std::ostream &io, const Vector3D &v)
{
  io << "(" << v.getX() << ", " << v.getY() << ", " << v.getZ() << ")";
  return io;
}
