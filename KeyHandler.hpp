#ifndef KEYHANDLER_HPP
#define KEYHANDLER_HPP

#define MAX_KEY_SIZE  4

class HandleMutex;

class KeyHandler
{
public:
  KeyHandler(int ms = 10);
  virtual ~KeyHandler();
  int     getNbKeys() const;
  void    addKey(int);
  void    operator<<(int);
  void    releaseKey(int);
  int     getKey(int);
  int     operator[](int);
  void    setInterval(int ms);
  int     getInterval();
  int     *getKeys();
  void    lock();
  void    unlock();

private:
  int         m_nbKeys;
  HandleMutex *m_mut;
  int         m_interval;
  int         m_key[MAX_KEY_SIZE];
};

#endif // KEYHANDLER_HPP
