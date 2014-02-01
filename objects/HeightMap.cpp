#include "HeightMap.hpp"
#include "HandleSDL.hpp"
#include "../HandleError.hpp"
#include "../shaders/ShaderHandler.hpp"
#include "../String_utils.hpp"

#define _PAD  2.f

using namespace Object;

HeightMap::HeightMap(Vector3D v, unsigned int size) : myGLWidget(v, Rotation()), m_width(size), m_height(size)
{
  for (int i = 0; i < 5; ++i)
    m_tex[i] = 0;
}

HeightMap::HeightMap(Vector3D v, string img) : myGLWidget(v, Rotation()), m_width(0), m_height(0), m_img(img)
{
  for (int i = 0; i < 5; ++i)
    m_tex[i] = 0;
}

HeightMap::~HeightMap()
{
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
      "out vec3 vWorldPos;\n"

      "mat4 my_translate(mat4 ori, vec3 t_pos){\n"
      "mat4 tmp = ori;\n"
      "tmp[3] = ori[0] * t_pos.x + ori[1] * t_pos.y + ori[2] * t_pos.z + ori[3];\n"
      "return tmp;\n}\n"

      "void main(){\n"
      "vec4 inPositionScaled = modelview * vec4(_pos, 1.0);\n"
      "gl_Position = projection * my_translate(modelview, _pos) * vec4(in_Vertex, 1.0);\n"
      "coordTexture = in_TexCoord0;\n"
      "vNormal = in_Normal;\n"
      "vec4 vWorldPosLocal = modelview*inPositionScaled;\n"
      "vWorldPos = vWorldPosLocal.xyz;\n"
      "}";


  std::string frag =
      "#version 330\n"

      "in vec2 vTexCoord;\n"
      "in vec3 vNormal;\n"

      "in vec3 vWorldPos;\n"

      "uniform sampler2D gSampler[5];\n"

      "struct DirectionalLight{\n"
      "vec3 vColor;\n"
      "vec3 vDirection;\n"
      "float fAmbient;};\n"
      "uniform DirectionalLight sunLight;\n"
      "uniform float fRenderHeight;\n"
      "uniform float fMaxTextureU;\n"
      "uniform float fMaxTextureV;\n"

      "out vec4 outputColor;\n"
      "uniform vec4 vColor;\n"

      "vec4 GetDirectionalLightColor(DirectionalLight dirLight, vec3 vNormal){\n"
      "float fDiffuseIntensity = max(0.0, dot(vNormal, -dirLight.vDirection));\n"
      "float fMult = clamp(dirLight.fAmbient+fDiffuseIntensity, 0.0, 1.0);\n"
      "return vec4(dirLight.vColor*fMult, 1.0);}\n"

      "void main(){\n"
      "vec3 vNormalized = normalize(vNormal);\n"
      "vec4 vTexColor = vec4(0.0);\n"

      "float fScale = vWorldPos.y/fRenderHeight;\n"

      "const float fRange1 = 0.15f;\n"
      "const float fRange2 = 0.3f;\n"
      "const float fRange3 = 0.65f;\n"
      "const float fRange4 = 0.85f;\n"

      "if(fScale >= 0.0 && fScale <= fRange1)\n"
      "vTexColor = texture2D(gSampler[0], vTexCoord);\n"
      "else if(fScale <= fRange2){\n"
      "fScale -= fRange1;\n"
      "fScale /= (fRange2-fRange1);\n"

      "float fScale2 = fScale;\n"
      "fScale = 1.0-fScale;\n"

      "vTexColor += texture2D(gSampler[0], vTexCoord)*fScale;\n"
      "vTexColor += texture2D(gSampler[1], vTexCoord)*fScale2;}\n"

      "else if(fScale <= fRange3)\n"
      "vTexColor = texture2D(gSampler[1], vTexCoord);"
      "else if(fScale <= fRange4){\n"
      "fScale -= fRange3;\n"
      "fScale /= (fRange4-fRange3);\n"

      "float fScale2 = fScale;\n"
      "fScale = 1.0-fScale;\n"

      "vTexColor += texture2D(gSampler[1], vTexCoord)*fScale;\n"
      "vTexColor += texture2D(gSampler[2], vTexCoord)*fScale2;}\n"
      "else\n"
      "vTexColor = texture2D(gSampler[2], vTexCoord);\n"

      "vec2 vPathCoord = vec2(vTexCoord.x/fMaxTextureU, vTexCoord.y/fMaxTextureV);\n"
      "vec4 vPathIntensity = texture2D(gSampler[4], vPathCoord);\n"
      "fScale = vPathIntensity.x;\n"

      "vec4 vPathColor = texture2D(gSampler[3], vTexCoord); // Black color means there is a path\n"
      "vec4 vFinalTexColor = fScale*vTexColor+(1-fScale)*vPathColor;\n"

      "vec4 vMixedColor = vFinalTexColor*vColor;\n"
      "vec4 vDirLightColor = GetDirectionalLightColor(sunLight, vNormal);\n"


      "outputColor = vMixedColor*(vDirLightColor);}\n";


  vert = Shader::getStandardVertexShader(true);
  frag = Shader::getStandardFragmentShader(true);
  m_shader = ShaderHandler::getInstance()->createShader(vert, frag);
  if (!m_shader){
      HandleError::showError("Shader didn't load in HeightMap");
      exit(-1);
    }
  m_uniLoc_projection = glGetUniformLocation(m_shader->getProgramID(), "projection");
  m_uniLoc_modelView = glGetUniformLocation(m_shader->getProgramID(), "modelview");
  m_uniloc_pos = glGetUniformLocation(m_shader->getProgramID(), "_pos");


  std::string tex_name[] = {"fungus.jpg", "sand_grass_02.jpg", "rock_2_4w.jpg", "sand.jpg"};

  for (auto &tmp : tex_name)
    tmp = "textures/heightmap/" + tmp;
  for (int i = 0; i < 4; ++i) {
      m_tex[i] = new Texture(tex_name[i]);
      m_tex[i]->load();
    }
  if (m_img.empty()) {
      return;
    }
  SDL_Surface *img = HandleSDL::loadImage(m_img);

  if (!img) {
      HandleError::showError("Invalid image: " + m_img);
      exit(0);
    }
  m_width = img->w;
  m_height = img->h;

  std::vector<std::vector<glm::vec3> >  tmp_v(m_height, std::vector<glm::vec3>(m_width));

  for (unsigned int i = 0; i < m_width; ++i)
    for (unsigned int j = 0; j < m_height; ++j) {
        Color tmp = HandleSDL::getPixelColor(img, i, j);

        tmp_v[j][i] = glm::vec3(i, (tmp.blue() + tmp.green() + tmp.red()) * 10.f, j);
      }


  for (unsigned int y = 0; y < m_height - 1; ++y)
    for (unsigned int x = 0; x < m_width - 1; ++x) {
        createTriangle(m_vertices, tmp_v[y][x], tmp_v[y][x + 1], tmp_v[y + 1][x]);
        m_textures.push_back(0.f); m_textures.push_back(0.f);
        m_textures.push_back(1.f); m_textures.push_back(0.f);
        m_textures.push_back(0.f); m_textures.push_back(1.f);
        createTriangle(m_vertices, tmp_v[y + 1][x], tmp_v[y + 1][x + 1], tmp_v[y][x + 1]);
        m_textures.push_back(0.f); m_textures.push_back(1.f);
        m_textures.push_back(1.f); m_textures.push_back(1.f);
        m_textures.push_back(1.f); m_textures.push_back(0.f);
      }

  m_pointsNumber = m_vertices.size() / 3;

  this->initVertexBufferObject();
  this->initVertexArrayObject();
}

void  HeightMap::paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix)
{
  //std::string s[] = {"gSampler[0]", "gSampler[1]", "gSampler[2]", "gSampler[3]"};
  glUseProgram(m_shader->getProgramID());

  glBindVertexArray(m_vaoID);

  glUniformMatrix4fv(m_uniLoc_projection, 1, GL_FALSE, glm::value_ptr(proj_matrix));

  glUniformMatrix4fv(m_uniLoc_modelView, 1, GL_FALSE, glm::value_ptr(view_matrix));
  glUniform3fv(m_uniloc_pos, 1, glm::value_ptr(glm::vec3(m_pos.x(), m_pos.y(), m_pos.z())));

  /*for (int i = 0; i < 4; ++i) {
      m_tex[i]->bindTexture(i);
      m_shader->setUniform(s[i], i);
    }*/
  m_tex[0]->bind();

  glDrawArrays(GL_TRIANGLES, 0, m_pointsNumber);

  m_tex[0]->unbind();

  Texture::bind(0);
  glBindVertexArray(0);
  glUseProgram(0);
}
