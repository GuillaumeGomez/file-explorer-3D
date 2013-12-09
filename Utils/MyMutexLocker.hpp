#ifndef MYMUTEXLOCKER_HPP
#define MYMUTEXLOCKER_HPP

class HandleMutex;

class MyMutexLocker
{
public:
  MyMutexLocker(HandleMutex*);
  ~MyMutexLocker();

private:
  HandleMutex   *m_mutex;
};

#endif // MYMUTEXLOCKER_HPP
