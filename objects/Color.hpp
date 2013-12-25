#ifndef COLOR_HPP
#define COLOR_HPP

class Color
{
public:
  Color(float red = 1.f, float green = 1.f, float blue = 1.f);
  virtual ~Color(){}
  float &red();
  float &green();
  float &blue();
  void  setRed(float);
  void  setGreen(float);
  void  setBlue(float);
  bool  operator==(Color const&) const;

protected:
  float m_red;
  float m_green;
  float m_blue;
};

#define RED     Color(1.f, 0.f, 0.f)
#define GREEN   Color(0.f, 1.f, 0.f)
#define BLUE    Color(0.f, 0.f, 1.f)
#define YELLOW  Color(1.f, 1.f, 0.f)
#define ORANGE  Color(0.98f, 0.4f, 0.05f)
#define PURPLE  Color(0.6f, 0.01f, 0.23f)
#define BLACK   Color(0.f, 0.f, 0.f)
#define WHITE   Color()

#endif // COLOR_HPP
