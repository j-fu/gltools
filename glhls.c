static char *rcsid="$Id: glhls.c,v 1.1 2000/06/28 08:46:39 fuhrmann Exp $";
#include <math.h>
#include "GL/gl.h"
#include "glhls.h"

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX3(a,b,c) MAX(MAX(a,b),c)
#define MIN3(a,b,c) MIN(MIN(a,b),c)
#define PI 3.141592683
#define TWOPI 2.0*PI

static float v(float m1, float m2, float hue)
{
    hue = hue-floor(hue);
    if (hue < 1.0/6.0) return m1 + (m2-m1)*hue*6.0;
    if (hue < 0.5) return m2;
    if (hue < 2.0/3.0) return m1 + (m2-m1)*(2.0/3.0-hue)*6.0;
    return m1;
}


float* glhlsConvertToRGB(float *hls, float *rgb)
{
  double m1,m2;
  if (hls[2] == 0.0) 
    {
      rgb[0]=rgb[1]=rgb[2]= hls[1];
      return rgb;
    }
  if (hls[1] <= 0.5)
    m2 = hls[1] * (1.0+hls[2]);
  else 
    m2 = hls[1]+hls[2]-(hls[1]*hls[2]);
  
  m1 = 2.0*hls[1] - m2;
  rgb[0]=v(m1, m2, hls[0]+1.0/3.0);
  rgb[1]=v(m1, m2, hls[0]);
  rgb[2]=v(m1, m2, hls[0]-1.0/3.0);
  return rgb;
}

void glhlsColor3d(double h, double  l, double s)
{
  float hls[3],rgb[3];
  hls[0]=h;
  hls[1]=l;
  hls[2]=s;
  glColor3fv(glhlsConvertToRGB(hls,rgb));
}

void glhlsColor3dv(double *dhls)
{
  float hls[3],rgb[3];
  hls[0]=dhls[0];
  hls[1]=dhls[1];
  hls[2]=dhls[2];
  glColor3fv(glhlsConvertToRGB(hls,rgb));
}

void glhlsColor3f(float h, float  l, float  s)
{
  float hls[3],rgb[3];
  hls[0]=h;
  hls[1]=l;
  hls[2]=s;
  glColor3fv(glhlsConvertToRGB(hls,rgb));
}


void glhlsColor3fv(float *hls)
{
  float rgb[3];
  glColor3fv(glhlsConvertToRGB(hls,rgb));
}




/*unchecked code */
float* rgb_to_hls(float *rgb, float *hls)
{
  float rgbc[3];
  double maxc = MAX3(rgb[0],rgb[1],rgb[2]);
  double minc = MIN3(rgb[0],rgb[1],rgb[2]);
  hls[0]=hls[1]=hls[2]=0.0;
  
  hls[1] = (minc+maxc)/2.0;
  if (minc == maxc) return hls;
  if (hls[1] <= 0.5) 
    hls[2] = (maxc-minc) / (maxc+minc);
  else  
    hls[2] = (maxc-minc) / (2.0-maxc-minc);
  rgbc[0] = (maxc-rgb[0]) / (maxc-minc);
  rgbc[1] = (maxc-rgb[1]) / (maxc-minc);
  rgbc[2] = (maxc-rgb[2]) / (maxc-minc);
  if (rgb[0] == maxc) hls[0] = rgbc[2]-rgbc[1];
  else if (rgb[1] == maxc) hls[0] = 2.0+rgbc[0]-rgbc[2];
  else hls[0] = 4.0+rgbc[1]-rgbc[0];
  hls[0]/=6.0;
  hls[0]-=floor(hls[0]);
  return hls;
}


