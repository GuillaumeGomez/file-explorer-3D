#ifndef OBJECTFACTORY_HPP
#define OBJECTFACTORY_HPP

#include <tuple>
#include "myGLWidget.hpp"

template<typename T, class... Args>
class SubObjectFactory;

template<std::size_t... X>
struct index_tuple{};

template<std::size_t I, typename IndexTuple, typename... Types>
struct make_indices_impl;

template<std::size_t I, std::size_t... Indices, typename T, typename... Types>
struct make_indices_impl<I, index_tuple<Indices...>, T, Types...>
{
  typedef typename make_indices_impl<I + 1, index_tuple<Indices..., I>, Types...>::type type;
};

template<std::size_t I, std::size_t... Indices>
struct make_indices_impl<I, index_tuple<Indices...> >
{
  typedef index_tuple<Indices...> type;
};

template<typename... Types>
struct make_indices : make_indices_impl<0, index_tuple<>, Types...>
{};

template <class T, class... Args>
T *creator(Args&&... a)
{
  return new T(a...);
}

class ObjectFactory
{
public:
  virtual myGLWidget  *createObject() = 0;
  virtual ~ObjectFactory(){}

  template<typename T, class... Args>
  static ObjectFactory  *createNewObject(Args... a)
  {
    ObjectFactory *tmp = new SubObjectFactory<T, Args...>(a...);

    tmp->m_is2D = false;
    tmp->m_isPauseObject = false;
    return tmp;
  }
  template<typename T, class... Args>
  static ObjectFactory  *createNew2DObject(Args... a)
  {
    ObjectFactory *tmp = new SubObjectFactory<T, Args...>(a...);

    tmp->m_is2D = true;
    tmp->m_isPauseObject = false;
    return tmp;
  }
  template<typename T, class... Args>
  static ObjectFactory  *createNewPauseObject(Args... a)
  {
    ObjectFactory *tmp = new SubObjectFactory<T, Args...>(a...);

    tmp->m_is2D = true;
    tmp->m_isPauseObject = true;
    return tmp;
  }

  bool  m_isPauseObject;
  bool  m_is2D;
};

template<typename T, class... Args>
class SubObjectFactory : public ObjectFactory
{
public:
  SubObjectFactory(Args... a) :
    args(a...)
  {
  }
  virtual ~SubObjectFactory(){}

  myGLWidget  *createObject()
  {
    typedef typename make_indices<Args...>::type Indices;
    return this->creation(Indices(), args);
  }

private:
  template <std::size_t... Indices>
  myGLWidget *creation(index_tuple<Indices...>, std::tuple<Args...>&ar)
  {
    myGLWidget *tmp = creator<T>(std::forward<Args>(std::get<Indices>(ar))...);

    if (tmp)
      tmp->setRender2D(m_is2D);
    return tmp;
  }
  std::tuple<Args...>  args;
};

#endif // OBJECTFACTORY_HPP
