static char *rcsid="$Id: glm-color.c,v 2.6 2001/05/15 15:23:45 petzoldt Exp $";
#include "glm-internal.h"
#include <math.h>
#define GLM_MAX_MAT 19
float glm_material_color[GLM_MAX_MAT][3]={
  {0.5,0.5,0.5},

  {1.0, 0.0, 0.0},
  {0.0, 1.0, 0.0},
  {0.0, 0.0, 1.0},

  {1.0, 1.0, 0.0},
  {1.0, 0.0, 1.0},
  {0.0, 1.0, 1.0},

  {1.0, 0.25, 0.25},
  {0.25, 1.0, 0.25},
  {0.25, 0.25, 1.0},

  {1.0, 1.0, 0.25},
  {1.0, 0.25, 1.0},
  {0.25, 1.0, 1.0},

  {1.0, 0.5, 0.25},
  {1.0, 0.5, 1.0},
  {0.5, 1.0, 1.0},

  {1.0, 0.0, 0.25},
  {1.0, 0.25, 0.0},
  {0.25, 0.0, 1.0}

};


float *glmDefaultMaterialColor(glMesh m, int i, float *rgbx)
{
  return glm_material_color[i];
}
double p=0.0;


/*default color function */
float* glmRBColor(glMesh m,double f, float *rgbx)
{
  double x;
  if (f>m->max) f=m->max;
  if (f<m->min) f=m->min;
  x=(f-m->min)*m->cdelta;
  if (p>0) x=1.0-pow(1.0-x,p); else if (p<0) x=pow(x,-p);
  rgbx[0]=x;
  rgbx[1]=0;
  rgbx[2]=1.0-x;
  return rgbx;
}

/*default color function */
float* glmThreePhaseColor(glMesh m,double f, float *rgbx)
{
  double x;
  double per=6.28*m->glm_color_fac;
  double offset=m->glm_color_ctrl;

  if (f>m->max) f=m->max;
  if (f<m->min) f=m->min;
  x=(f-m->min)*m->cdelta;
  rgbx[0]=0.5*(cos(per*x+offset)+1.0);
  rgbx[1]=0.5*(cos(per*x-2.0*3.141/3.0+offset)+1.0);
  rgbx[2]=0.5*(cos(per*x-4.0*3.141/3.0+offset)+1.0);
  return rgbx;
}

float* glmContrastColor(glMesh m,double f, float *rgbx)
{
  double x;
  double per=5.0;
  if (f>m->max) f=m->max;
  if (f<m->min) f=m->min;
  x=(f-m->min)*m->cdelta;
  x=0.5*(cos(per*x*3.141592653)+1.0);
  rgbx[0]=x;
  rgbx[1]=0;
  rgbx[2]=1.0-x;
  return rgbx;
}

float* glmBWColor(glMesh m,double f, float *rgbx)
{
  double x;
  double offset=m->glm_color_ctrl;
  if (f>m->max) f=m->max;
  if (f<m->min) f=m->min;
  x=(f-m->min)*m->cdelta;

  x=(-offset)+offset*x;
  if (x > 0.6)
    x=0.6;
  x=x-floor(x);
  /* jf??
  if (p>0) x=1.0-pow(1.0-x,p); else if (p<0) x=pow(x,-p);
   x=0.1+0.8*x;
   */
  rgbx[0]=x;
  rgbx[1]=x;
  rgbx[2]=x;
  return rgbx;
}


float*  glmSteelColor(glMesh m,double f, float *rgbx)
{
  double x;
  if (f>m->max) f=m->max;
  if (f<m->min) f=m->min;
  x=(f-m->min)*m->cdelta;
  if (p>0) x=1.0-pow(1.0-x,p); else if (p<0) x=pow(x,-p);


  /* blau-> rot */
  if (x<=0.333)
    {
      rgbx[0]=3.0*x;
      rgbx[1]=0.0;
      rgbx[2]=1.0-3.0*x;
    }
  else if (x<=0.666)
    {
      rgbx[0]=1.0;
      rgbx[1]=3.0*(x-0.333);
      rgbx[2]=0.0;
    }
  else
    {
      rgbx[0]=1.0;
      rgbx[1]=1.0;
      rgbx[2]=3.0*(x-0.666);
    }
  return rgbx;
}

float*  glmGrayColor(glMesh m,double f, float *rgbx)
{
  double offset=m->glm_color_ctrl;
  double clr=(0.5+offset)-floor(0.5+offset);
  rgbx[0]=clr;
  rgbx[1]=clr;
  rgbx[2]=clr;
  return rgbx;
}

float*  glmMapColor(glMesh m,double f, float *rgbx)
{
  double x,scale=0;
  if (f>m->max) f=m->max;
  if (f<m->min) f=m->min;
  x=(f-m->min)*m->cdelta;
  if (-m->min >m->max) scale= -1.0/m->min;
  x=f*scale;
  if (p>0) x=1.0-pow(1.0-x,p); else if (p<0) x=pow(x,-p);

  /* blau-> Hellblau */
  if (x<=0.0)
    {
      rgbx[0]=0.333+0.333*x;
      rgbx[1]=rgbx[0];
      rgbx[2]=1.0+0.5*x;
    }
  else
    {
      rgbx[0]=x;
      rgbx[1]=1.0-x;
      rgbx[2]=0.0;
    }
  return rgbx;
}

/*
 * $Log: glm-color.c,v $
 * Revision 2.6  2001/05/15 15:23:45  petzoldt
 * Martins Farbskala: Konflikte beseitigt
 *
 * Revision 2.5  2000/10/18 08:41:50  fuhrmann
 *  scl_3d next step
 *
 * Revision 2.4  2000/08/04 16:31:40  fuhrmann
 * some little more annoyning bugs...
 *
 * Revision 2.3  2000/02/14 15:53:51  fuhrmann
 *  user material color callback
 *
 * Revision 2.2  1999/04/13 11:43:31  fuhrmann
 * more thorough checks, k&r stuff deleted
 *
 * Revision 2.1  1999/04/08 15:03:14  fuhrmann
 * glmesh split up
 *
 */
