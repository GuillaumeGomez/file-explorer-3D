#ifndef HANDLE_2048_HPP
#define HANDLE_2048_HPP

#include "myGLWidget.hpp"
#include <vector>
#include "Text.hpp"
#include "Plane.hpp"
#include "String_utils.hpp"

#define TRANS_TIME 0.3f

class Handle_2048 : public myGLWidget
{
  enum {
    NO_MOVE,
    R_MOVE,
    L_MOVE,
    U_MOVE,
    D_MOVE
  };
  typedef struct Tile {
    float         to_x;
    float         to_y;
    float         pos_x;
    float         pos_y;
    float         from_x;
    float         from_y;
    int           value;
    int           x;
    int           y;
    Object::Text  *m_text;
    Object::Plane *m_back;

    Tile(int y, int x) : to_x(0.f), to_y(0.f), pos_x(0.f), pos_y(0.f), from_x(0.f), from_y(0.f),
      value(0), x(x), y(y), m_text(0), m_back(0) {}
    ~Tile() {
      if (m_text)
        delete m_text;
      if (m_back)
        delete m_back;
    }
    void initializeGL() {
      if (m_text)
        m_text->initializeGL();
      if (m_back) {
          m_back->initializeGL();
          to_x = pos_x = from_x = m_back->getPosition().x();
          to_y = pos_y = from_y = m_back->getPosition().y();
        }
    }
    void paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix) {
      if (!value)
        return;
      if (m_back)
        m_back->paintGL(view_matrix, proj_matrix);
      if (m_text)
        m_text->paintGL(view_matrix, proj_matrix);
    }
    void  endMove() {
      pos_x = from_x = to_x;
      pos_y = from_y = to_y;

      float tmp_x = 0.f, tmp_y = 0.f;

      if (pos_x > to_x)
        tmp_x = to_x - pos_x;
      else
        tmp_x = to_x - pos_x;
      if (pos_y > to_y)
        tmp_y = to_y - pos_y;
      else
        tmp_y = to_y - pos_y;
      pos_x = from_x;
      pos_y = from_y;

      m_text->setPosition(Vector3D(pos_x, pos_y));
      m_back->setPosition(Vector3D(pos_x, pos_y));

      std::vector<GLfloat>  t = m_text->getVertices();
      for (unsigned int i = 0; i < t.size(); i += 2) {
          t[i] += tmp_x;
          t[i + 1] += tmp_y;
        }
      m_text->updateVertices(t);
      std::vector<GLfloat>  t2 = m_back->getVertices();
      for (unsigned int i = 0; i < t2.size(); i += 2) {
          t2[i] += tmp_x;
          t2[i + 1] += tmp_y;
        }
      m_back->updateVertices(t2);
    }

    void  update(const float &t, const float &move) {
      if (!value)
        return;

      if (to_x != from_x || to_y != from_y) {
          float tt = move - t;
          if (tt < 0.f)
            tt = t + tt;
          else
            tt = t;
          float tmp = tt / TRANS_TIME;
          float add_x = tmp * (to_x - from_x);
          float add_y = tmp * (to_y - from_y);

          pos_x += add_x;
          pos_y += add_y;

          m_text->setPosition(Vector3D(pos_x, pos_y));
          m_back->setPosition(Vector3D(pos_x, pos_y));

          std::vector<GLfloat>  t = m_text->getVertices();
          for (unsigned int i = 0; i < t.size(); i += 2) {
              t[i] += add_x;
              t[i + 1] += add_y;
            }
          m_text->updateVertices(t);
          std::vector<GLfloat>  t2 = m_back->getVertices();
          for (unsigned int i = 0; i < t2.size(); i += 2) {
              t2[i] += add_x;
              t2[i + 1] += add_y;
            }
          m_back->updateVertices(t2);
        }
      if (m_text)
        m_text->update(t);
      if (m_back)
        m_back->update(t);
    }
    void  setValue(int x) {
      if (x == value || x & 1)
        return;
      value = x;
      m_text->setText(Utility::toString<int>(value));
      switch (x / 2) {
        case 2:
          m_back->setColor(Color(0.93f, 0.89f, 0.85f));
          m_text->setColor(RED);
          break;
        case 4:
          m_back->setColor(Color(0.93f, 0.88f, 0.78f));
          m_text->setColor(RED);
          break;
        case 8:
          m_back->setColor(Color(0.95f, 0.69f, 0.47f));
          m_text->setColor(WHITE);
          break;
        case 16:
          m_back->setColor(Color(0.96f, 0.58f, 0.39f));
          m_text->setColor(WHITE);
          break;
        case 32:
          m_back->setColor(Color(0.96f, 0.48f, 0.37f));
          m_text->setColor(WHITE);
          break;
        case 64:
          m_back->setColor(Color(0.96f, 0.36f, 0.23f));
          m_text->setColor(WHITE);
          break;
        case 128:
          m_back->setColor(Color(0.93f, 0.81f, 0.45f));
          m_text->setColor(WHITE);
          break;
        case 256:
          m_back->setColor(Color(0.93f, 0.8f, 0.38f));
          m_text->setColor(WHITE);
          break;
        default:
          m_back->setColor(Color(0.93f, 0.78f, 0.31f));
          m_text->setColor(WHITE);
          break;
        }
    }
  } Tile;

public:
  Handle_2048();
  virtual ~Handle_2048();
  void  initializeGL();
  void  keyReleaseEvent(int key);
  void  update(const float &);
  void  paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix);

private:
  void  create_back_tile(Vector3D, float size, Color);
  void  create_new_number();
  void  switchTile(Tile *tile, Tile *tmp, int x, int y);
  const float                       tile_s, border_s;
  int   m_map[4][4];
  std::vector<std::vector<Tile*> >  m_tiles;
  Object::Plane                     *m_background;
  float                             m_move;
  char                              m_direction;
};

#endif // HANDLE_2048_HPP
