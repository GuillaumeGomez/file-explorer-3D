#include "KeyHandler.hpp"
#include "../Utils/HandleMutex.hpp"

KeyHandler::KeyHandler(int ms) : m_nbKeys(0), m_mut(new HandleMutex), m_interval(ms)
{
}

KeyHandler::~KeyHandler()
{
  delete m_mut;
}

int   KeyHandler::getNbKeys() const
{
  return m_nbKeys;
}

void  KeyHandler::addKey(int k)
{
  int i;

  if (m_nbKeys >= MAX_KEY_SIZE)
    return;
  for (i = 0; i < m_nbKeys; ++i){
      if (m_key[i] == k)
        return;
    }
  m_key[m_nbKeys++] = k;
}

void  KeyHandler::operator<<(int k)
{
  this->addKey(k);
}

void  KeyHandler::releaseKey(int k)
{
  int i;

  if (m_nbKeys <= 0)
    return;
  for (i = 0; i < m_nbKeys; ++i){
      if (m_key[i] == k){
          for (; i + 1 < m_nbKeys; ++i){
              m_key[i] = m_key[i + 1];
            }
          --m_nbKeys;
          return;
        }
    }
}

int KeyHandler::getKey(int i)
{
  /*if (i < 0 || i >= m_nbKeys)
    return -1;*/
  return m_key[i];
}

int KeyHandler::operator[](int i)
{
  return getKey(i);
}

void  KeyHandler::setInterval(int ms)
{
  m_interval = ms;
  if (m_interval <= 0)
    m_interval = 10;
}

int KeyHandler::getInterval()
{
  return m_interval;
}

int *KeyHandler::getKeys()
{
  return m_key;
}

void  KeyHandler::lock()
{
  m_mut->lock();
}

void  KeyHandler::unlock()
{
  m_mut->unlock();
}

/*void  KeyHandler::emitTimeout()
{
  emit repeatKey();
}*/
