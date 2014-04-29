#include "HeightMap.hpp"
#include "HandleSDL.hpp"
#include "../HandleError.hpp"
#include "../shaders/ShaderHandler.hpp"
#include "../String_utils.hpp"
#include "../Utils/PerlinNoise.hpp"

using namespace Object;

void  HeightMap::generateRandomHeightMap()
{
  SDL_Surface *s = SDL_CreateRGBSurface(SDL_SWSURFACE, m_width, m_height, 32, 0, 0, 0, 0);

  // Create a PerlinNoise object with a random permutation vector generated with seed
  //unsigned int seed = 237;
  PerlinNoise pn(rand());

  unsigned int kk = 0;
  // Visit every pixel of the image and assign a color generated with Perlin noise
  for(unsigned int i = 0; i < m_height; ++i) {     // y
      for(unsigned int j = 0; j < m_width; ++j) {  // x
          double x = (double)j/((double)m_width);
          double y = (double)i/((double)m_height);

          // Typical Perlin noise
          double n = pn.noise(x, y, 140.);

          // Wood like structure
          //n = 20 * pn.noise(x, y, 0.8);
          //n = n - floor(n);

          // Map the values to the [0, 255] interval, for simplicity we use
          // tones of grey
          ((char*)s->pixels)[kk++] = floor(255 * n);
          ((char*)s->pixels)[kk++] = floor(255 * n);
          ((char*)s->pixels)[kk++] = floor(255 * n);
          ((char*)s->pixels)[kk++] = 0;
        }
    }

  data_ptr = static_cast<void*>(s);
}

HeightMap::HeightMap(Vector3D v, unsigned int width, unsigned int height, float case_size)
  : myGLWidget(v, Rotation()), m_width(width), m_height(height), m_case_size(case_size),
    m_tex_repeat(m_case_size * 3.f / 4.f), data_ptr(0)
{
  for (int i = 0; i < 4; ++i)
    m_tex[i] = 0;
  m_className = "HeightMap";
  this->generateRandomHeightMap();
}

HeightMap::HeightMap(Vector3D v, string img, float case_size)
  : myGLWidget(v, Rotation()), m_width(0), m_height(0), m_case_size(case_size),
    m_tex_repeat(m_case_size * 3.f / 4.f), data_ptr(0), m_img(img)
{
  for (int i = 0; i < 4; ++i)
    m_tex[i] = 0;
  m_className = "HeightMap";
}

HeightMap::~HeightMap()
{
  for (int i = 0; i < 4; ++i)
    delete m_tex[i];
}

void createTriangle(std::vector<GLfloat> &m_vertices, glm::vec3 s1, glm::vec3 s2, glm::vec3 s3)
{
  for (int x = 0; x < 3; ++x)
    m_vertices.push_back(s1[x]);
  for (int x = 0; x < 3; ++x)
    m_vertices.push_back(s2[x]);
  for (int x = 0; x < 3; ++x)
    m_vertices.push_back(s3[x]);
}

