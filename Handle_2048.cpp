#include "Handle_2048.hpp"
#include "Text.hpp"
#include "Plane.hpp"
#include "shaders/ShaderHandler.hpp"
#include "HandleError.hpp"

//-0.55 -0.3 -0.05 0.05 0.3 0.55

const float tile_s = 0.35f, border_s = 0.05f;

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

Handle_2048::Handle_2048() : myGLWidget(Vector3D(), Rotation())
{
  m_render2D = true;
  m_move = 0.f;

  create_back_tile(Vector3D(-0.85f, -0.95f), 1.65f, GREY);
  for (int y = 0; y < 4; ++y) {
      std::vector<Tile*> tmp;
      for (int x = 0; x < 4; ++x) {
          m_map[y][x] = 0;
          Tile *t = new Tile(y, x);

          t->m_text = new Object::Text("", BLACK, x * tile_s - 0.8f + border_s * x, y * tile_s - 0.9f + border_s * y, tile_s - tile_s / 20);
          t->m_back = new Object::Plane(Vector3D(x * tile_s - 0.8f + border_s * x, y * tile_s - 0.9f + border_s * y),
                                        Rotation(), RED, tile_s, tile_s, false);

          if (!t->m_text || !t->m_back)
            exit(-4);

          create_back_tile(t->m_back->getPosition(), tile_s, LGREY);

          t->m_back->setRender2D(true);
          tmp.push_back(t);
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
  //m_background->initializeGL();

  create_new_number();
  create_new_number();
}

void  Handle_2048::update(const float &t)
{
  for (auto &it : m_tiles)
    for (auto it2 : it)
      it2->update(t);
  if (m_move > 0.f) {
      m_move -= t;
      if (m_move < 0.f)
        m_move = 0.f;
    }
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
  /*for (auto &it : m_backs)
    for (auto it2 : it)
      it2->paintGL(view_matrix, proj_matrix);*/
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
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
  m_move = 0.5f;
  for (auto it : m_tiles)
    for (auto it2 : it)
      if (it2->value) {
          it2->from_x = it2->pos_x;
          it2->from_y = it2->pos_y;
          int x;
          switch (m_direction) {
            case R_MOVE:
              for (x = it2->x; x < 3 && !it[x + 1]->value; ++x);
              it2->to_x = it2->from_x + tile_s * (x - it2->x) + border_s * (x - it2->x);
              it2->to_y = it2->from_y;
              break;
            case L_MOVE:
              for (x = it2->x; x > 0 && !it[x - 1]->value; --x);
              it2->to_x = it2->from_x - (tile_s * (it2->x - x) + border_s * (it2->x - x));
              it2->to_y = it2->from_y;
              break;
            case D_MOVE:
              for (x = it2->y; x < 3 && !m_tiles[x + 1][it2->x]->value; ++x);
              it2->to_x = it2->from_x;
              it2->to_y = it2->from_y - (tile_s * (x - it2->y) + border_s * (x - it2->y));
              break;
            case U_MOVE:
              for (x = it2->y; x > 0 && !m_tiles[x - 1][it2->x]->value; --x);
              it2->to_x = it2->from_x;
              it2->to_y = it2->from_y + tile_s * (it2->y - x) + border_s * (it2->y - x);
              break;
            default:
              break;
            }
        }
}

void  Handle_2048::create_new_number()
{
  std::vector<Tile*>  tmp;

  for (auto it : m_tiles)
    for (auto it2 : it)
      if (!it2->value)
        tmp.push_back(it2);

  int z = rand() % tmp.size();
  int r = rand() % 10;
  tmp[z]->setValue(r >= 8 ? 4 : 2); //80% getting a 2

  /*int z = rand() % 16;
  int r = rand() % 10;
  Tile *tmp = m_tiles[z / 4][z % 4];

  if (!tmp->value)
    tmp->value = r > 8 ? 4 : 2;*/
}
