#ifndef ROTATION_HPP
#define ROTATION_HPP

#include <Vector3D.hpp>

class Rotation
{
public:
    Rotation(float rot_x = 0.f, float speed_x = 0.f, float rot_y = 0.f, float speed_y = 0.f, float rot_z = 0.f, float speed_z = 0.f);
    Rotation(Rotation const&);
    virtual ~Rotation(){}
    float x();
    float y();
    float z();
    void setX(float);
    void setY(float);
    void setZ(float);
    float speedX();
    float speedY();
    float speedZ();
    void setSpeedX(float);
    void setSpeedY(float);
    void setSpeedZ(float);
    Vector3D  toVector3D() const;
    void  update(const float &);

private:
    float m_rotX;
    float m_speedX;
    float m_rotY;
    float m_speedY;
    float m_rotZ;
    float m_speedZ;
};

#endif // ROTATION_HPP
