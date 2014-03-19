#include "Shader.hpp"
#include <iostream>
#include <fstream>

#include "../HandleError.hpp"

Shader::Shader() : m_vertexID(0), m_fragmentID(0), m_programID(0), m_vertexSource(), m_fragmentSource()
{
}

Shader::Shader(Shader const &toCopy)
  : m_vertexID(0), m_fragmentID(0), m_programID(0), m_vertexSource(), m_fragmentSource()
{
  m_vertexSource = toCopy.m_vertexSource;
  m_fragmentSource = toCopy.m_fragmentSource;

  load();
}

Shader::Shader(std::string vertexSource, std::string fragmentSource)
  : m_vertexID(0), m_fragmentID(0), m_programID(0), m_vertexSource(vertexSource), m_fragmentSource(fragmentSource)
{
}

Shader::~Shader()
{
  glDeleteShader(m_vertexID);
  glDeleteShader(m_fragmentID);
  glDeleteProgram(m_programID);
}

Shader& Shader::operator=(Shader const &toCopy)
{
  m_vertexSource = toCopy.m_vertexSource;
  m_fragmentSource = toCopy.m_fragmentSource;

  load();
  return *this;
}

bool  Shader::load()
{
  if (glIsShader(m_vertexID) == GL_TRUE)
    glDeleteShader(m_vertexID);
  if (glIsShader(m_fragmentID) == GL_TRUE)
    glDeleteShader(m_fragmentID);
  if (glIsProgram(m_programID) == GL_TRUE)
    glDeleteProgram(m_programID);

  // Compilation des shaders
  if (m_vertexSource.find('\n') != std::string::npos){
      if (!compileShader(m_vertexID, GL_VERTEX_SHADER, m_vertexSource))
        return false;
    }
  else{
      if (!compileShaderFromFile(m_vertexID, GL_VERTEX_SHADER, m_vertexSource))
        return false;
    }
  if (m_fragmentSource.find('\n') != std::string::npos){
      if (!compileShader(m_fragmentID, GL_FRAGMENT_SHADER, m_fragmentSource))
        return false;
    }
  else{
      if (!compileShaderFromFile(m_fragmentID, GL_FRAGMENT_SHADER, m_fragmentSource))
        return false;
    }

  // Creation du programme
  m_programID = glCreateProgram();

  // Association des shaders
  glAttachShader(m_programID, m_vertexID);
  glAttachShader(m_programID, m_fragmentID);

  // Verrouillage des entrees shader
  glBindAttribLocation(m_programID, 0, "in_Vertex");
  glBindAttribLocation(m_programID, 1, "in_Color");
  glBindAttribLocation(m_programID, 2, "in_TexCoord0");
  glBindAttribLocation(m_programID, 3, "in_Normal");

  // Linkage du programme
  glLinkProgram(m_programID);

  // Verification du linkage
  GLint erreurLink(0);
  glGetProgramiv(m_programID, GL_LINK_STATUS, &erreurLink);

  if (erreurLink != GL_TRUE)
    {
      std::string s = getError(m_programID);
      std::cout << s << std::endl;

      glDeleteProgram(m_programID);
      return false;
    }
  return true;
}

bool  Shader::compileShaderFromFile(GLuint &shader, GLenum type, const std::string &source)
{
  std::ifstream fd(source.c_str());
  int           length;
  char          *buffer;
  std::string   datas;

  if (!fd.is_open())
    return false;

  fd.seekg(0, fd.end);
  length = fd.tellg();
  fd.seekg(0, fd.beg);

  buffer = new char[length + 1];

  fd.read(buffer, length);
  fd.close();
  buffer[length] = 0;
  datas = buffer;
  delete[] buffer;
  return this->compileShader(shader, type, datas);
}

