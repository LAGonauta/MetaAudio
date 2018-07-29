// Copyright 2018
// Licensed under GPLv3
// Refer to the LICENSE.md file included.

#ifndef UTIL_H
#define UTIL_H

#include <limits>
#include <math.h>

//Vector
#ifndef M_PI
#define M_PI 3.141592653589
#endif

inline float DotProduct(float *x, float *y)
{
  return (x[0] * y[0] + x[1] * y[1] + x[2] * y[2]);
}

#define VectorSubtract(a,b,c) {(c)[0]=(a)[0]-(b)[0];(c)[1]=(a)[1]-(b)[1];(c)[2]=(a)[2]-(b)[2];}
#define VectorAdd(a,b,c) {(c)[0]=(a)[0]+(b)[0];(c)[1]=(a)[1]+(b)[1];(c)[2]=(a)[2]+(b)[2];}
#define VectorCopy(a,b) {(b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2];}
#define VectorMultiply(a,b,c) {(c)[0]=(a)[0]*(b);(c)[1]=(a)[1]*(b);(c)[2]=(a)[2]*(b);}
#define VectorClear(a) {(a)[0]=0.0;(a)[1]=0.0;(a)[2]=0.0;}

inline void VectorMA(float *a, float scale, float *b, float *c)
{
  c[0] = a[0] + scale * b[0];
  c[1] = a[1] + scale * b[1];
  c[2] = a[2] + scale * b[2];
}

#define CrossProduct(a,b,c) ((c)[0]=(a)[1]*(b)[2]-(a)[2]*(b)[1],(c)[1]=(a)[2]*(b)[0]-(a)[0]*(b)[2],(c)[2]=(a)[0]*(b)[1]-(a)[1]*(b)[0])

inline void VectorScale(float *a, float b)
{
  a[0] = b * a[0];
  a[1] = b * a[1];
  a[2] = b * a[2];
}

inline void VectorScale(const vec3_t& in, vec_t scale, vec3_t& result)
{
  VectorMultiply(in, scale, result);
}

inline float VectorLength(float *a)
{
  return sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
}

inline void VectorNormalize(float *a)
{
  float flLength = VectorLength(a);
  a[0] = a[0] / flLength;
  a[1] = a[1] / flLength;
  a[2] = a[2] / flLength;
}

inline void VectorVectors(const vec3_t &forward, vec3_t &right, vec3_t &up)
{
  vec3_t tmp;

  // Comparison not ideal, but works for us.
  if (forward[0] <= std::numeric_limits<float>::epsilon() && forward[1] <= std::numeric_limits<float>::epsilon())
  {
    // pitch 90 degrees up/down from identity
    right[0] = 0;
    right[1] = -1;
    right[2] = 0;
    up[0] = -forward[2];
    up[1] = 0;
    up[2] = 0;
  }
  else
  {
    tmp[0] = 0; tmp[1] = 0; tmp[2] = 1.0;
    CrossProduct(forward, tmp, right);
    VectorNormalize(right);
    CrossProduct(right, forward, up);
    VectorNormalize(up);
  }
}

inline float VectorAngle(vec3_t a, vec3_t b)
{
  float la = VectorLength(a);
  float lb = VectorLength(b);
  float lab = la * lb;
  if (lab == 0.0)
    return 0.0;
  else
    return static_cast<float>(acos(DotProduct(a, b) / lab) * (180 / M_PI));
}

inline float ApproachVal(float target, float value, float speed)
{
  float delta = target - value;

  if (delta > speed)
    value += speed;
  else if (delta < -speed)
    value -= speed;
  else value = target;

  return value;
}

wchar_t *UTF8ToUnicode(const char* str);
wchar_t *ANSIToUnicode(const char* str);
char *UnicodeToANSI(const wchar_t* str);
char *UnicodeToUTF8(const wchar_t* str);

char *UTIL_VarArgs(char *format, ...);

#define va UTIL_VarArgs

#define XX (g_x->value)
#define YY (g_y->value)
#define WW (g_w->value)
#define HH (g_h->value)

#define RANDOM_FLOAT gEngfuncs.pfnRandomFloat
#define RANDOM_LONG gEngfuncs.pfnRandomLong

#endif