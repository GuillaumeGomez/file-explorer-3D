#include "Color.hpp"

Color::Color(float red, float green, float blue) : m_red(red), m_green(green), m_blue(blue)
{
}

float &Color::red()
{
  return m_red;
}

float &Color::green()
{
  return m_green;
}

float &Color::blue()
{
  return m_blue;
}

void  Color::setRed(float r)
{
  m_red = r;
}

void  Color::setGreen(float g)
{
  m_green = g;
}

void  Color::setBlue(float b)
{
  m_blue = b;
}

bool  Color::operator==(Color const &c) const
{
  return (int)(c.m_red * 255.f) == (int)(m_red * 255.f) &&
      (int)(c.m_blue * 255.f) == (int)(m_blue * 255.f) &&
      (int)(c.m_green * 255.f) == (int)(m_green * 255.f);
}
