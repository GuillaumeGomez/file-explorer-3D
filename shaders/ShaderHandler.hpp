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

  static Shader  *createShader(std::string vertexShader, std::string fragmentShader);
  static Shader  *createShader(const char *vertexShader, const char *fragmentShader);
  static void    destroyShader(Shader*);

private:
  ShaderHandler();
  Shader  *_createShader(std::string vertexShader, std::string fragmentShader);
  Shader  *_createShader(const char *vertexShader, const char *fragmentShader);
  void    _destroyShader(Shader*);

  std::vector<std::pair<unsigned int, Shader*> > m_list;
  static ShaderHandler    *m_instance;
};

#endif // SHADERHANDLER_HPP
