#ifndef VECTOR3D_HPP
#define VECTOR3D_HPP

#include <iostream>

class Vector3D
{
public:
  Vector3D(float x = 0.f, float y = 0.f, float z = 0.f);
  Vector3D(const Vector3D &vector);
  Vector3D  normalized() const;
  void      normalize();
  Vector3D  crossProduct(const Vector3D& v2);
  float     dotProduct(const Vector3D& v2);
  static Vector3D crossProduct(const Vector3D &v1, const Vector3D &v2);
  static float    dotProduct(const Vector3D& v1, const Vector3D& v2);

  float     getDistance(const Vector3D&) const;

  Vector3D  normal(const Vector3D& v1, const Vector3D& v2);
  float     length() const;
  float     lengthSquared() const;
  float     x() const;
  float     y() const;
  float     z() const;
  void      setX(float);
  void      setY(float);
  void      setZ(float);
  Vector3D  operator*(float) const;
  Vector3D  operator*(const Vector3D &v) const;
  void      operator*=(float);
  void      operator*=(const Vector3D &v);
  Vector3D  operator/(float) const;
  Vector3D  operator/(const Vector3D &v)const;
  void      operator/=(float);
  void      operator/=(const Vector3D &v);
  void      operator+=(const Vector3D &v);
  Vector3D  operator+(const Vector3D &v) const;
  void      operator-=(const Vector3D &v);
  Vector3D  operator-(const Vector3D &v) const;
  void      operator=(const Vector3D &v);
  bool      operator==(const Vector3D &v) const;
  bool      operator!=(const Vector3D &v) const;
  std::string toString() const;
  float     *getDatas();

private:
  float   pos[3];
};

std::ostream &operator<<(std::ostream&, const Vector3D&);

#endif // VECTOR3D_HPP
