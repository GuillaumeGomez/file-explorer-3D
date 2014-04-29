#ifndef KEYHANDLER_HPP
#define KEYHANDLER_HPP

#define MAX_KEY_SIZE  6

class HandleMutex;

class KeyHandler
{
public:
  KeyHandler(float seconds = 0.01f);
  virtual ~KeyHandler();
  int     getNbKeys() const;
  bool    addKey(int);
  void    operator<<(int);
  void    releaseKey(int);
  int     getKey(int);
  int     operator[](int);
  void    setInterval(float seconds);
  const float &getInterval() const;
  int     *getKeys();
  bool    update(const float &time); // returns true if the remaining time <= 0
  void    setRemaining(float t);
  float const &getRemaining() const;

private:
  int         m_nbKeys;
  float       m_interval;
  float       m_remaining;
  int         m_key[MAX_KEY_SIZE];
};

#endif // KEYHANDLER_HPP
