#ifndef ROOM_HPP
#define ROOM_HPP

#include "../myGLWidget.hpp"

class Room : public myGLWidget
{
public:
  Room(Vector3D p, Rotation r, const char *path);
  virtual ~Room();
  virtual void  initializeGL();
  virtual void  initializeGLNoList();
  virtual void  paintGL(const glm::mat4& view_matrix, const glm::mat4& proj_matrix);
  virtual void  pick(int &x, const glm::mat4& view_matrix, const glm::mat4& proj_matrix);
  unsigned int  nbObjects() const;
  virtual bool  hasBeenPicked(int);
  virtual int   objectSize();
  void          setMainWindow(MyWindow*);

private:
  std::vector<myGLWidget*>  dirs;
  std::vector<myGLWidget*>  files;
  std::vector<myGLWidget*>  objects;
  std::vector<float>        save_pos;
  Vector3D                  old_pos;
};

#endif // ROOM_HPP