bool Shader::compileShader(GLuint &shader, GLenum type, std::string const &source)
{
  shader = glCreateShader(type);

  if (!shader) {
      HandleError::showError("Erreur, le type de shader n'existe pas");
      return false;
    }

  const GLchar* chaineCodeSource = source.c_str();
  // Envoi du code source au shader
  glShaderSource(shader, 1, &chaineCodeSource, 0);

  glCompileShader(shader);

  GLint erreurCompilation(0);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &erreurCompilation);

  if (erreurCompilation != GL_TRUE)
    {
      std::string err = getError(shader);
      if (type == GL_VERTEX_SHADER)
        HandleError::showError("vertex error: " + err);
      else
        HandleError::showError("fragment error: " + err);

      glDeleteShader(shader);
      return false;
    }
  return true;
}

std::string Shader::getError(GLuint t)
{
  std::string err;
  GLint errorSize(0);
  char *error;

  glGetShaderiv(t, GL_INFO_LOG_LENGTH, &errorSize);

  error = new char[errorSize + 1];
  glGetShaderInfoLog(t, errorSize, &errorSize, error);
  error[errorSize] = '\0';
  err = error;
  delete[] error;
  return err;
}

GLuint Shader::getProgramID() const
{
  return m_programID;
}

void  Shader::setVertexSource(std::string s)
{
  m_vertexSource = s;
}

void  Shader::setFragmentSource(std::string s)
{
  m_fragmentSource = s;
}

std::string const &Shader::getFragmentSource() const
{
  return m_fragmentSource;
}

std::string const &Shader::getVertexSource() const
{
  return m_vertexSource;
}

