#ifndef HANDLE_2048_HPP
#define HANDLE_2048_HPP

#include "myGLWidget.hpp"
#include <vector>
#include "Text.hpp"
#include "Plane.hpp"

class Handle_2048 : public myGLWidget
{
  struct Tile {
    Object::Text  *m_text;
    Object::Plane *m_back;
    float         pos_x;
    float         pos_y;

    Tile() : m_text(0), m_back(0), pos_x(0.f), pos_y(0.f){}
    ~Tile() {
      if (m_text)
        delete m_text;
      if (m_back)
        delete m_back;
    }
    void initializeGL() {
      if (m_text)
        m_text->initializeGL();
      if (m_back)
        m_back->initializeGL();
    }
    void paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix) {
      if (m_text)
        m_text->paintGL(view_matrix, proj_matrix);
      if (m_back)
        m_back->paintGL(view_matrix, proj_matrix);
    }
  };

public:
  Handle_2048();
  virtual ~Handle_2048();
  void  initializeGL();
  void  keyReleaseEvent(int key);
  void  update(const float &);
  void  paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix);

private:
  void  create_back_tile(Vector3D, float size, Color);
  int   m_map[4][4];
  std::vector<std::vector<Tile*> >  m_tiles;
  Object::Plane                     *m_background;
};

#endif // HANDLE_2048_HPP
