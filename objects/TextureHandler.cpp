#include "TextureHandler.hpp"
#include <cstdlib>

TextureHandler  *TextureHandler::instance = 0;

void  clearTextureHandler()
{
  TextureHandler  *tmp = TextureHandler::getInstance();

  if (tmp)
    delete tmp;
}

TextureHandler *TextureHandler::getInstance()
{
  if (!instance) {
      instance = new TextureHandler;
      atexit(clearTextureHandler);
    }
  return instance;
}

TextureHandler::TextureHandler()
{
}

TextureHandler::~TextureHandler()
{
  for (auto it = textures.begin(); it != textures.end(); ++it){
      glDeleteTextures(1, &it->second.first);
    }
}

GLuint        TextureHandler::add(const char *s, GLuint id)
{
  if (!s)
    return 0;
  std::string st = s;
  return this->add(st, id);
}

GLuint        TextureHandler::add(std::string const &s, GLuint id)
{
  auto  it = this->findElement(s);

  if (it != textures.end()){
      it->second.second += 1;
      return it->second.first;
    }
  auto tmp = std::pair<GLuint, unsigned int>(id, 1);
  textures.push_back(std::pair<std::string, std::pair<GLuint, unsigned int> >(s, tmp));
  return id;
}

GLuint        TextureHandler::add(GLuint id)
{
  auto  it = this->findElement(id);

  if (it != textures.end()){
      it->second.second += 1;
      return it->second.first;
    }
  auto tmp = std::pair<GLuint, unsigned int>(id, 1);
  textures.push_back(std::pair<std::string, std::pair<GLuint, unsigned int> >(std::string(""), tmp));
  return id;
}

GLuint        TextureHandler::get(const char *s)
{
  if (!s)
    return 0;
  std::string st = s;
  return this->get(st);
}

GLuint        TextureHandler::get(std::string const &s)
{
  tex_map::iterator  it = this->findElement(s);

  if (it == textures.end())
    return 0;
  return it->second.first;
}

GLuint        TextureHandler::operator[](const char *s)
{
  return this->get(s);
}

GLuint        TextureHandler::operator[](std::string const &s)
{
  return this->get(s);
}

std::string const TextureHandler::get(GLuint id)
{
  for (tex_map::iterator it = textures.begin(); it != textures.end(); ++it){
      if (it->second.first == id){
          return it->first;
        }
    }
  return std::string("");
}

std::string const TextureHandler::operator[](GLuint id)
{
  return this->get(id);
}

unsigned int  TextureHandler::size() const
{
  return textures.size();
}

bool  TextureHandler::destroy(const char *s)
{
  if (!s)
    return false;
  std::string st = s;
  return this->destroy(st);
}

unsigned int TextureHandler::numberOfThisTexture(GLuint id)
{
  auto it = this->findElement(id);

  if (it == textures.end())
    return 0;
  return it->second.second;
}

tex_map::iterator TextureHandler::findElement(GLuint id)
{
  if (!id)
    return textures.end();
  for (auto it = textures.begin(); it != textures.end(); ++it) {
      if (it->second.first == id)
        return it;
    }
  return textures.end();
}

tex_map::iterator TextureHandler::findElement(std::string st)
{
  if (st.empty())
    return textures.end();
  for (auto it = textures.begin(); it != textures.end(); ++it) {
      if (it->first == st)
        return it;
    }
  return textures.end();
}

bool  TextureHandler::destroy(const std::string &s)
{
  auto  it = this->findElement(s);

  if (it == textures.end())
    return false;
  it->second.second -= 1;
  if (it->second.second > 0)
    return true;
  glDeleteTextures(1, &it->second.first);
  textures.erase(it);
  return true;
}

bool  TextureHandler::destroy(GLuint id)
{
  auto  it = this->findElement(id);

  if (it == textures.end())
    return false;
  it->second.second -= 1;
  if (it->second.second > 0)
    return true;
  glDeleteTextures(1, &it->second.first);
  textures.erase(it);
  return true;
}
