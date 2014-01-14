#ifndef ROTATION_HPP
#define ROTATION_HPP

#include <Vector3D.hpp>

class Rotation
{
public:
  Rotation(float rot = 0.f, float speed = 0.f, float x = 0.f, float y = 0.f, float z = 0.f);
  Rotation(Rotation const&);
  virtual ~Rotation(){}
  float &rotation();
  void  setRotation(float);
  float &speed();
  void  setSpeed(float s);
  float &x();
  float &y();
  float &z();
  void  setRotX(float);
  void  setRotY(float);
  void  setRotZ(float);
  Vector3D  toVector3D() const;
  void  update(const float &);

private:
  float m_rotation;
  float m_acSpeed;
  float m_rotX;
  float m_rotY;
  float m_rotZ;
};

#endif // ROTATION_HPP
