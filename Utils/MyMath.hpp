#ifndef MYMATH_HPP
#define MYMATH_HPP

#include <intrin.h>
#include "float.h"
#include "math.h"
#include <iostream>
#include "stdint.h"
#include <algorithm>

#ifndef INFINITY
#define INFINITY 3.402823466e+38F
#endif
#define Pi  3.14159265358979323846f
#define INV_Pi  1.0f/Pi
#define PI_D_CI 2*Pi/360.0f//pi divided by circle
#define CI_D_PI  360.0f/(Pi*2)// circle divided by pi
#define MATH_EPS 0.0001f
#define TWOPI  (Pi * 2.0f)
#define ONEOVERTWOPI  (1.0f / TWOPI)
#define PIOVER180 (Pi / 180.0f)
#define ONEEIGHTYOVERPI  (180.0f / Pi)
#define PIOVERTWO (Pi /2.0f)

inline float wrapPi(float theta) {
  theta += Pi;
  theta -= floor(theta * ONEOVERTWOPI) * TWOPI;
  theta -= Pi;
  return theta;
}
inline void sincosf(float& s, float& c, const float theta){
  s = sinf(theta);
  c = cosf(theta);
}
//A floating-point value; 32-bit floating-point formats use IEEE 754 single-precision (s23e8 format): sign bit, 8-bit biased (127) exponent, and 23-bit mantissa.
//16-bit floating-point formats use half-precision (s10e5 format): sign bit, 5-bit biased (15) exponent, and 10-bit mantissa.
inline uint16_t Uint16ToFloat16(uint16_t v){// this function is really only used for Direct x's  16 bit float format...
  float t(static_cast<float>(v));// convert it to a real float....
  uint16_t* p(reinterpret_cast<uint16_t*>(&t));
  uint16_t ret(0);// this is what will be returned
  ret|=*p>>13;// get the last two bytes of data, which is part of the mantisaa and shift off the extra data
  ret|=*(p+1)&0xC000;// get the sign of the number and the sign of the exponent
  ret|=(*(p+1)&0x07FF)<<3;// only get 4 bits of exponent, and the rest of the mantissa
  return ret;
}
inline float Float16ToFloat32(uint16_t v){
  float ret(0);
  uint16_t* r=reinterpret_cast<uint16_t*>(&ret);// this is what will be returned
  *(r+1)=(v&0x03ff);
  *reinterpret_cast<uint32_t*>(&ret)>>=3;//shift the mantisaa into place
  *(r+1)|=(v&0x3C00)>>3;
  *(r+1)|=v&0xC000;// get the sign
  return ret;
}
inline uint16_t Float32ToFloat16(float v){// this function is really only used for Direct x's  16 bit float format...
  uint16_t* p(reinterpret_cast<uint16_t*>(&v));
  uint16_t ret(0);// this is what will be returned
  ret|=*p>>13;// get the last two bytes of data, which is part of the mantisaa and shift off the extra data
  ret|=*(p+1)&0xC000;// get the sign of the number and the sign of the exponent
  ret|=(*(p+1)&0x07FF)<<3;// only get 4 bits of exponent, and the rest of the mantissa
  return ret;
}
// the Below code was taken from graphics.stanford.edu/~seander/bithacks.html
static const int MultiplyDeBruijnBitPosition[32] =
{
  0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
  8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31
};
// this works for 32 bit integers
inline unsigned int BSR(unsigned int v){
  v |= v >> 1; // first round down to one less than a power of 2
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  return MultiplyDeBruijnBitPosition[(unsigned int)(v * 0x07C4ACDDU) >> 27];
}

template<class T> inline void Swap( T &a, T &b ){
  const T temp(a);
  a = b;
  b= temp;
}

inline float CircleX(float x, float a, float b){// simple circle right meow, a and b are the height and width of the desired circle (NOT SQUARED) returns the y value, given a x
  return sqrtf ( (1.0f - (x*x)/(b*b)) *a*a);
}// I hope the compiler can optimize all of this away since the values will all be computable and known at runtime

