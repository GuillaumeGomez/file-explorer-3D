#include "Menu.hpp"
#include "Rotation.hpp"

Menu::Menu() : myGLWidget(Vector3D(), Rotation())
{
}

Menu::~Menu()
{
}

void  Menu::initializeGL()
{
  //m_vertices.push_back();
}

void  Menu::paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix)
{
  (void)view_matrix;
  (void)proj_matrix;
}

void  Menu::update(const float &t)
{
  (void)t;
}
