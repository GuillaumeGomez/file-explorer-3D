#ifndef HANDLEMUTEX_HPP
#define HANDLEMUTEX_HPP

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

class AHandleMutex
{
public:
  AHandleMutex();
  virtual void    lock() = 0;
  virtual bool    trylock() = 0;
  virtual void    unlock() = 0;
};

#ifdef WIN32

class HandleMutex : public AHandleMutex
{
public:
  HandleMutex();
  virtual ~HandleMutex();
  void  lock();
  bool  trylock();
  void  unlock();

private:
  HANDLE  m_mutex;
};

#else
class HandleMutex : public AHandleMutex
{
public:
  HandleMutex();
  virtual ~HandleMutex();
  void  lock();
  bool  trylock();
  void  unlock();

private:
  pthread_mutex_t  m_mutex;
};

#endif

#endif // HANDLEMUTEX_HPP
