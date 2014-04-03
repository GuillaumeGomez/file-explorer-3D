#ifndef MENU_HPP
#define MENU_HPP

#include "myGLWidget.hpp"

class Menu : public myGLWidget
{
public:
  Menu();
  virtual ~Menu();
  virtual void paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix);
  virtual void update(const float &);
  virtual void initializeGL();

private:
};

#endif // MENU_HPP