// the following function will take the range [fromleft, fromright] and convert that range to [toleft, toright]
// x must be in the range [fromleft, fromright] and the returned value with be the value converted to the [toleft, toright]
struct cRange{
  float scale, invtrans, trans;
};
inline cRange CreateRange(float fromleft, float fromright, float toleft, float toright){
  float fromrange(abs(fromleft - fromright));
  float torange(abs(toleft - toright));
  cRange te;
  te.scale =torange/fromrange;
  te.invtrans = fromleft < fromright ? fromleft : fromright;// get the smallest number of the two for the invtrans
  te.trans = toleft < toright ? toleft : toright;// get the smallest number of the two for the trans
  return te;
}
inline float ChangeRange(float x, float fromleft, float fromright, float toleft, float toright){
  float fromrange(abs(fromleft - fromright));
  float torange(abs(toleft - toright));
  float scale(torange/fromrange);
  float invtrans(fromleft < fromright ? -fromleft : -fromright);
  float trans(toleft < toright ? toleft : toright);
  return (((x - invtrans) * scale) + trans);
}
inline float ChangeRange(float x, const cRange& range){
  return (((x - range.invtrans) * range.scale) + range.trans);
}
template<typename T>T Clamp(T Value, T Min, T Max){
  return (Value < Min) ? Min : (Value > Max) ? Max : Value;
}
#define DEGTORAD(deg) (deg*(Pi/180.0f))
#define RADTODEG(Rad) (Rad*(180.0f/Pi))
inline bool QuadraticFormula(const float a,const float b,const float c, float& r1, float& r2 ){
  const float q = b*b - 4*a*c;
  if( q >= 0 ){
      const float sq = sqrtf(q);
      const float d = 1 / (2*a);
      r1 = ( -b + sq ) * d;
      r2 = ( -b - sq ) * d;
      return true;//real roots
    }
  return false;//complex roots
}

#define POWOFTWO(a) (a && ! (a & (a - 1) ) )// returns true if the number is a power of two.. false otherwise
inline float lerp(const float u, const float v, const float x) { return u * (1 - x) + v * x; }
inline float cerp(const float u0, const float u1, const float u2, const float u3, const float x){
  float p = (u3 - u2) - (u0 - u1);
  float q = (u0 - u1) - p;
  float r = u2 - u0;
  return x * (x * (x * p + q) + r) + u1;
}

inline float herp(const float u0, const float u1, const float u2, const float u3, const float x, const float tension, const float bias){
  float m0 = (u1 - u0) * (1 + bias) * (1 - tension) / 2 + (u2 - u1) * (1 - bias) * (1 - tension) / 2;
  float m1 = (u2 - u1) * (1 + bias) * (1 - tension) / 2 + (u3 - u2) * (1 - bias) * (1 - tension) / 2;
  float x2 = x * x;
  float x3 = x * x2;
  float a0 =  2 * x3 - 3 * x2 + 1;
  float a1 =      x3 - 2 * x2 + x;
  float a2 =      x3 -     x2;
  float a3 = -2 * x3 + 3 * x2;

  return a0 * u1 + a1 * m0 + a2 * m1 + a3 * u2;
}


class vec2{
public:

  union {
    struct{
      float x, y;
    };

    float data[2];
  };

  vec2() {}// when initaling a vec2or, it is left in a scalartate of unknown!!!
  vec2(const vec2 &a) : x(a.x), y(a.y) {}
  vec2(float nx, float ny) : x(nx), y(ny) {}

  void operator += (const float scalar){ x += scalar; y += scalar;}
  void operator += (const vec2 &v){ x += v.x; y += v.y;}
  void operator -= (const float scalar){ x -= scalar; y -= scalar; }
  void operator -= (const vec2 &v){ x -= v.x; y -= v.y; }
  void operator *= (const float scalar){ x *= scalar; y *= scalar; }
  void operator *= (const vec2 &v){ x *= v.x; y *= v.y; }
  void operator /= (const float scalar){ float inv(1.0f/scalar);  x*=inv; y *=inv; }// NOTE: no check for divide by zero here
  void operator /= (const vec2 &v){ x /= v.x; y /= v.y; }

  void zero() { x=y=0.0f; }// zero's out the scalartruct

  float LengthSq() const{ return x*x + y*y;}
  float Length() const { return sqrtf(LengthSq()); }
  float& operator [](size_t index) { return data[index]; }
  float operator [](size_t index) const { return data[index]; }

};

