static char *rcsid= "$Id: glview.c,v 2.23 2001/03/08 17:37:28 fuhrmann Exp $";

#include "gltensview.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <time.h>

static int nx,ny,nz;
static double func(double x, double y, double z, double t)
{
  double r,xx,yy;
  xx=x-0.2;
  yy=y-0.2;
  

  r=exp(-(xx*xx+yy*yy)*20.0)*cos(0.3*t);

  xx=x-0.8;
  yy=y-0.8;
  r+=exp(-(xx*xx+yy*yy)*30.0)*sin(t);

  xx=x-0.2;
  yy=y-0.8;
  r+=exp(-(xx*xx+yy*yy)*40.0)*cos(2.5*t);
  
  xx=x-0.5;
  yy=y-0.5;
  r+=exp(-(xx*xx+yy*yy)*40.0)*sin(3.0*t);
  return r*cos(6.3832*z);
}

static void flux(double x, double y, double z, double t, double *flx)
{
  double r,xx,yy;
  xx=x-0.2;
  yy=y-0.2;
  

  xx=x-0.8;
  yy=y-0.8;
  flx[1]=3*exp(-(xx*xx+yy*yy)*30.0)*sin(t)+cos(0.1*t);

  xx=x-0.2;
  yy=y-0.8;
  flx[2]=3*exp(-(xx*xx+yy*yy)*40.0)*cos(2.5*t)+sin(0.3*t);
  
  xx=x-0.5;
  yy=y-0.5;
  flx[3]=exp(-(xx*xx+yy*yy)*40.0)*sin(3.0*t)+sin(0.5*t);

}









static double tics[9]={0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9};

void main(int argc, char *argv[])
{
  GLTensorMesh m;
  double time=0.0;
  double h=0.05;
  double t;
  int iarg;

  char *datafile=0;
  int nframes=0;
  int cellflux=0;
  int nodefunc=1;
  iarg=1;
  printf("This is glview $Revision: 2.23 $.\n");
  nx=ny=nz=20;
  while (iarg<argc)
    {
      if      (strcmp(argv[iarg],"-nframes")==0) {  nframes=atoi(argv[iarg+1]); iarg+=2;}
      else if  (strcmp(argv[iarg],"-nx")==0) {nx=atoi(argv[iarg+1]); iarg+=2;}
      else if  (strcmp(argv[iarg],"-ny")==0) {ny=atoi(argv[iarg+1]); iarg+=2;}
      else if  (strcmp(argv[iarg],"-nz")==0) {nz=atoi(argv[iarg+1]); iarg+=2;}
      else if  (strcmp(argv[iarg],"-cellflux")==0) {cellflux=1; nodefunc=0;iarg+=1;}
      else {datafile=argv[iarg]; iarg++;}
     }
  
  if (datafile)
    {
      FILE *in;
      assert(in=fopen(datafile,"r"));
      assert(m=gltvCreateFile(in));
      gltvData(m);
    }
  else
    {
      m=gltvCreateSimple(nx,ny,nz,0.0,1.0,0.0,1.0,0.0,1.0);
      for (time=1.5;;time=time+h)
	{
	  if (nodefunc)
	    {
	      if (!gltvNodeFuncf(m,func,time,1,-1)) break;
	    }
	  else 
	    if (cellflux)
	      {
		if (!gltvCellFluxf(m,flux,time,1,-1)) break;
	      }
	}
    }
  /*
  glrSetAxisTics(rnd,'x',9,tics);
  glrSetAxisTics(rnd,'y',9,tics);
  glrSetAxisTics(rnd,'z',9,tics);
  glrSetAxisTics(rnd,'f',9,tics);
  */
  
  gltvDestroy(m);
}

/*
 * $Log: glview.c,v $
 * Revision 2.23  2001/03/08 17:37:28  fuhrmann
 * First running version of gltensview stuff
 *
 * Revision 2.22  2001/03/08 16:18:30  fuhrmann
 * interim
 *
 * Revision 2.21  2000/10/17 16:46:36  fuhrmann
 * started scl 3d visualization
 *
 * Revision 2.20  2000/02/14 15:53:51  fuhrmann
 *  user material color callback
 *
 * Revision 2.19  1999/04/29 17:46:33  fuhrmann
 * various changes
 *
 * Revision 2.18  1999/04/09 12:20:29  fuhrmann
 * 3d cell flux running, will need raster point method
 *
 * Revision 2.17  1999/04/09 11:25:08  fuhrmann
 * first examples running with flux stuff
 *
 * Revision 2.16  1999/04/08 15:03:15  fuhrmann
 * glmesh split up
 *
 * Revision 2.15  1999/03/12 16:26:01  fuhrmann
 * scaling bug in data file case removed
 *
 * Revision 2.14  1999/02/05 20:24:35  fuhrmann
 * tics introduced
 *
 * Revision 2.13  1999/02/01 18:36:34  fuhrmann
 * minor changes
 *
 * Revision 2.12  1999/01/26 17:14:49  fuhrmann
 * time depenedent demo mode in glview
 * color interpolation for 3D sections in glmesh
 *
 * Revision 2.11  1998/07/27  19:10:04  fuhrmann
 * material stuff in glmesh
 *
 * Revision 2.10  1998/07/06  10:45:12  fuhrmann
 * Increased compiler warning level for all PDELIB-1.7 modules,
 * removed all bugs, some warnings remain (rcsids, ld_init, unused parameters)
 *
 * Revision 2.9  1998/03/06  16:45:31  fuhrmann
 * tiff->ppm
 * print command
 * video recording
 *
 * Revision 2.8  1997/11/04  15:53:30  fuhrmann
 * color scales ; crash with glrnd
 *
 * Revision 2.7  1997/03/19  11:07:17  fuhrmann
 * setperspective 'raus
 *
 * Revision 2.6  1997/02/28  14:15:06  fuhrmann
 * some return values (SGI)
 *
 * Revision 2.5  1996/09/06  18:16:14  fuhrmann
 * change of glmesh interface
 *
 * Revision 2.4  1996/09/04  08:40:18  fuhrmann
 * video format error removed
 *
 * Revision 2.3  1996/09/03  11:25:45  fuhrmann
 * window size fitted to video
 *
 * Revision 2.2  1996/09/03  09:08:25  fuhrmann
 * Default window size from glrnd
 *
 * Revision 2.1  1996/03/28  11:30:25  fuhrmann
 * 2000 x 2000 default pixmap for offscreen rendering test
 *
 * Revision 2.0  1996/02/15  19:57:10  fuhrmann
 * First meta-stable distribution
 *
 * Revision 1.2  1995/10/23  21:32:51  fuhrmann
 * Revision number in output
 *
 * Revision 1.1  1995/10/20  15:42:03  fuhrmann
 * Initial revision
 *
 */
