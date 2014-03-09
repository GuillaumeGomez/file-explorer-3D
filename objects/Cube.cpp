#include "Cube.hpp"
#include <cmath>

using namespace Object;

Cube::Cube(Vector3D p, Rotation r, Color c, float size) : Rectangle(p, r, c, size, size, size)
{
  m_className = "Cube";
}

Cube::Cube(Vector3D p, Rotation r, std::string c, float size) : Rectangle(p, r, c, size, size, size)
{
  m_className = "Cube";
}
