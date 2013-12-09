#include "Vector3D.hpp"
#include "../String_utils.hpp"
#include <cmath>

using namespace std;

Vector3D::Vector3D(float x, float y, float z)
{
  pos[0] = x;
  pos[1] = y;
  pos[2] = z;
}

Vector3D::Vector3D(const Vector3D &vector)
{
  pos[0] = vector.pos[0];
  pos[1] = vector.pos[1];
  pos[2] = vector.pos[2];
}

float Vector3D::getDistance(const Vector3D &v) const
{
  return sqrtf(((v.pos[0] - pos[0]) * (v.pos[0] - pos[0])) +
               ((v.pos[1] - pos[1]) * (v.pos[1] - pos[1])) +
               ((v.pos[2] - pos[2]) * (v.pos[2] - pos[2])));
}

Vector3D Vector3D::normalized() const
{
  float l = this->length();

  if (l == 0.f)
    return Vector3D();
  return Vector3D(pos[0] / l, pos[1] / l, pos[2] / l);
}

void Vector3D::normalize()
{
  float l = this->length();

  if (l == 0.f){
      pos[0] = pos[1] = pos[2] = 0.f;
    }
  else {
      pos[0] /= l;
      pos[1] /= l;
      pos[2] /= l;
    }
}

float Vector3D::dotProduct(const Vector3D& v2)
{
  return pos[0] * v2.pos[0] + pos[1] * v2.pos[1] + pos[2] * v2.pos[2];
}

float Vector3D::dotProduct(const Vector3D& v1, const Vector3D& v2)
{
  return v1.pos[0] * v2.pos[0] + v1.pos[1] * v2.pos[1] + v1.pos[2] * v2.pos[2];
}

Vector3D Vector3D::crossProduct(const Vector3D& v2)
{
  return Vector3D(pos[1] * v2.pos[2] - pos[2] * v2.pos[1],
                  pos[2] * v2.pos[0] - pos[0] * v2.pos[2],
                  pos[0] * v2.pos[1] - pos[1] * v2.pos[0]);
}

Vector3D Vector3D::crossProduct(const Vector3D& v1, const Vector3D& v2)
{
  return Vector3D(v1.pos[1] * v2.pos[2] - v1.pos[2] * v2.pos[1],
                  v1.pos[2] * v2.pos[0] - v1.pos[0] * v2.pos[2],
                  v1.pos[0] * v2.pos[1] - v1.pos[1] * v2.pos[0]);
}

Vector3D Vector3D::normal(const Vector3D& v1, const Vector3D& v2)
{
  return crossProduct(v1, v2).normalized();
}

float Vector3D::length() const
{
  return sqrtf(pos[0] * pos[0] + pos[1] * pos[1] + pos[2] * pos[2]);
}

float Vector3D::lengthSquared() const
{
  return pos[0] * pos[0] + pos[1] * pos[1] + pos[2] * pos[2];
}

float Vector3D::x() const
{
  return pos[0];
}

float Vector3D::y() const
{
  return pos[1];
}

float Vector3D::z() const
{
  return pos[2];
}

void  Vector3D::setX(float x)
{
  this->pos[0] = x;
}

void  Vector3D::setY(float y)
{
  this->pos[1] = y;
}

void  Vector3D::setZ(float z)
{
  this->pos[2] = z;
}

Vector3D  Vector3D::operator*(float s) const
{
  return Vector3D(pos[0] * s, pos[1] * s, pos[2] * s);
}

Vector3D  Vector3D::operator*(const Vector3D &v) const
{
  return Vector3D(pos[0] * v.pos[0], pos[1] * v.pos[1], pos[2] * v.pos[2]);
}

void      Vector3D::operator*=(float s)
{
  pos[0] *= s;
  pos[1] *= s;
  pos[2] *= s;
}

void      Vector3D::operator*=(const Vector3D &v)
{
  pos[0] *= v.pos[0];
  pos[1] *= v.pos[1];
  pos[2] *= v.pos[2];
}

Vector3D  Vector3D::operator/(float s) const
{
  return Vector3D(pos[0] / s, pos[1] / s, pos[2] / s);
}

Vector3D  Vector3D::operator/(const Vector3D &v) const
{
  return Vector3D(pos[0] / v.pos[0], pos[1] / v.pos[1], pos[2] / v.pos[2]);
}

void      Vector3D::operator/=(float s)
{
  pos[0] /= s;
  pos[1] /= s;
  pos[2] /= s;
}

void      Vector3D::operator/=(const Vector3D &v)
{
  pos[0] /= v.pos[0];
  pos[1] /= v.pos[1];
  pos[2] /= v.pos[2];
}

void  Vector3D::operator+=(const Vector3D &v)
{
  pos[0] += v.pos[0];
  pos[1] += v.pos[1];
  pos[2] += v.pos[2];
}

Vector3D Vector3D::operator+(const Vector3D &v) const
{
  return Vector3D(pos[0] + v.pos[0], pos[1] + v.pos[1], pos[2] + v.pos[2]);
}

void Vector3D::operator-=(const Vector3D &v)
{
  pos[0] -= v.pos[0];
  pos[1] -= v.pos[1];
  pos[2] -= v.pos[2];
}

Vector3D Vector3D::operator-(const Vector3D &v) const
{
  return Vector3D(pos[0] - v.pos[0], pos[1] - v.pos[1], pos[2] - v.pos[2]);
}

void  Vector3D::operator=(const Vector3D &v)
{
  pos[0] = v.pos[0];
  pos[1] = v.pos[1];
  pos[2] = v.pos[2];
}

bool  Vector3D::operator==(const Vector3D &v) const
{
  return v.pos[0] == this->pos[0] && v.pos[1] == this->pos[1] && v.pos[2] == this->pos[2];
}

bool  Vector3D::operator!=(const Vector3D &v) const
{
  return v.pos[0] != pos[0] || v.pos[1] != pos[1] || v.pos[2] != pos[2];
}

std::string Vector3D::toString() const
{
  std::string s;

  s = "x: " + Utility::toString(pos[0]) + " / y: " + Utility::toString(pos[1]) + " / z: " + Utility::toString(pos[2]);
  return s;
}

float *Vector3D::getDatas()
{
  return pos;
}

std::ostream &operator<<(std::ostream &io, const Vector3D &v)
{
  io << "(" << v.x() << ", " << v.y() << ", " << v.z() << ")";
  return io;
}
