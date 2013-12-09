#include "MyMutexLocker.hpp"
#include "HandleMutex.hpp"

MyMutexLocker::MyMutexLocker(HandleMutex *m) : m_mutex(m)
{
  m_mutex->lock();
}

MyMutexLocker::~MyMutexLocker()
{
  m_mutex->unlock();
}