inline vec2 operator + (const vec2 &u, const vec2 &v){ return vec2(u.x + v.x, u.y + v.y); }
inline vec2 operator + (const vec2 &v, const float scalar){ return vec2(v.x + scalar, v.y + scalar);}
inline vec2 operator + (const float scalar, const vec2 &v){ return vec2(v.x + scalar, v.y + scalar);}
inline vec2 operator - (const vec2 &u, const vec2 &v){ return vec2(u.x - v.x, u.y - v.y); }
inline vec2 operator - (const vec2 &v, const float scalar){ return vec2(v.x - scalar, v.y - scalar); }
inline vec2 operator - (const float scalar, const vec2 &v){ return vec2(v.x - scalar, v.y - scalar); }
inline vec2 operator - (const vec2 &v){ return vec2(-v.x, -v.y); }
inline vec2 operator * (const vec2 &u, const vec2 &v){ return vec2(u.x * v.x, u.y * v.y); }
inline vec2 operator * (const float scalar, const vec2 &v){ return vec2(v.x * scalar, v.y * scalar); }
inline vec2 operator * (const vec2 &v, const float scalar){ return vec2(v.x * scalar, v.y * scalar); }
inline vec2 operator / (const vec2 &u, const vec2 &v){ return vec2(u.x / v.x, u.y / v.y); }// NOTE: no check for divide by zero here
inline vec2 operator / (const vec2 &v, const float scalar){ float inv(1.0f/scalar); return vec2(v.x *inv, v.y *inv); }// NOTE: no check for divide by zero here
inline vec2 operator / (const float scalar, const vec2 &v){ float inv(1.0f/scalar); return vec2(v.x *inv, v.y *inv); }// NOTE: no check for divide by zero here
inline bool operator == (const vec2 &u, const vec2 &v){ return ((u.x == v.x) & (u.y == v.y));}
inline bool operator != (const vec2 &u, const vec2 &v){ return ((u.x != v.x) | (u.y != v.y));}

inline float Dot(const vec2& a, const vec2 &b) { return a.x*b.x + a.y*b.y; }
inline vec2 Floor (const vec2& a ){ return vec2(floor(a.x),floor(a.y)); }

inline vec2 Lerp(const vec2 &u, const vec2 &v, const vec2 &x){ return u + x * (v - u); }
inline vec2 Lerp(const vec2 &u, const vec2 &v, const float x){ return u + x * (v - u); }

inline vec2 Serp(const vec2 &u0, const vec2 &u1, const vec2 &u2, const vec2 &u3, const float x){
  vec2 p = (u3 - u2) - (u0 - u1);
  vec2 q = (u0 - u1) - p;
  vec2 r = u2 - u0;
  return x * (x * (x * p + q) + r) + u1;
}
inline float Length(const vec2 &a, const vec2 &b) {return (a-b).Length();}
inline float LengthSq(const vec2 &a, const vec2 &b) { return (a-b).LengthSq();}

inline bool CloseTo(const vec2& p1, const vec2& p2, const float epison = .001f){  vec2 temp(p1 - p2); return (abs(Dot(temp, temp)) < epison); }
inline vec2 abs(const vec2& v){ return vec2(abs(v.x), abs(v.y));  }
inline std::ostream& operator<<(std::ostream& strem, const vec2& obj){
  strem<<"x="<<obj.x<<"  y="<<obj.y;
  return strem;
}


/*	--------------------------------VEC3---------------------------------------------- */


class vec3 {
public:

  union{
    float data[3];
    struct{
      float x, y, z;
    };
  };
  vec3(){}// when initaling a std::vector, it is left in a scalartate of unknown!!!
  vec3(float nx, float ny, float nz): x(nx), y(ny), z(nz) {}
  vec3(const vec2& v): x(v.x), y(v.y), z(0.0f) {}
  vec3(const vec3 &a) : x(a.x), y(a.y), z(a.z) {}
  vec3(const float* v) : x(*v), y(*(v+1)), z(*(v+2)) {}// array initialization
  vec3(const vec2 &iv, const float iz): x(iv.x), y(iv.y), z(iz) {}
  vec3(const float ix, const vec2 &iv): x(ix), y(iv.x), z(iv.y) {}

