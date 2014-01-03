#ifndef HANDLETHREAD_HPP
#define HANDLETHREAD_HPP

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

typedef void  *(*thread_func)(void*);

class HandleMutex;

class AHandleThread
{
public:
  AHandleThread();
  AHandleThread(thread_func, void*);
  virtual ~AHandleThread(){}
  virtual bool  start() = 0;
  virtual bool  start(thread_func, void *arg) = 0;
  virtual void  stop() = 0;
  virtual bool  isRunning() = 0;
  virtual bool  wait(unsigned long ms) = 0;

protected:
  thread_func m_func;
  void        *m_arg;
};

#ifdef WIN32

class HandleThread : public AHandleThread
{
public:
    HandleThread();
    HandleThread(thread_func, void*);
    virtual ~HandleThread();
    bool          start();
    bool          start(thread_func f, void *arg);
    void          stop();
    bool          isRunning();
    bool          wait(DWORD wait_time = INFINITE);
    //HandleMutex   *getMutex();

    /*never call it from outside*/
    void          run();

private:
    HANDLE        m_thread;
    HandleMutex   *m_mutex;
};

#else

class HandleThread : public AHandleThread
{
public:
  HandleThread();
  HandleThread(thread_func, void*);
  virtual ~HandleThread();
  bool  start();
  bool  start(thread_func, void*);
  void  stop();
  bool  isRunning();
  bool  wait(unsigned long ms = 0);

  /*never call it from outside*/
  void          run();

private:
  pthread_t     m_thread;
  HandleMutex   *m_mutex;
};
#endif

#endif // HANDLETHREAD_HPP
