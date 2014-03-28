#include "Handle_2048.hpp"
#include "Text.hpp"
#include "Plane.hpp"
#include "shaders/ShaderHandler.hpp"
#include "HandleError.hpp"

//-0.55 -0.3 -0.05 0.05 0.3 0.55

void  Handle_2048::create_back_tile(Vector3D v, float size, Color c)
{
  m_vertices.push_back(v.x());
  m_vertices.push_back(v.y());
  m_vertices.push_back(v.x());
  m_vertices.push_back(v.y() + size);
  m_vertices.push_back(v.x() + size);
  m_vertices.push_back(v.y() + size);

  m_vertices.push_back(v.x());
  m_vertices.push_back(v.y());
  m_vertices.push_back(v.x() + size);
  m_vertices.push_back(v.y() + size);
  m_vertices.push_back(v.x() + size);
  m_vertices.push_back(v.y());

  for (int x = 0; x < 6; ++x) {
      m_colors.push_back(c.red());
      m_colors.push_back(c.green());
      m_colors.push_back(c.blue());
    }
}

Handle_2048::Handle_2048() : myGLWidget(Vector3D(), Rotation()),
  tile_s(0.35f), border_s(0.05f)
{
  m_render2D = true;
  m_move = 0.f;

  create_back_tile(Vector3D(-0.85f, -0.95f), 1.65f, GREY);
  float decal = tile_s + border_s;
  for (int y = 0; y < 4; ++y) {
      std::vector<Tile*> tmp;
      for (int x = 0; x < 4; ++x) {
          Tile *t = new Tile(y, x);
          float x_pos = decal * x - 0.8f;
          float y_pos = 0.3f - y * decal;

          t->m_text = new Object::Text("", BLACK, x_pos, y_pos, tile_s - tile_s / 20);
          t->m_back = new Object::Plane(Vector3D(x_pos, y_pos),
                                        Rotation(), RED, tile_s, tile_s, false);

          if (!t->m_text || !t->m_back)
            exit(-4);

          create_back_tile(t->m_back->getPosition(), tile_s, LGREY);

          t->m_back->setRender2D(true);
          tmp.push_back(t);
          m_move_tiles.push_back(new Tile(tile_s, x_pos, y_pos));
        }
      m_tiles.push_back(tmp);
    }

  //m_background = new Object::Plane(Vector3D(-0.85f, -0.95f), Rotation(), GREY, 1.65f, 1.65f, false);

  /*if (!m_background)
    exit(-5);
  m_background->setRender2D(true);*/
}

Handle_2048::~Handle_2048()
{
  for (auto &it : m_tiles)
    for (auto it2 : it)
      delete it2;
  //delete m_background;
}

void  Handle_2048::initializeGL()
{
  m_shader = ShaderHandler::createShader(Shader::getStandard2DVertexShader(false), Shader::getStandard2DFragmentShader(false));
  if (!m_shader){
      HandleError::showError("Shader didn't load in Handle_2048");
      exit(-1);
    }

  m_pointsNumber = m_vertices.size() / 2;

  this->initVertexBufferObject();
  this->initVertexArrayObject();

  for (auto &it : m_tiles)
    for (auto it2 : it)
      it2->initializeGL();
  for (auto it : m_move_tiles)
    it->initializeGL();
  //m_background->initializeGL();

  create_new_number();
  create_new_number();
}

void  Handle_2048::update(const float &t)
{
  for (auto &it : m_move_tiles)
    it->update(t, m_move);
  if (m_move > 0.f) {
      m_move -= t;
      if (m_move <= 0.f) {
          m_move = 0.f;
          /*for (auto &it : m_tiles)
            for (auto it2 : it)
              it2->endMove();*/
          int z;
          for (z = 0; z < 16 && m_move_tiles[z]->value; ++z) {
              Tile *ti = m_move_tiles[z];
              Tile *tt = m_tiles[ti->y][ti->x];

              /*if (tt->value > 0)
                tt->m_hide = false;*/
              tt->setValue(ti->value + tt->value);
              tt->m_valueChanged = false;
              ti->setValue(0);
            }
          for (auto &it : m_tiles)
            for (auto it2 : it) {
                it2->m_hide = false;
                it2->m_valueChanged = false;
              }
          // to add when tiles move will work
          if (z)
            create_new_number();
        }
    }
  for (auto &it : m_tiles)
    for (auto it2 : it)
      it2->update(t, m_move);
}

void  Handle_2048::paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix)
{
  //(void)view_matrix;
  //(void)proj_matrix;
  glUseProgram(m_shader->getProgramID());

  glBindVertexArray(m_vaoID);

  glEnable(GL_TEXTURE_2D);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //m_background->paintGL(view_matrix, proj_matrix);

  glDrawArrays(GL_TRIANGLES, 0, m_pointsNumber);

  for (auto &it : m_tiles)
    for (auto it2 : it)
      it2->paintGL(view_matrix, proj_matrix);
  if (m_move > 0.f)
    for (auto it : m_move_tiles)
      it->paintGL(view_matrix, proj_matrix);
  /*for (auto &it : m_backs)
    for (auto it2 : it)
      it2->paintGL(view_matrix, proj_matrix);*/
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
}