  void operator += (const float scalar){ x += scalar; y += scalar; z += scalar; }
  void operator += (const vec3 &v){ x += v.x; y += v.y; z += v.z;}
  void operator -= (const float scalar){ x -= scalar; y -= scalar; z -= scalar; }
  void operator -= (const vec3 &v){ x -= v.x; y -= v.y; z -= v.z; }
  void operator *= (const float scalar){ x *= scalar; y *= scalar; z *= scalar; }
  void operator *= (const vec3 &v){ x *= v.x; y *= v.y; z *= v.z; }
  void operator /= (const float scalar){ float inv(1.0f/ scalar); x *=inv; y *=inv; z *=inv; }
  void operator /= (const vec3 &v){ x /= v.x; y /= v.y; z /= v.z; }

  // utility
  float& operator [](size_t index) { return data[index]; }
  float operator [](size_t index) const { return data[index]; }
  void zero() { x=y=z=0.0f; }// zero's out the scalartruct
  void	normalize() { float inv(1.0f /Length()); x *= inv; y *= inv; z *= inv; }// no check for div by zero!!!!!!
  float LengthSq() const{ return x*x + y*y + z*z;}
  float Length() const{ return sqrtf(LengthSq()); }
};

inline vec3 operator + (const vec3 &u, const vec3 &v){ return vec3(u.x + v.x, u.y + v.y, u.z + v.z); }
inline vec3 operator + (const vec3 &v, const float scalar){ return vec3(v.x + scalar, v.y + scalar, v.z + scalar); }
inline vec3 operator + (const float scalar, const vec3 &v){ return vec3(v.x + scalar, v.y + scalar, v.z + scalar); }
inline vec3 operator - (const vec3 &u, const vec3 &v){ return vec3(u.x - v.x, u.y - v.y, u.z - v.z); }
inline vec3 operator - (const vec3 &v, const float scalar){ return vec3(v.x - scalar, v.y - scalar, v.z - scalar); }
inline vec3 operator - (const float scalar, const vec3 &v){ return vec3(v.x - scalar, v.y - scalar, v.z - scalar); }
inline vec3 operator - (const vec3 &v){ return vec3(-v.x, -v.y, -v.z); }
inline vec3 operator * (const vec3 &u, const vec3 &v){ return vec3(u.x * v.x, u.y * v.y, u.z * v.z); }
inline vec3 operator * (const float scalar, const vec3 &v){ return vec3(v.x * scalar, v.y * scalar, v.z * scalar);}
inline vec3 operator * (const vec3 &v, const float scalar){ return vec3(v.x * scalar, v.y * scalar, v.z * scalar); }

inline vec3 operator / (const vec3 &u, const vec3 &v){ return vec3(u.x / v.x, u.y / v.y, u.z / v.z); } // no check for div by zero!!!!!!
inline vec3 operator / (const vec3 &v, const float scalar){ float inv(1.0f/scalar); return vec3(v.x *inv, v.y *inv, v.z *inv); }// no check for div by zero!!!!!!
inline vec3 operator / (const float scalar, const vec3 &v){ float inv(1.0f/scalar); return vec3(inv* v.x, inv* v.y, inv* v.z); } // no check for div by zero!!!!!!
inline bool operator == (const vec3 &u, const vec3 &v){ return ((u.x == v.x) & (u.y == v.y) & (u.z == v.z)); }
inline bool operator != (const vec3 &u, const vec3 &v){ return ((u.x != v.x) | (u.y != v.y) | (u.z != v.z)); }

inline vec3 Lerp(const vec3 &u, const vec3 &v, const vec3 &x){ return u + x * (v - u); }
inline vec3 Lerp(const vec3 &u, const vec3 &v, const float x){ return u + x * (v - u); }
vec3 Serp(const vec3 &u0, const vec3 &u1, const vec3 &u2, const vec3 &u3, const float x);

inline float Dot(const vec3& a, const vec3& b){ return b.x*a.x + b.y*a.y + b.z*a.z; }


inline bool CloseTo(const vec3& p1, const vec3& p2, const float epison = .001f){ vec3 temp(p1 - p2); return (abs(Dot(temp, temp)) < epison); }
inline vec3 abs(const vec3& v){ return vec3(abs(v.x), abs(v.y), abs(v.z));  }
inline vec3 Cross(const vec3 &a, const vec3 &b) {	return vec3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x); }

