#include "KeyHandler.hpp"
#include "../Utils/HandleMutex.hpp"

KeyHandler::KeyHandler(int ms) : m_nbKeys(0), m_interval(ms), m_remaining(0.f)
{
}

KeyHandler::~KeyHandler()
{
}

int   KeyHandler::getNbKeys() const
{
  return m_nbKeys;
}

bool  KeyHandler::addKey(int k)
{
  int i;

  if (m_nbKeys >= MAX_KEY_SIZE)
    return false;
  for (i = 0; i < m_nbKeys; ++i){
      if (m_key[i] == k)
        return false;
    }
  m_key[m_nbKeys++] = k;
  return true;
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
  m_remaining = (m_interval / 1000.f);
}

int KeyHandler::getInterval()
{
  return m_interval;
}

int *KeyHandler::getKeys()
{
  return m_key;
}

bool  KeyHandler::update(const float &time)
{
  bool  b;

  m_remaining -= time;
  b = (m_remaining <= 0.f);
  if (m_remaining <= 0.f)
    m_remaining += (m_interval / 1000.f);
  return b;
}

void  KeyHandler::setRemaining(float t)
{
  m_remaining = t;
  if (m_remaining < 0.f)
    m_remaining = 0.f;
}
