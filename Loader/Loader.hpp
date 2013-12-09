#ifndef LOADER_HPP
#define LOADER_HPP

#include <list>

class ObjectFactory;
class MyWindow;
class LoadingMenu;

class Loader
{
public:
  explicit Loader(MyWindow*);
  virtual ~Loader();
  void     loadDatas();

private:
  MyWindow                  *m_win;
  std::list<ObjectFactory*> objList;
  LoadingMenu               *m_load;
};

#endif // LOADER_HPP