inline float Length(const vec3 &a, const vec3 &b) {return (a-b).Length();}
inline float LengthSq(const vec3 &a, const vec3 &b) { return (a-b).LengthSq();}
inline vec3 floor(const vec3& a) { return vec3(floor(a.x), floor(a.y), floor(a.z) ); }
inline vec3 ceil(const vec3& a) { return vec3(ceil(a.x), ceil(a.y), ceil(a.z) ); }

/* --------------------------------------VEC4------------------------------------ */


class vec4{
public:
  union{
#ifdef _WIN64
    __m128 vec;
#endif
    float data[4];
    struct{
      float x, y, z, w;
    };
    struct{
      float top, left, right, bottom;
    };
  };
  vec4() {}// when initaling a vec4, it is left in a state of unknown!!!
  vec4(const vec3 &a) : x(a.x), y(a.y), z(a.z), w(0.0f) {}
  vec4(float nx, float ny, float nz, float nw) : x(nx), y(ny), z(nz), w(nw) {}
  vec4(const vec2& iv, const float nz, const float nw): x(iv.x), y(iv.y), z(nz), w(nw) {}
  vec4(const float nx, const vec2& iv, const float nw): x(nx), y(iv.x), z(iv.y), w(nw) {}
  vec4(const float nx, const float ny, const vec2& iv): x(nx), y(ny), z(iv.x), w(iv.y) {}
  vec4(const vec2& nv0, const vec2& nv1): x(nv0.x), y(nv0.y), z(nv1.x), w(nv1.y) {}
  vec4(const vec3& iv, const float iw): x(iv.x), y(iv.y), z(iv.z), w(iw) {}
  vec4(const float ix, const vec3& iv): x(ix), y(iv.x), z(iv.y), w(iv.z) {}
  vec4(const float* v) : x(*v), y(*(v+1)), z(*(v+2)), w(*(v+3)) {}// array initialization

#ifdef _WIN64
  vec4(__m128& ve) : vec(ve) {}
  vec4(const vec4 &a) : vec(a.vec) {}
  void operator += (const float scalar){ vec = _mm_add_ps(vec, _mm_set_ps1(scalar));}
  void operator += (const vec4 &v){ vec = _mm_add_ps(vec, v.vec);}
  void operator -= (const float scalar){ vec = _mm_sub_ps(vec, _mm_set_ps1(scalar));}
  void operator -= (const vec4 &v){ vec = _mm_sub_ps(vec, v.vec);}
  void operator *= (const float scalar){ vec = _mm_mul_ps(vec, _mm_set_ps1(scalar));}
  void operator *= (const vec4 &v){ vec = _mm_mul_ps(vec, v.vec);}
  void operator /= (const float scalar){ vec = _mm_div_ps(vec, _mm_set_ps1(scalar));}
  void operator /= (const vec4 &v){ vec = _mm_div_ps(vec, v.vec);}
  void zero() { vec = _mm_setzero_ps(); }// zero's out the struct
  void normalize() {
    vec = _mm_mul_ps(vec, _mm_set_ps1(1.0f / Length()));
  }
  float LengthSq() const{
    __m128 temp(_mm_mul_ps(vec, vec));
    float* t(reinterpret_cast<float*>(&temp));
    return t[0]+ t[1]+t[2]+t[3];
  }
  void floor() { x = floorf(x); y=floorf(y); z=floorf(z); w=floorf(w);  }
  void ceil() { x = ceilf(x); y=ceilf(y); z=ceilf(z); w=ceilf(w);  }
#else
  vec4(const vec4 &a) : x(a.x), y(a.y), z(a.z), w(a.w) {}
  void operator += (const float scalar){ 	x += scalar; y += scalar; z += scalar; w += scalar; }
  void operator += (const vec4 &v){  x += v.x; y += v.y; z += v.z; w += v.w; }
  void operator -= (const float scalar){ 	x -= scalar;y -= scalar;z -= scalar;w -= scalar; }
  void operator -= (const vec4 &v){ 	x -= v.x;y -= v.y;z -= v.z; w -= v.w;}
  void operator *= (const float scalar){ 	x *= scalar;y *= scalar;z *= scalar; w *= scalar;}
  void operator *= (const vec4 &v){ 	x *= v.x;y *= v.y;z *= v.z; w *= v.w;}
  void operator /= (const float scalar){ 	float inv(1.0f/scalar); x *= inv; y *= inv; z *= inv; w *= inv;}
  void operator /= (const vec4 &v){  x /= v.x;y /= v.y;z /= v.z; w /= v.w;}
  void zero() { x=y=z=w=0.0f; }// zero's out the struct
  void normalize() {  float inv(1.0f /Length()); x *= inv; y *= inv; z *= inv; w *= inv; } // no check for div by zero!!!!!!
  float LengthSq() const{ return x*x + y*y + z*z + w*w; }
#endif
  operator uint32_t () const {// convert from [0, 1] to rgba 8
    uint32_t dwR = x >= 1.0f ? 0xff : x <= 0.0f ? 0x00 : (uint32_t) (x * 255.0f + 0.5f);
    uint32_t dwG = y >= 1.0f ? 0xff : y <= 0.0f ? 0x00 : (uint32_t) (y * 255.0f + 0.5f);
    uint32_t dwB = z >= 1.0f ? 0xff : z <= 0.0f ? 0x00 : (uint32_t) (z * 255.0f + 0.5f);
    uint32_t dwA = w >= 1.0f ? 0xff : w <= 0.0f ? 0x00 : (uint32_t) (w * 255.0f + 0.5f);
    return (dwA << 24) | (dwR << 16) | (dwG << 8) | (dwB << 0);
  }

