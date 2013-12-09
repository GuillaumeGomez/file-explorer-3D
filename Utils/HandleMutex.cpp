#include "HandleMutex.hpp"
#include "MyException.hpp"

AHandleMutex::AHandleMutex()
{
}

#ifdef WIN32
HandleMutex::HandleMutex() : AHandleMutex(), m_mutex(CreateMutex(0, FALSE, 0))
{
  if (!m_mutex)
    throw MyException("CreateMutex failed...");
}

HandleMutex::~HandleMutex()
{
  if (m_mutex)
    CloseHandle(m_mutex);
}

void    HandleMutex::lock()
{
  WaitForSingleObject(m_mutex, INFINITE);
}

void    HandleMutex::unlock()
{
  ReleaseMutex(m_mutex);
}

bool    HandleMutex::trylock()
{
  return WaitForSingleObject(m_mutex, 0) != WAIT_TIMEOUT;
}

#else

HandleMutex::HandleMutex() : AHandleMutex()
{
  if (pthread_mutex_init(&m_mutex, 0))
    throw MyException("CreateMutex failed...");
}

HandleMutex::~HandleMutex()
{
  pthread_mutex_destroy(&m_mutex);
}

void    HandleMutex::lock()
{
  pthread_mutex_lock(&m_mutex);
}

void    HandleMutex::unlock()
{
  pthread_mutex_unlock(&m_mutex);
}

bool    HandleMutex::trylock()
{
  return !pthread_mutex_trylock(&m_mutex);
}

#endif