//void  Handle_2048::switchTile(Tile *tile, Tile *tmp, int x, int y)
void  Handle_2048::switchTile(Tile *tile, Tile *tmp, int &z, int x, int y)
{
  if (x == tile->x && y == tile->y)
    return;

  if (tmp->value && tmp->value != tile->value)
    return;

  Tile *move = m_move_tiles[z++];
  /*tile->to_x = tmp->pos_x;
  tile->to_y = tmp->pos_y;

  if (x != tile->x) {
      m_tiles[y][x] = tile;
      m_tiles[y][tile->x] = tmp;
    } else {
      m_tiles[y][tile->x] = tile;
      m_tiles[tile->y][tile->x] = tmp;
    }

  tmp->x = tile->x;
  tmp->y = tile->y;
  tmp->pos_x = tmp->to_x = tmp->from_x = tile->pos_x;
  tmp->pos_y = tmp->to_y = tmp->from_y = tile->pos_y;
  tile->x = x;
  tile->y = y;*/
  /*move->setValue(tile->value);
  move->setPos(tile->pos_x, tile->pos_y, true);*/
  (*move) = (*tile);
  move->to_x = tmp->pos_x;
  move->to_y = tmp->pos_y;
  move->x = tmp->x;
  move->y = tmp->y;
  /*if (tmp->value)
    tmp->m_valueChanged = true;*/
  //tmp->value = tmp->value + tile->value;
  /*if (tmp->value)
    tmp->value += tile->value;*/
  tile->setValue(0);
}

bool Handle_2048::canMove(int y, int x, int value)
{
  int nb(0);

  for (int i = 0; i < 16 && m_move_tiles[i]->value; ++i) {
      if (m_move_tiles[i]->x == x && m_move_tiles[i]->y == y) {
          if (m_move_tiles[i]->value != value)
            return false;
          ++nb;
        }
    }
  if (nb > 1)
    return false;
  if (nb == 1 && m_tiles[y][x]->value)
    return false;
  return m_tiles[y][x]->value == value || !m_tiles[y][x]->value;
}

void  Handle_2048::keyReleaseEvent(int key)
{
  if (m_move > 0.f)
    return;
  switch (key) {
    case SDLK_RIGHT:
      m_direction = R_MOVE;
      break;
    case SDLK_LEFT:
      m_direction = L_MOVE;
      break;
    case SDLK_UP:
      m_direction = U_MOVE;
      break;
    case SDLK_DOWN:
      m_direction = D_MOVE;
      break;
    default:
      m_direction = NO_MOVE;
      return;
    }
  int z(0);
  for (int i_y = (m_direction == D_MOVE ? 3 : 0); (m_direction == D_MOVE ? i_y >= 0 : i_y < 4);
       (m_direction == D_MOVE ? --i_y : ++i_y)) {
      for (int i_x = (m_direction == R_MOVE ? 3 : 0); (m_direction == R_MOVE ? i_x >= 0 : i_x < 4);
           (m_direction == R_MOVE ? --i_x : ++i_x)) {
          Tile *it2 = m_tiles[i_y][i_x];
          if (it2->value) {
              int x;
              switch (m_direction) {
                case R_MOVE:
                  for (x = it2->x; x < 3 && canMove(i_y, x + 1, it2->value) == true; ++x);
                  //switchTile(it2, m_tiles[it2->y][x], x, i_y);
                  switchTile(it2, m_tiles[i_y][x], z, x, i_y);
                  break;
                case L_MOVE:
                  for (x = it2->x; x > 0 && canMove(i_y, x - 1, it2->value) == true; --x);
                  //switchTile(it2, m_tiles[it2->y][x], x, i_y);
                  switchTile(it2, m_tiles[i_y][x], z, x, i_y);
                  break;
                case D_MOVE:
                  for (x = it2->y; x < 3 && canMove(x + 1, i_x, it2->value) == true; ++x);
                  //switchTile(it2, m_tiles[x][i_x], i_x, x);
                  switchTile(it2, m_tiles[x][i_x], z, i_x, x);
                  break;
                case U_MOVE:
                  for (x = it2->y; x > 0 && canMove(x - 1, i_x, it2->value) == true; --x);
                  //switchTile(it2, m_tiles[x][i_x], i_x, x);
                  switchTile(it2, m_tiles[x][i_x], z, i_x, x);
                  break;
                default:
                  break;
                }
            }
        }
    }
  if (z > 0)
    m_move = TRANS_TIME;
}

void  Handle_2048::create_new_number()
{
  std::vector<Tile*>  tmp;

  for (auto it : m_tiles)
    for (auto it2 : it)
      if (!it2->value)
        tmp.push_back(it2);

  if (!tmp.size())
    return;
  int z = rand() % tmp.size();
  int r = rand() % 10;
  tmp[z]->setValue(r >= 9 ? 4 : 2); //90% getting a 2

  /*int z = rand() % 16;
  int r = rand() % 10;
  Tile *tmp = m_tiles[z / 4][z % 4];

  if (!tmp->value)
    tmp->value = r > 8 ? 4 : 2;*/
}
