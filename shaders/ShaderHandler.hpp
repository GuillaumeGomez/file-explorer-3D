#ifndef SHADERHANDLER_HPP
#define SHADERHANDLER_HPP

#include <string>
#include <vector>
#include <utility>

class Shader;

class ShaderHandler
{
public:
  static ShaderHandler *getInstance();
  virtual ~ShaderHandler();
  Shader  *createShader(std::string vertexShader, std::string fragmentShader);
  Shader  *createShader(const char *vertexShader, const char *fragmentShader);
  void    destroyShader(Shader*);

private:
  ShaderHandler();

  std::vector<std::pair<unsigned int, Shader*> > m_list;
  static ShaderHandler    *m_instance;
};

#endif // SHADERHANDLER_HPP
