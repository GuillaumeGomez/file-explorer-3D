#ifndef HANDLEFPSCOUNT_HPP
#define HANDLEFPSCOUNT_HPP

#include <ctime>

class HandleFpsCount
{
public:
  HandleFpsCount();
  virtual ~HandleFpsCount();
  float   getFpsCount();

private:
  int     frames;
  float   fps;
  clock_t overtime;
  clock_t nextUpdate;
};

#endif // HANDLEFPSCOUNT_HPP
