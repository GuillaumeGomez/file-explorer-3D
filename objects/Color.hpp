#ifndef COLOR_HPP
#define COLOR_HPP

class Color
{
public:
  Color(float red = 1.f, float green = 1.f, float blue = 1.f);
  virtual ~Color(){}
  float const &red() const;
  float const &green() const;
  float const &blue() const;
  int         ired() const;
  int         igreen() const;
  int         iblue() const;
  void  setRed(float);
  void  setRed(int);
  void  setGreen(float);
  void  setGreen(int);
  void  setBlue(float);
  void  setBlue(int);
  bool  operator==(Color const&) const;

protected:
  float m_red;
  float m_green;
  float m_blue;
  int   m_ired;
  int   m_igreen;
  int   m_iblue;
};

#define RED     Color(1.f, 0.f, 0.f)
#define GREEN   Color(0.f, 1.f, 0.f)
#define BLUE    Color(0.f, 0.f, 1.f)
#define YELLOW  Color(1.f, 1.f, 0.f)
#define ORANGE  Color(0.98f, 0.4f, 0.05f)
#define PURPLE  Color(0.6f, 0.01f, 0.23f)
#define BLACK   Color(0.f, 0.f, 0.f)
#define WHITE   Color()
#define GREY    Color(0.61f, 0.58f, 0.56f)
#define LGREY    Color(0.75f, 0.72f, 0.7f)

#endif // COLOR_HPP