  float Length() const{ return sqrtf(LengthSq()); }
  float& operator [](size_t index) { return data[index]; }
  float operator [](size_t index) const { return data[index]; }

};

#ifdef _WIN64

inline vec4 operator + (const vec4 &u, const vec4 &v){ return vec4(_mm_add_ps(u.vec, v.vec)); }
inline vec4 operator + (const vec4 &v, const float s){ return vec4(_mm_add_ps(v.vec, _mm_set_ps1(s))); }
inline vec4 operator + (const float s, const vec4 &v){ return vec4(_mm_add_ps(_mm_set_ps1(s), v.vec)); }
inline vec4 operator - (const vec4 &u, const vec4 &v){ return vec4(_mm_sub_ps(u.vec, v.vec)); }
inline vec4 operator - (const vec4 &v, const float s){ return vec4(_mm_sub_ps(v.vec, _mm_set_ps1(s))); }
inline vec4 operator - (const float s, const vec4 &v){ return vec4(_mm_sub_ps( _mm_set_ps1(s), v.vec)); }
inline vec4 operator - (const vec4 &v){ return vec4(_mm_mul_ps(_mm_set_ps1(-1.0f), v.vec));  }
inline vec4 operator * (const vec4 &u, const vec4 &v){ return vec4(_mm_mul_ps(u.vec, v.vec)); }
inline vec4 operator * (const float s, const vec4 &v){ return vec4(_mm_mul_ps(_mm_set_ps1(s), v.vec)); }
inline vec4 operator * (const vec4 &v, const float s){ return vec4(_mm_mul_ps( v.vec, _mm_set_ps1(s))); }
inline vec4 operator / (const vec4 &u, const vec4 &v){ return vec4(_mm_div_ps(u.vec, v.vec)); }
inline vec4 operator / (const vec4 &v, const float s){ return vec4(_mm_div_ps(v.vec, _mm_set_ps1(s))); }
inline vec4 operator / (const float s, const vec4 &v){ return vec4(_mm_div_ps(_mm_set_ps1(s), v.vec)); }

inline vec4 Min (const vec4& a, const vec4& b){ return vec4(_mm_min_ps(a.vec, b.vec)); }
inline vec4 Max (const vec4& a, const vec4& b){ return vec4(_mm_max_ps(a.vec, b.vec)); }

