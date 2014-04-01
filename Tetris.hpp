#ifndef TETRIS_HPP
#define TETRIS_HPP

#include "myGLWidget.hpp"

enum {
  SCORE,
  LINE,
  LVL,
  TETRIS_MSG
};

typedef struct
{
  char    rot[4][4][4];
  Color   color;
  GLfloat tex_coord[2];
} PieceDatas;

typedef struct
{
  int id;
  int x;
  int y;
  int rot;
} Piece;

namespace Object {
  class Text;
}

class Tetris : public myGLWidget
{
public:
  Tetris();
  ~Tetris();
  void  paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix);
  void  initializeGL();
  void  update(const float &n);
  void  keyPressEvent(int key);
  void  keyReleaseEvent(int key);

private:
  bool  canMove(Piece &p);
  void  createNewPiece();
  void  setColor(int x, int y, PieceDatas *color, bool ghost = false);
  void  copyInMap(Piece &p, int val = 10);
  void  deleteTmpPiece();
  int   definitivePaste();
  void  restart();
  void  moveGhost();

  int         m_level;
  int         m_score;
  float       m_elapsed;
  bool        m_hasNew;
  int         m_lines;
  bool        m_end;
  bool        m_pause;
  int         m_oldSize;
  char        map[22][10];
  float       m_speeds[12];
  int         m_levels[11];
  PieceDatas  m_pieces[7];
  Piece       m_piece;
  Piece       m_ghost;
  std::vector<Object::Text*>  m_texts;
};

#endif // TETRIS_HPP