std::string Shader::getStandardVertexShader(bool hasTexture)
{
  std::string vert;

  if (hasTexture) {
      vert =
          "#version 330\n"

          "in vec3 in_Vertex;\n"
          "in vec2 in_TexCoord0;\n"
          "in vec3 in_Normal;\n"

          "uniform mat4 projection;\n"
          "uniform mat4 modelview;\n"
          "uniform vec4 _rot;\n"
          "uniform vec3 _pos;\n"

          "out vec2 coordTexture;\n"
          "out vec3 vNormal;\n"

          "mat4 my_translate(mat4 ori, vec3 t_pos){\n"
          "mat4 tmp = ori;\n"
          "tmp[3] = ori[0] * t_pos.x + ori[1] * t_pos.y + ori[2] * t_pos.z + ori[3];\n"
          "return tmp;\n}\n"

          "mat4 my_rotate(vec4 rot, mat4 ori){\n"
          "float ra = radians(rot.w);\n"
          "float si = sin(ra);\n"
          "float co = cos(ra);\n"
          "vec3 axis = rot.xyz;\n"

          "axis = normalize(axis);\n"
          "vec3 temp = (1.0 - co) * axis;\n"
          "mat4 rota;\n"
          "rota[0][0] = co + temp[0] * axis[0];\n"
          "rota[0][1] = temp[0] * axis[1] + si * axis[2];\n"
          "rota[0][2] = temp[0] * axis[2] - si * axis[1];\n"
          "rota[1][0] = temp[1] * axis[0] - si * axis[2];\n"
          "rota[1][1] = co + temp[1] * axis[1];\n"
          "rota[1][2] = temp[1] * axis[2] + si * axis[0];\n"
          "rota[2][0] = temp[2] * axis[0] + si * axis[1];\n"
          "rota[2][1] = temp[2] * axis[1] - si * axis[0];\n"
          "rota[2][2] = co + temp[2] * axis[2];\n"

          "mat4 Result;\n"
          "Result[0] = ori[0] * rota[0][0] + ori[1] * rota[0][1] + ori[2] * rota[0][2];\n"
          "Result[1] = ori[0] * rota[1][0] + ori[1] * rota[1][1] + ori[2] * rota[1][2];\n"
          "Result[2] = ori[0] * rota[2][0] + ori[1] * rota[2][1] + ori[2] * rota[2][2];\n"
          "Result[3] = ori[3];\n"
          "return Result;\n"
          "}\n"

          "mat4 my_transform(mat4 model, vec3 po, vec4 an){\n"
          "mat4 tmp = my_translate(model, po);\n"
          "if (an.w != 0.0 && (an.x != 0.0 || an.y != 0.0 || an.z != 0.0)){\n"
          "return my_rotate(an, tmp);\n} else {\n"
          "return tmp;\n}\n"
          "}\n"

          "void main(){\n"
          "gl_Position = projection * my_transform(modelview, _pos, _rot) * vec4(in_Vertex, 1.0);\n"
          "coordTexture = in_TexCoord0;\n"
          "vNormal = in_Normal;\n"
          "}";
    } else {
      vert =
          "#version 330\n"

          "in vec3 in_Vertex;\n"
          "in vec3 in_Color;\n"
          "in vec3 in_Normal;\n"

          "uniform mat4 projection;\n"
          "uniform mat4 modelview;\n"
          "uniform vec4 _rot;\n"
          "uniform vec3 _pos;\n"

          "out vec3 color;\n"
          "out vec3 vNormal;\n"

          "mat4 my_translate(mat4 ori, vec3 t_pos){\n"
          "mat4 tmp = ori;\n"
          "tmp[3] = ori[0] * t_pos.x + ori[1] * t_pos.y + ori[2] * t_pos.z + ori[3];\n"
          "return tmp;\n}\n"

          "mat4 my_rotate(vec4 rot, mat4 ori){\n"
          "float ra = radians(rot.w);\n"
          "float si = sin(ra);\n"
          "float co = cos(ra);\n"
          "vec3 axis = rot.xyz;\n"

          "axis = normalize(axis);\n"
          "vec3 temp = (1.0 - co) * axis;\n"
          "mat4 rota;\n"
          "rota[0][0] = co + temp[0] * axis[0];\n"
          "rota[0][1] = temp[0] * axis[1] + si * axis[2];\n"
          "rota[0][2] = temp[0] * axis[2] - si * axis[1];\n"
          "rota[1][0] = temp[1] * axis[0] - si * axis[2];\n"
          "rota[1][1] = co + temp[1] * axis[1];\n"
          "rota[1][2] = temp[1] * axis[2] + si * axis[0];\n"
          "rota[2][0] = temp[2] * axis[0] + si * axis[1];\n"
          "rota[2][1] = temp[2] * axis[1] - si * axis[0];\n"
          "rota[2][2] = co + temp[2] * axis[2];\n"

          "mat4 Result;\n"
          "Result[0] = ori[0] * rota[0][0] + ori[1] * rota[0][1] + ori[2] * rota[0][2];\n"
          "Result[1] = ori[0] * rota[1][0] + ori[1] * rota[1][1] + ori[2] * rota[1][2];\n"
          "Result[2] = ori[0] * rota[2][0] + ori[1] * rota[2][1] + ori[2] * rota[2][2];\n"
          "Result[3] = ori[3];\n"
          "return Result;\n"
          "}\n"

          "mat4 my_transform(mat4 model, vec3 po, vec4 an){\n"
          "mat4 tmp = my_translate(model, po);\n"
          "if (an.w != 0.0 && (an.x != 0.0 || an.y != 0.0 || an.z != 0.0)){\n"
          "return my_rotate(an, tmp);\n} else {\n"
          "return tmp;\n}\n"
          "}\n"

          "void main(){\n"
          "gl_Position = projection * my_transform(modelview, _pos, _rot) * vec4(in_Vertex, 1.0);\n"
          "color = in_Color;\n"
          "vNormal = in_Normal;\n"
          "}";
    }
  return vert;
}

std::string Shader::getStandardFragmentShader(bool hasTexture)
{
  std::string frag;

  if (hasTexture) {
      frag =
          "#version 330\n"

          "in vec2 coordTexture;\n"
          "in vec3 vNormal;\n"

          "uniform sampler2D tex;\n"

          "out vec4 out_Color;\n"

          "void main(){\n"
          "vec3 normalized = normalize(vNormal);"
          "out_Color = texture(tex, coordTexture);\n"
          "}";
    } else {
      frag =
          "#version 330\n"

          "in vec3 color;\n"
          "in vec3 vNormal;\n"

          "out vec4 out_Color;\n"

          "void main(){\n"
          "vec3 normalized = normalize(vNormal);"
          "out_Color = vec4(color, 1.0);\n"
          "}";
    }
  return frag;
}