inline float Dot(const vec4& a, const vec4& b){
  __m128 ts(_mm_mul_ps(a.vec, b.vec));
  float* t(reinterpret_cast<float*>(&ts));
  return t[0] + t[1]+t[2]+t[3];// using the _mm_hadd_ps function is slower than doing this... dont believe me? try it
}
inline vec4 floor(const vec4& a) { return vec4(floorf(a.x),floorf(a.y),floorf(a.z),floorf(a.w));  }
inline vec4 ceil(const vec4& a) { return vec4(ceilf(a.x),ceilf(a.y),ceilf(a.z),ceilf(a.w));  }
#else
inline vec4 operator + (const vec4 &u, const vec4 &v){ return vec4(u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w); }
inline vec4 operator + (const vec4 &v, const float s){ return vec4(v.x + s, v.y + s, v.z + s, v.w + s); }
inline vec4 operator + (const float s, const vec4 &v){ return vec4(v.x + s, v.y + s, v.z + s, v.w + s); }
inline vec4 operator - (const vec4 &u, const vec4 &v){ return vec4(u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w); }
inline vec4 operator - (const vec4 &v, const float s){ return vec4(v.x - s, v.y - s, v.z - s, v.w - s); }
inline vec4 operator - (const float s, const vec4 &v){ return vec4(v.x - s, v.y - s, v.z - s, v.w - s); }
inline vec4 operator - (const vec4 &v){ return vec4(-v.x, -v.y, -v.z, -v.w); }
inline vec4 operator * (const vec4 &u, const vec4 &v){ return vec4(u.x * v.x, u.y * v.y, u.z * v.z, u.w * v.w); }
inline vec4 operator * (const float s, const vec4 &v){ return vec4(v.x * s, v.y * s, v.z * s, v.w * s); }
inline vec4 operator * (const vec4 &v, const float s){ return vec4(v.x * s, v.y * s, v.z * s, v.w * s); }
inline vec4 operator / (const vec4 &u, const vec4 &v){ return vec4(u.x / v.x, u.y / v.y, u.z / v.z, u.w / v.w); }
inline vec4 operator / (const vec4 &v, const float s){ return vec4(v.x / s, v.y / s, v.z / s, v.w / s); }
inline vec4 operator / (const float s, const vec4 &v){ return vec4(s / v.x, s / v.y, s / v.z, s / v.w); }
inline float Dot(const vec4& a, const vec4& b){ return b.x*a.x + b.y*a.y + b.z*a.z + b.w*a.w; }

#endif

inline bool operator == (const vec4 &u, const vec4 &v){ return (u.x == v.x) & (u.y ==v.y) & ( u.z==v.z) & ( u.w==v.w); }
inline bool operator != (const vec4 &u, const vec4 &v){ return (u.x != v.x) | (u.y !=v.y) | ( u.z!=v.z) | ( u.w!=v.w); }

inline vec4 Lerp(const vec4 &u, const vec4 &v, const vec4 &x){ return u + x * (v - u); }
inline vec4 Lerp(const vec4 &u, const vec4 &v, const float x){ return u + x * (v - u); }
inline vec4 Serp(const vec4 &u0, const vec4 &u1, const vec4 &u2, const vec4 &u3, const float x){
  vec4 p = (u3 - u2) - (u0 - u1);
  vec4 q = (u0 - u1) - p;
  vec4 r = u2 - u0;
  return x * (x * (x * p + q) + r) + u1;
}
inline bool CloseTo(const vec4& p1, const vec4& p2, const float epison = MATH_EPS){   vec4 temp(p1 - p2); return (abs(Dot(temp, temp)) < epison); }

extern const vec4 ABSROWVAL;

inline vec4 abs(const vec4& v){
#ifdef _WIN64
  return vec4(_mm_and_ps(v.vec, ABSROWVAL.vec ));
#else
  return vec4(abs(v.x), abs(v.y), abs(v.z), abs(v.w));
#endif
}
inline std::ostream& operator<<(std::ostream& strem, const vec4& obj){
  strem<<"x="<<obj.x<<"  y="<<obj.y<<"  z="<<obj.z<<"  w="<<obj.w;
  return strem;
}
inline void copy(vec4& dst, __m128* src){// copy used for 32 bit because the compiler cannot guarantee alignment of __m128 on the stack
  dst.x = *(reinterpret_cast<float*>(src) + 0);
  dst.y = *(reinterpret_cast<float*>(src) + 1);
  dst.z = *(reinterpret_cast<float*>(src) + 2);
  dst.w = *(reinterpret_cast<float*>(src) + 3);
}
inline void copy( __m128* dst, const vec4& src){// copy used for 32 bit because the compiler cannot guarantee alignment of __m128 on the stack
  *(reinterpret_cast<float*>(dst) + 0) = src.x;
  *(reinterpret_cast<float*>(dst) + 1) = src.y;
  *(reinterpret_cast<float*>(dst) + 2) = src.z;
  *(reinterpret_cast<float*>(dst) + 3) = src.w;
}



#endif // MYMATH_HPP
