#include "HandleThread.hpp"
#include "MyMutexLocker.hpp"
#include "HandleMutex.hpp"

AHandleThread::AHandleThread() : m_func(0), m_arg(0)
{
}

AHandleThread::AHandleThread(thread_func f, void *a) : m_func(f), m_arg(a)
{
}

#ifdef WIN32
HandleThread::HandleThread() : AHandleThread(), m_mutex(new HandleMutex)
{
}

HandleThread::HandleThread(thread_func f, void *a) : AHandleThread(f, a), m_mutex(new HandleMutex)
{
}

HandleThread::~HandleThread()
{
  stop();
  delete m_mutex;
}

DWORD WINAPI    HandleThread_func(LPVOID a)
{
  HandleThread  *mt = static_cast<HandleThread*>(a);

  mt->run();
  mt->stop();
  return 0;
}

bool  HandleThread::start()
{
  return ((m_thread = CreateThread(NULL, 0, &HandleThread_func, static_cast<void*>(this), 0, 0)) != NULL);
}

bool    HandleThread::start(thread_func f, void *arg)
{
  if (!arg || !f)
    return false;
  stop();
  m_arg = arg;
  m_func = f;
  return this->start();
}

bool    HandleThread::isRunning()
{
  if (!m_thread)
    return false;
  return this->m_mutex->trylock() == false;
}

/*void    HandleThread::stop(DWORD wait_time)
{
  if (!waitForFinish(wait_time))
    stop();
}*/

void    HandleThread::stop()
{
  if (!isRunning())
    return;
  TerminateThread(m_thread, 0);
  WaitForSingleObject(m_thread, INFINITE);
  CloseHandle(m_thread);
  m_thread = 0;
}

bool	HandleThread::wait(DWORD wait_time)
{
  if (!isRunning())
    return true;
  return WaitForSingleObject(m_thread, wait_time) != WAIT_TIMEOUT;
}

void    HandleThread::run()
{
  MyMutexLocker   lock(m_mutex);

  (void)lock;
  (*m_func)(m_arg);
}
#else

HandleThread::HandleThread() : AHandleThread(), m_thread(-1)
{
}

HandleThread::HandleThread(thread_func f, void *a) : AHandleThread(f, a), m_thread(-1)
{
}

HandleThread::~HandleThread()
{
}

bool  HandleThread::start()
{
  if (!m_func || !m_arg)
    return false;
  this->stop();
  return pthread_create(&m_thread, 0, m_func, m_arg) == 0;
}

bool  HandleThread::start(thread_func f, void *a)
{
  m_func = f;
  m_arg = a;
  return this->start();
}

void  HandleThread::stop()
{
    if (m_thread != -1)
        pthread_cancel(m_thread);
    m_thread = -1;
}

bool  HandleThread::isRunning()
{
  return true;
}

bool HandleThread::wait(unsigned long ms)
{
  (void)ms;
  return !pthread_join(m_thread, 0);
}

#endif
