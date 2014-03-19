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

Handle_2048::Handle_2048() : myGLWidget(Vector3D(), Rotation())
{
  const float tile_s = 0.35f, border_s = 0.05f;
  m_render2D = true;

  create_back_tile(Vector3D(-0.85f, -0.95f), 1.65f, GREY);
  for (int y = 0; y < 4; ++y) {
      std::vector<Tile*> tmp;
      for (int x = 0; x < 4; ++x) {
          m_map[y][x] = 0;
          Tile *t = new Tile;

          t->m_text = new Object::Text("", x * 0.25f - 0.55f, y * -0.25f + 0.55f);
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
}

void  Handle_2048::update(const float &t)
{
  (void)t;
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
  (void)key;
}