void  HeightMap::initializeGL()
{
  std::string vert =
      "#version 330\n"

      "in vec3 in_Vertex;\n"
      "in vec2 in_TexCoord0;\n"
      "in vec3 in_Normal;\n"

      "uniform mat4 projection;\n"
      "uniform mat4 modelview;\n"
      "uniform vec3 _pos;\n"

      "out vec2 coordTexture;\n"
      "out vec3 vNormal;\n"
      "out float _height;\n"

      "mat4 my_translate(mat4 ori, vec3 t_pos){\n"
      "mat4 tmp = ori;\n"
      "tmp[3] = ori[0] * t_pos.x + ori[1] * t_pos.y + ori[2] * t_pos.z + ori[3];\n"
      "return tmp;\n}\n"

      "void main(){\n"
      "gl_Position = projection * my_translate(modelview, _pos) * vec4(in_Vertex, 1.0);\n"
      "coordTexture = in_TexCoord0;\n"
      "vNormal = in_Normal;\n"
      "_height = in_Vertex.y;"
      "}";


  std::string frag =
      "#version 330\n"

      "in vec2 coordTexture;\n"
      "in vec3 vNormal;\n"

      "in float _height;\n"

      "uniform sampler2D gSampler[4];\n"

      "uniform float fRenderHeight;\n"

      "out vec4 outputColor;\n"

      "void main(){\n"
      "float fScale = _height / fRenderHeight;\n"

      "const float fRange1 = 0.15f;\n"
      "const float fRange2 = 0.3f;\n"
      "const float fRange3 = 0.65f;\n"
      "const float fRange4 = 0.85f;\n"

      "outputColor = vec4(0.0);\n"

      "if(fScale <= fRange1)\n"
      "outputColor = texture(gSampler[0], coordTexture);\n"
      //"vTexColor = texture(gSampler[0], coordTexture);\n"
      "else if(fScale <= fRange2 - 0.01){\n"
      "float f1 = (fScale - fRange1) / (fRange2 - fRange1);\n"
      "float f2 = 1 - f1;\n"
      "outputColor += texture(gSampler[0], coordTexture) * f2;\n"
      "outputColor += texture(gSampler[1], coordTexture) * f1;}\n"
      //"outputColor = mix(texture(gSampler[0], coordTexture), texture(gSampler[1], coordTexture), (fScale - fRange1) / (fRange2 - fRange1));}\n"
      "else if(fScale <= fRange3 - 0.01){\n"
      "float f1 = (fScale - fRange2) / (fRange3 - fRange2);\n"
      "float f2 = 1 - f1;\n"
      "outputColor += texture(gSampler[1], coordTexture) * f2;\n"
      "outputColor += texture(gSampler[2], coordTexture) * f1;}\n"
      //"outputColor = mix(texture(gSampler[1], coordTexture), texture(gSampler[2], coordTexture), (fScale - fRange2) / (fRange3 - fRange2));}\n"
      "else if(fScale <= fRange4 - 0.01){\n"
      "float f1 = (fScale - fRange3) / (fRange4 - fRange3);\n"
      "float f2 = 1 - f1;\n"
      "outputColor += texture(gSampler[2], coordTexture) * f2;\n"
      "outputColor += texture(gSampler[3], coordTexture) * f1;}\n"
      //"outputColor = mix(texture(gSampler[2], coordTexture), texture(gSampler[3], coordTexture), (fScale - fRange3) / (fRange4 - fRange3));}\n"
      "else\n"
      "outputColor = texture(gSampler[3], coordTexture);\n"
      "}\n";

  m_shader = ShaderHandler::getInstance()->createShader(vert, frag);
  if (!m_shader){
      HandleError::showError("Shader didn't load in HeightMap");
      exit(-1);
    }
  m_uniLoc_projection = glGetUniformLocation(m_shader->getProgramID(), "projection");
  m_uniLoc_modelView = glGetUniformLocation(m_shader->getProgramID(), "modelview");
  m_uniloc_pos = glGetUniformLocation(m_shader->getProgramID(), "_pos");
  m_uniloc_height = glGetUniformLocation(m_shader->getProgramID(), "fRenderHeight");

  std::string s[] = {"gSampler[0]", "gSampler[1]", "gSampler[2]", "gSampler[3]"};
  for (int i = 0; i < 4; ++i) {
      m_uniloc_tex.push_back(glGetUniformLocation(m_shader->getProgramID(), s[i].c_str()));
    }

  std::string tex_name[] = {"sand.jpg", "sand_grass_02.jpg", "fungus.jpg", "rock_2_4w.jpg"};

  for (auto &tmp : tex_name)
    tmp = "textures/heightmap/" + tmp;
  for (int i = 0; i < 4; ++i) {
      m_tex[i] = new Texture(tex_name[i]);
      if (!m_tex[i]->load()) {
          HandleError::showError(m_tex[i]->getTextureName() + ": error with file");
          exit(-1);
        }
    }
  SDL_Surface *img = static_cast<SDL_Surface*>(data_ptr);

  if (!img && !m_img.empty()) {
      img = HandleSDL::loadImage(m_img);
      m_width = img->w;
      m_height = img->h;
    }

  if (!img) {
      HandleError::showError("Invalid image: " + m_img);
      exit(0);
    }

  std::vector<std::vector<glm::vec3> >  tmp_v(m_height, std::vector<glm::vec3>(m_width));
  bool min_set(false);
  const float fscaleX = m_tex_repeat / m_width, fscaleY = m_tex_repeat / m_height;

  for (unsigned int i = 0; i < m_width; ++i)
    for (unsigned int j = 0; j < m_height; ++j) {
        Color tmp = HandleSDL::getPixelColor(img, i, j);

        float tmp_h = (tmp.blue() + tmp.green() + tmp.red()) * 10.f * m_case_size;
        tmp_v[j][i] = glm::vec3(i * m_case_size, tmp_h, j * m_case_size);
        if (!min_set) {
            min_height = max_height = tmp_h;
            min_set = true;
          }
        else if (tmp_h < min_height) {
            min_height = tmp_h;
          }
        else if (tmp_h > max_height) {
            max_height = tmp_h;
          }
      }
  HandleSDL::freeSurface(img);

  //convert into GL_TRIANGLE_STRIP after
  height = max_height - min_height;
  float tmp_x(0.f), tmp_y(0.f);
  for (unsigned int y = 0; y < m_height - 1;) {
      for (unsigned int x = 0; x < m_width; ++x) {
          m_vertices.push_back(tmp_v[y][x][0]);
          m_vertices.push_back(tmp_v[y][x][1] - min_height);
          m_vertices.push_back(tmp_v[y][x][2]);
          m_textures.push_back(tmp_x);
          m_textures.push_back(tmp_y);

          m_vertices.push_back(tmp_v[y + 1][x][0]);
          m_vertices.push_back(tmp_v[y + 1][x][1] - min_height);
          m_vertices.push_back(tmp_v[y + 1][x][2]);
          m_textures.push_back(tmp_x);
          m_textures.push_back(tmp_y + fscaleY);

          tmp_x += fscaleX;
        }
      ++y;
      tmp_y += fscaleY;
      if (y < m_height - 1)
        for (int x = m_width - 1; x >= 0; --x) {
            tmp_x -= fscaleX;

            m_vertices.push_back(tmp_v[y][x][0]);
            m_vertices.push_back(tmp_v[y][x][1] - min_height);
            m_vertices.push_back(tmp_v[y][x][2]);
            m_textures.push_back(tmp_x);
            m_textures.push_back(tmp_y);

            m_vertices.push_back(tmp_v[y + 1][x][0]);
            m_vertices.push_back(tmp_v[y + 1][x][1] - min_height);
            m_vertices.push_back(tmp_v[y + 1][x][2]);
            m_textures.push_back(tmp_x);
            m_textures.push_back(tmp_y + fscaleY);
          }
      ++y;
      tmp_x = 0.f;
      tmp_y += fscaleY;
    }

  m_pointsNumber = m_vertices.size() / 3;

  this->initVertexBufferObject();
  this->initVertexArrayObject();
}

