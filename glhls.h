/*$Id: glhls.h,v 1.1 2000/06/28 08:46:39 fuhrmann Exp $*/


/*
This code is does the obvious: it handles hls color definitions
in an OpenGL  style Interface and maps them to glColor3XX.

Please note that from the OpenGL efficiency point of view,
this code is a crime as it emits glColor commands after quite
lengthy calculations of hls color. The real thing should do 
something else to keep the rendering pipeline fed.

The code  has been cloned from colorsys.py of python-1.5.2
on Suse Linux 6.3, the source is included in the implementation.
*/

float* glhlsConvertToRGB(float *hls, float *rgb);
void glhlsColor3d(double h, double  l, double s);
void glhlsColor3dv(double *hls);
void glhlsColor3f(float h, float  l, float  s);
void glhlsColor3fv(float *hls);

/*unchecked code */
float* rgb_to_hls(float *rgb, float *hls);
/*
 *$Log: glhls.h,v $
 *Revision 1.1  2000/06/28 08:46:39  fuhrmann
 *glhls added
 *
 */














