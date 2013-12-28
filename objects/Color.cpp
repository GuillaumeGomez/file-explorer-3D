#include "Color.hpp"

Color::Color(float red, float green, float blue)
  : m_red(red), m_green(green), m_blue(blue),
    m_ired(red * 255.f), m_igreen(green * 255.f), m_iblue(blue * 255.f)
{
}

float const &Color::red() const
{
  return m_red;
}

float const &Color::green() const
{
  return m_green;
}

float const &Color::blue() const
{
  return m_blue;
}

void  Color::setRed(float r)
{
  m_red = r;
  m_ired = r * 255.f;
}

void  Color::setGreen(float g)
{
  m_green = g;
  m_igreen = g * 255.f;
}

void  Color::setBlue(float b)
{
  m_blue = b;
  m_iblue = b * 255.f;
}

bool  Color::operator==(Color const &c) const
{
  return (c.m_ired == m_ired &&
          c.m_iblue == m_iblue &&
          c.m_igreen == m_igreen);
}