std::string Shader::getStandard2DVertexShader(bool hasTexture)
{
  std::string vert;

  if (hasTexture) {
      vert =
          "#version 330\n"

          "in vec2 in_Vertex;\n"
          "in vec2 in_TexCoord0;\n"

          "out vec2 coordTexture;\n"
          "void main()\n"
          "{\n"
          "gl_Position.xy = in_Vertex;\n"
          "coordTexture = in_TexCoord0;\n"
          "}";
    } else {
      vert =
          "#version 330\n"

          "in vec2 in_Vertex;\n"
          "in vec3 in_Color;\n"

          "out vec3 color;\n"
          "void main()\n"
          "{\n"
          "gl_Position.xy = in_Vertex;\n"
          "color = in_Color;\n"
          "}";
    }
  return vert;
}

std::string Shader::getStandard2DFragmentShader(bool hasTexture)
{
  std::string frag;

  if (hasTexture) {
      frag =
          "#version 330\n"

          "in vec2 coordTexture;\n"
          "uniform sampler2D tex;\n"

          "out vec4 out_Color;\n"
          "void main()\n"
          "{\n"
          "out_Color = texture(tex, coordTexture);\n"
          "}";
    } else {
      frag =
          "#version 330\n"

          "in vec3 color;\n"

          "out vec4 out_Color;\n"
          "void main()\n"
          "{\n"
          "out_Color = vec4(color, 1.0);\n"
          "}";
    }
  return frag;
}

GLint Shader::getUniform(const std::string &s)
{
  return glGetUniformLocation(this->m_programID, s.c_str());
}

void        Shader::setUniform(std::string const &s, glm::vec2 const &v)
{
  int iLoc = glGetUniformLocation(m_programID, s.c_str());
  glUniform2fv(iLoc, 1, glm::value_ptr(v));
}

void        Shader::setUniform(GLint loc, glm::vec2 const &v)
{
  glUniform2fv(loc, 1, glm::value_ptr(v));
}

void        Shader::setUniform(std::string const &s, glm::vec3 const &v)
{
  int iLoc = glGetUniformLocation(m_programID, s.c_str());
  glUniform3fv(iLoc, 1, glm::value_ptr(v));
}

void        Shader::setUniform(GLint loc, glm::vec3 const &v)
{
  glUniform3fv(loc, 1, glm::value_ptr(v));
}

void        Shader::setUniform(std::string const &s, glm::vec4 const &v)
{
  int iLoc = glGetUniformLocation(m_programID, s.c_str());
  glUniform4fv(iLoc, 1, glm::value_ptr(v));
}

void        Shader::setUniform(GLint loc, glm::vec4 const &v)
{
  glUniform4fv(loc, 1, glm::value_ptr(v));
}

void        Shader::setUniform(std::string const &s, glm::mat3 const &m)
{
  int iLoc = glGetUniformLocation(m_programID, s.c_str());
  glUniformMatrix3fv(iLoc, 1, GL_FALSE, glm::value_ptr(m));
}

void        Shader::setUniform(GLint loc, glm::mat3 const &m)
{
  glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(m));
}

void        Shader::setUniform(std::string const &s, glm::mat4 const &m)
{
  int iLoc = glGetUniformLocation(m_programID, s.c_str());
  glUniformMatrix4fv(iLoc, 1, GL_FALSE, glm::value_ptr(m));
}

void        Shader::setUniform(GLint loc, glm::mat4 const &m)
{
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
}

void        Shader::setUniform(std::string const &s, int i)
{
  int iLoc = glGetUniformLocation(m_programID, s.c_str());
  glUniform1i(iLoc, i);
}

void        Shader::setUniform(GLint loc, int i)
{
  glUniform1i(loc, i);
}

void        Shader::setUniform(std::string const &s, float const &f)
{
  int iLoc = glGetUniformLocation(m_programID, s.c_str());
  glUniform1fv(iLoc, 1, &f);
}

void        Shader::setUniform(GLint loc, float const &f)
{
  glUniform1fv(loc, 1, &f);
}

void  Shader::bind()
{
  glUseProgram(m_programID);
}

void  Shader::unbind()
{
  glUseProgram(0);
}