void  HeightMap::paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix)
{
  glUseProgram(m_shader->getProgramID());

  glBindVertexArray(m_vaoID);

  glUniformMatrix4fv(m_uniLoc_projection, 1, GL_FALSE, glm::value_ptr(proj_matrix));

  glUniformMatrix4fv(m_uniLoc_modelView, 1, GL_FALSE, glm::value_ptr(view_matrix));
  glUniform3fv(m_uniloc_pos, 1, glm::value_ptr(glm::vec3(m_pos.x(), m_pos.y(), m_pos.z())));
  glUniform1f(m_uniloc_height, height);

  for (unsigned int i = 0; i < m_uniloc_tex.size(); ++i) {

      glBindMultiTextureEXT(GL_TEXTURE0 + i, GL_TEXTURE_2D, m_tex[i]->getTextureID());
      //m_tex[i]->bindTexture(i);
      m_shader->setUniform(m_uniloc_tex[i], (int)i);
    }

  glDrawArrays(GL_TRIANGLE_STRIP, 0, m_pointsNumber);

  glBindVertexArray(0);
  glUseProgram(0);
}

float const &HeightMap::getMinHeight() const
{
  return min_height;
}

float const &HeightMap::getMaxHeight() const
{
  return max_height;
}

unsigned int HeightMap::getWidth() const
{
  return m_width;
}

unsigned int HeightMap::getHeight() const
{
  return m_height;
}

float const &HeightMap::getCaseSize() const
{
  return m_case_size;
}
