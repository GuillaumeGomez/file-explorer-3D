#ifndef KEYHANDLER_HPP
#define KEYHANDLER_HPP

#define MAX_KEY_SIZE  6

class HandleMutex;

class KeyHandler
{
public:
  KeyHandler(int ms = 10);
  virtual ~KeyHandler();
  int     getNbKeys() const;
  bool    addKey(int);
  void    operator<<(int);
  void    releaseKey(int);
  int     getKey(int);
  int     operator[](int);
  void    setInterval(int ms);
  int     getInterval();
  int     *getKeys();
  bool    update(const float &time); // returns true if the remaining time has gone below zero
  void    setRemaining(float t);

private:
  int         m_nbKeys;
  int         m_interval;
  float       m_remaining;
  int         m_key[MAX_KEY_SIZE];
};

#endif // KEYHANDLER_HPP