/* This is colorsys.py. See the hint in the header file.
"""Conversion functions between RGB and other color systems.

This modules provides two functions for each color system ABC:

  rgb_to_abc(r, g, b) --> a, b, c
  abc_to_rgb(a, b, c) --> r, g, b

All inputs and outputs are triples of floats in the range [0.0...1.0].
Inputs outside this range may cause exceptions or invalid outputs.

Supported color systems:
RGB: Red, Green, Blue components
YIQ: used by composite video signals
HLS: Hue, Luminance, S???
HSV: Hue, Saturation, Value(?)
"""
# References:
# XXX Where's the literature?


# Some floating point constants

ONE_THIRD = 1.0/3.0
ONE_SIXTH = 1.0/6.0
TWO_THIRD = 2.0/3.0


# YIQ: used by composite video signals (linear combinations of RGB)
# Y: perceived grey level (0.0 == black, 1.0 == white)
# I, Q: color components

def rgb_to_yiq(r, g, b):
    y = 0.30*r + 0.59*g + 0.11*b
    i = 0.60*r - 0.28*g - 0.32*b
    q = 0.21*r - 0.52*g + 0.31*b
    return (y, i, q)

def yiq_to_rgb(y, i, q):
    r = y + 0.948262*i + 0.624013*q
    g = y - 0.276066*i - 0.639810*q
    b = y - 1.105450*i + 1.729860*q
    if r < 0.0: r = 0.0
    if g < 0.0: g = 0.0
    if b < 0.0: b = 0.0
    if r > 1.0: r = 1.0
    if g > 1.0: g = 1.0
    if b > 1.0: b = 1.0
    return (r, g, b)


# HLS: Hue, Luminance, S???
# H: position in the spectrum
# L: ???
# S: ???

def rgb_to_hls(r, g, b):
    maxc = max(r, g, b)
    minc = min(r, g, b)
    # XXX Can optimize (maxc+minc) and (maxc-minc)
    l = (minc+maxc)/2.0
    if minc == maxc: return 0.0, l, 0.0
    if l <= 0.5: s = (maxc-minc) / (maxc+minc)
    else: s = (maxc-minc) / (2.0-maxc-minc)
    rc = (maxc-r) / (maxc-minc)
    gc = (maxc-g) / (maxc-minc)
    bc = (maxc-b) / (maxc-minc)
    if r == maxc: h = bc-gc
    elif g == maxc: h = 2.0+rc-bc
    else: h = 4.0+gc-rc
    h = (h/6.0) % 1.0
    return h, l, s

def hls_to_rgb(h, l, s):
    if s == 0.0: return l, l, l
    if l <= 0.5: m2 = l * (1.0+s)
    else: m2 = l+s-(l*s)
    m1 = 2.0*l - m2
    return (_v(m1, m2, h+ONE_THIRD), _v(m1, m2, h), _v(m1, m2, h-ONE_THIRD))

def _v(m1, m2, hue):
    hue = hue % 1.0
    if hue < ONE_SIXTH: return m1 + (m2-m1)*hue*6.0
    if hue < 0.5: return m2
    if hue < TWO_THIRD: return m1 + (m2-m1)*(TWO_THIRD-hue)*6.0
    return m1


# HSV: Hue, Saturation, Value(?)
# H: position in the spectrum
# S: ???
# V: ???

def rgb_to_hsv(r, g, b):
    maxc = max(r, g, b)
    minc = min(r, g, b)
    v = maxc
    if minc == maxc: return 0.0, 0.0, v
    s = (maxc-minc) / maxc
    rc = (maxc-r) / (maxc-minc)
    gc = (maxc-g) / (maxc-minc)
    bc = (maxc-b) / (maxc-minc)
    if r == maxc: h = bc-gc
    elif g == maxc: h = 2.0+rc-bc
    else: h = 4.0+gc-rc
    h = (h/6.0) % 1.0
    return h, s, v

def hsv_to_rgb(h, s, v):
    if s == 0.0: return v, v, v
    i = int(h*6.0) # XXX assume int() truncates!
    f = (h*6.0) - i
    p = v*(1.0 - s)
    q = v*(1.0 - s*f)
    t = v*(1.0 - s*(1.0-f))
    if i%6 == 0: return v, t, p
    if i == 1: return q, v, p
    if i == 2: return p, v, t
    if i == 3: return p, q, v
    if i == 4: return t, p, v
    if i == 5: return v, p, q
    # Cannot get here
*/

/*
 *$Log: glhls.c,v $
 *Revision 1.1  2000/06/28 08:46:39  fuhrmann
 *glhls added
 *
 */













