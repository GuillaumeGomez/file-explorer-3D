#include "ShaderHandler.hpp"
#include "Shader.hpp"
#include <cstdlib>

ShaderHandler *ShaderHandler::m_instance = 0;

void  clearShaders()
{
  ShaderHandler *i = ShaderHandler::getInstance();

  if (i)
    delete i;
}

ShaderHandler *ShaderHandler::getInstance()
{
  if (!m_instance) {
      static bool t(true);

      m_instance = new ShaderHandler;
      if (t)
        atexit(clearShaders);
      t = false;
    }
  return m_instance;
}

ShaderHandler::ShaderHandler()
{
}

ShaderHandler::~ShaderHandler()
{
  for (auto tmp : m_list) {
      if (tmp.second)
        delete tmp.second;
    }
  m_list.clear();
  m_instance = 0;
}

Shader  *ShaderHandler::createShader(const char *vertexShader, const char *fragmentShader)
{
  if (!vertexShader || !fragmentShader)
    return 0;
  return this->createShader(std::string(vertexShader), std::string(fragmentShader));
}


Shader  *ShaderHandler::createShader(std::string vertexShader, std::string fragmentShader)
{
  for (auto &tmp : m_list) {
      if (tmp.second) {
          Shader  *s = tmp.second;
          if (s->getVertexSource() == vertexShader && s->getFragmentSource() == fragmentShader) {
              tmp.first += 1;
              return s;
            }
        }
    }
  Shader *s = new Shader(vertexShader, fragmentShader);

  if (!s->load()) {
      delete s;
      return 0;
    }
  m_list.push_back(std::pair<unsigned int, Shader*>(1, s));
  return s;
}

void  ShaderHandler::destroyShader(Shader *s)
{
  for (auto tmp = m_list.begin(); tmp != m_list.end(); ++tmp) {
      if (tmp->second != s)
        continue;
      tmp->first -= 1;
      if (!tmp->first) {
          delete tmp->second;
          m_list.erase(tmp);
        }
      return;
    }
  delete s;
}
