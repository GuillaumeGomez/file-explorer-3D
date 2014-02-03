#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include "Cube.hpp"

namespace Object
{
  class SkyBox : public Cube
  {
  public:
    SkyBox();
    virtual ~SkyBox();
    void  initializeGL();
    void  initializeGLNoList();
    void  paintGL(float&x, float&y);
    virtual std::string getClassName() const;

  private:
    GLuint      textures[6];
    GLenum      cube_map_target[6];
    GLuint      cube_map_texture_ID;
    GLint       PVM, vertex;
  };
}

#endif // SKYBOX_HPP
