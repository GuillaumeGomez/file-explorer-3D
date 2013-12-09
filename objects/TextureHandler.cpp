#include "TextureHandler.hpp"

TextureHandler::TextureHandler()
{
}

TextureHandler::~TextureHandler()
{
  for (tex_map::iterator it = textures.begin(); it != textures.end(); ++it){
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
  tex_map::iterator  it = textures.find(s);

  if (it != textures.end()){
      it->second.second += 1;
      return it->second.first;
    }
  textures[s] = std::pair<GLuint, int>(id, 1);
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
  tex_map::iterator  it = textures.find(s);

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

std::pair<GLuint, unsigned int>  TextureHandler::getPair(const std::string &s)
{
  for (tex_map::iterator it = textures.begin(); it != textures.end(); ++it){
      if (it->first == s){
          return it->second;
        }
    }
  return std::pair<GLuint, unsigned int>(0, 0);
}

std::pair<GLuint, unsigned int>  TextureHandler::getPair(GLuint id)
{
  for (tex_map::iterator it = textures.begin(); it != textures.end(); ++it){
      if (it->second.first == id){
          return it->second;
        }
    }
  return std::pair<GLuint, unsigned int>(0, 0);
}

bool  TextureHandler::destroy(const char *s)
{
  if (!s)
    return false;
  std::string st = s;
  return this->destroy(st);
}

bool  TextureHandler::destroy(const std::string &s)
{
  std::pair<GLuint, int>  id = this->getPair(s);

  if (id.first < 1)
    return false;
  id.second -= 1;
  if (id.second > 0)
    return true;
  glDeleteTextures(1, &id.first);
  textures.erase(textures.find(s));
  return true;
}

bool  TextureHandler::destroy(GLuint id)
{
  return this->destroy(this->get(id));
}
