static char *rcsid="$Id: glmesh.c,v 2.42 2001/05/15 15:23:45 petzoldt Exp $";
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "glmesh.h"
#include "glm-internal.h"

/*nisolev=21;
isoline_distance=0.05;
icol=0;
eval=0
showmat=0
*/

/* singleton data -- put this into application options of rnd */
static int eval=0;
static int icol=0;
int glm_showmat;
static int initialized=0;

void glm_set_colorscale(glRenderer rnd, glMesh m)
{
  switch(icol)
    {
    case 1:m->color=glmRBColor;break;
    case 2:m->color=glmMapColor; break;
    case 3:m->color=glmBWColor; break;
    case 4:m->color=glmGrayColor; break;
    case 5:m->color=glmContrastColor; break;
    case 6:m->color=glmThreePhaseColor; break;
    default: m->color=glmSteelColor; break;
    }
}

int glm_flip_colorscale(glRenderer rnd,  int mask)
{
  glMesh m=(glMesh)glrGetInfo(rnd);
  icol=(icol+1)%7;		/* 7: number of color scales */
  glm_set_colorscale(rnd,m);
  return GL_TRUE;
}

int glm_toggle_eval(glRenderer rnd, int mask)
{

  eval=!eval;
  return GL_TRUE;
}

int glm_toggle_mesh_mat(glRenderer rnd, int mask)
{

  glm_showmat=(glm_showmat+1) %2;
  return GL_TRUE;
}


double glm_scale_func(glMesh m, double f)
{
  return m->plane+m->vscale*(f-m->min);
}

					   
glMesh glmCreate(int nn, int nc, int dim, void *user_data,
		    glmLoopCallback loop)
{

  glMesh m=calloc(1,sizeof(struct glMeshStruct));

  m->cell_num=nc;
  m->node_num=nn;
  m->meshdim=dim;
  m->user_data=user_data;
  m->voffset=0;
  m->loop=loop;
  m->color=glmMapColor;
  m->matcolor=glmDefaultMaterialColor;
  m->nodefunc=NULL;
  m->cellflux=NULL;
  m->min=1.0;
  m->max=-1.0;
  m->first_time=1;
  return m;
}
double * glmGetNodeFunc(glMesh m)
{
  return m->nodefunc;
}
void *glmGetUserData(glMesh m)
{
  return m->user_data;
}

void glmSetVoffset(glMesh m,int voffset)
{
  m->voffset=voffset;
}
void glmDestroy(glMesh m)
{
  free(m);

}

void glmSetFunction(glMesh m, double *values, double min, double max)
{
  double *f=values+m->voffset;
  int i;
  m->nodefunc=values;
  if (min>max)
    {
      min=max=f[0];
      for (i=0;i<m->node_num;i++)
	{
	  if (f[i]>max) max=f[i];
	  else if  (f[i]<min) min=f[i];
	}
    }
  if (min==max) max=min+1.0;
  m->min=min;
  m->max=max;
}


void glmSetCellFlux(glMesh m, double *values, double min, double max)
{
  double *f=values+m->voffset;
  int i;
  double x;
  double *q;
  m->cellflux=values;
  if (max<min &&  m->meshdim==2) 
    { 
      min=1.0e30;
      max=-1.0e30;
      for (i=m->voffset;i<m->cell_num+m->voffset;i++)
	{
	  q=m->cellflux+m->voffset+2*(i-m->voffset);
	  x=sqrt(q[0]*q[0]+q[1]*q[1]);
	  if (x>max) max=x;
	  else if (x<min) min=x;
	}
      if (max==0.0) max=1.0;
    }
  else if (max<min &&  m->meshdim==3) 
    {
      min=1.0e30;
      max=-1.0e30;
      for (i=m->voffset;i<m->cell_num+m->voffset;i++)
	{
	  q=m->cellflux+m->voffset+3*(i-m->voffset);
	  x=sqrt(q[0]*q[0]+q[1]*q[1]+q[2]*q[2]);
	  if (x>max) max=x;
	  else if (x<min) min=x;
	}
      if (max==0.0) max=1.0;
    }
  if (min<0) min=0;
  m->min=min;
  m->max=max;
}


void glmSetColorCallback(glMesh m, glmColorCallback col)
{
  if (col!=NULL)
    m->color=col;
}

void glmSetMaterialColorCallback(glMesh m, glmMaterialColorCallback col)
{
  if (col!=NULL)
    m->matcolor=col;
}


/*---------------------------------------------------------------------*/
/*----------------------------------------------------------------*/




/*----------------------------------------------------------------*/



void glm_mesh_plain(glRenderer rnd, glMesh m)
{

  if   (m->meshdim==2)
    glm_2d_mesh_plain(rnd,m);
  else if   (m->meshdim==3)
    glm_3d_mesh_plain(rnd,m);
}

double glm_mesh_eval(glRenderer rnd, glMesh m)
{

  glrGetPoint(rnd,&m->pointCoord[0],&m->pointCoord[1],&m->pointCoord[2]);
  m->pointValue=0.0;
  switch(m->meshdim)
    {
    case 2:
      m->loop(m,m->user_data,glm_2d_simplex_eval);
      break;
    case 3:
      m->loop(m,m->user_data,glm_3d_simplex_eval);
      break;
    }
  return m->pointValue;
}

/*********************************************************/
/* Draw callbacks */
/* Check if idist, min,max have changed, do new calculations
   if necessary, notify the renderer.
   The renderer calls glCompile only when update is necessary.

   Get Christian's scale code.
 */

void glmDrawInfo(glRenderer rnd, glMesh m)
{
  /* this renders ally into the right renderer
     field */
  glRendererState state=glrGetRendererState(rnd);
  glWindow w=glrGetWindow(rnd);
  double h,f,idist,ypos;
  float rgb[3];
  int niso,i;
  glrDefaultInfoCallback(rnd,m);
  glm_set_colorscale(rnd,m);
  if (m->isoline_mode%2)
    idist=state->isoline_distance;
  else
    idist=0.1;

  niso=(int)(1.0/(idist))+1;
  h=0.7*idist;
  ypos=0.01;
  idist*=(m->max-m->min);

  for(f=m->min,i=0;f<=m->max+0.001*idist && i<=niso;f+=idist,i++)
    {
      glColor3fv(m->color(m,f,rgb));
      glRasterPos2d(0.8,ypos);
      glwPrintf(w,"%+.3g",f);
      ypos+=h;
    }
  ypos+=h;
  if (eval)
  {
    glColor3fv(m->color(m,m->pointValue,rgb));
    glRasterPos2d(0.8,ypos);
    glwPrintf(w,"f=%.3g",m->pointValue);
  }
}



/* main draw callback */
void glmDraw(glRenderer rnd, glMesh m)
{
  int wireframe;
  int flat;
  int xlev;
  int i;
  glRendererState state=glrGetRendererState(rnd);
  if (!initialized)
    {
      glm_showmat=0;
      initialized=1;
    }
  
  if (m->first_time)
    {
      glrRegisterKeyAction(rnd,GLW_C, (glrKeyAction)glm_flip_colorscale,"C: Flip color scale");
      glrRegisterKeyAction(rnd,GLW_e, (glrKeyAction)glm_toggle_eval,"e: Toggle function evaluation at point");
      glrRegisterKeyAction(rnd,GLW_M,
			   (glrKeyAction)glm_toggle_mesh_mat,
			   "M: Toggle mesh/material view");
      m->first_time=0;
    }

  state->fmin=m->min;
  state->fmax=m->max;
  state->spacedim=m->meshdim;
  
  for (i=0;i<=10;i++)
    {
      m->xtics[i]=state->min[0]+0.1*(state->max[0]-state->min[0])*(double)i;
      m->ytics[i]=state->min[1]+0.1*(state->max[1]-state->min[1])*(double)i;
      m->ztics[i]=state->min[2]+0.1*(state->max[2]-state->min[2])*(double)i;
      m->ftics[i]=state->fmin+0.1*(state->fmax-state->fmin)*(double)i;
    }
  glrSetAxisTics(rnd,'x',11,m->xtics);
  glrSetAxisTics(rnd,'y',11,m->ytics);
  glrSetAxisTics(rnd,'z',11,m->ztics);
  glrSetAxisTics(rnd,'f',11,m->ftics);

  m->glm_color_ctrl=state->ctrl_prm;
  m->glm_color_fac=state->ctrl_fac;


  if (m->nodefunc==NULL&&m->cellflux==NULL)
    {
      glm_mesh_plain(rnd,m);
      return;
    }

  glm_set_colorscale(rnd,m);
  glrGetWireframe(rnd,&wireframe);
  glrGetFlatshading(rnd, &flat);
  glrGetLevelSurface(rnd, &xlev);
  glrGetIsolineMode(rnd, &m->isoline_mode);
  glrGetLevel(rnd,&m->lev);

  m->cdelta=1.0/(m->max-m->min+1.0e-16);


  if (m->cellflux!=NULL)
    {
      if (m->meshdim==2)
	glm_2d_mesh_cellflux(rnd,m);
      else if (m->meshdim==3)
	glm_3d_mesh_cellflux(rnd,m);
      return;
    }


  if (m->meshdim==2)
    {

      if (m->isoline_mode%2)
	{
	  glm_2d_mesh_isolines(rnd,m);
	}
      else
	{
	  if (!wireframe)
	    {
	      if (flat)
		{
		  glm_2d_mesh_flat(rnd,m);
		}
	      else
		{
		  glm_2d_mesh_gouraud(rnd,m);
		}
	    }
	  else
	   glm_2d_mesh_wireframe(rnd,m);
	}
      if (eval)
	glm_mesh_eval(rnd,m);
    }
  else if   (m->meshdim==3)
    {
      if (xlev)
	glm_3d_mesh_levelsurface(rnd,m);
      else
	{
	  glm_3d_mesh_planesection(rnd,m);
	  if (eval)
	    glm_mesh_eval(rnd,m);
	}

   }
}





/*
 * $Log: glmesh.c,v $
 * Revision 2.42  2001/05/15 15:23:45  petzoldt
 * Martins Farbskala: Konflikte beseitigt
 *
 * Revision 2.41  2000/10/18 16:43:32  fuhrmann
 * scl_3d_gl works
 *
 * Revision 2.40  2000/10/18 08:41:50  fuhrmann
 *  scl_3d next step
 *
 * Revision 2.39  2000/10/17 16:46:36  fuhrmann
 * started scl 3d visualization
 *
 * Revision 2.38  2000/02/14 15:53:51  fuhrmann
 *  user material color callback
 *
 * Revision 2.37  1999/04/29 17:46:33  fuhrmann
 * various changes
 *
 * Revision 2.36  1999/04/22 19:28:20  fuhrmann
 * roundoff bug in draw info
 *
 * Revision 2.35  1999/04/13 11:43:31  fuhrmann
 * more thorough checks, k&r stuff deleted
 *
 * Revision 2.34  1999/04/12 14:08:23  fuhrmann
 * bug in draw info
 *
 * Revision 2.33  1999/04/09 12:20:29  fuhrmann
 * 3d cell flux running, will need raster point method
 *
 * Revision 2.32  1999/04/09 11:25:08  fuhrmann
 * first examples running with flux stuff
 *
 * Revision 2.31  1999/04/08 15:03:15  fuhrmann
 * glmesh split up
 *
 * Revision 2.30  1999/02/09 16:07:26  fuhrmann
 * F key for tics
 * Stupid bug in glmesh.c removed!!!
 *
 * Revision 2.29  1999/02/05 20:24:34  fuhrmann
 * tics introduced
 *
 * Revision 2.28  1999/02/02 14:37:43  fuhrmann
 * mode switch handling in glrnd improved
 * bug in glmesh removed
 *
 * Revision 2.27  1999/02/01 18:36:34  fuhrmann
 * minor changes
 *
 * Revision 2.26  1999/01/26 17:14:49  fuhrmann
 * time depenedent demo mode in glview
 * color interpolation for 3D sections in glmesh
 *
 * Revision 2.25  1998/08/10  19:52:34  fuhrmann
 * isoline distance, vscale change in glrnd
 *
 * Revision 2.24  1998/08/05  17:50:49  fuhrmann
 * m key ->M key
 *
 * Revision 2.23  1998/07/29  16:01:24  fuhrmann
 * material stuff
 *
 * Revision 2.22  1998/07/27  19:10:03  fuhrmann
 * material stuff in glmesh
 *
 * Revision 2.21  1998/07/27  18:04:28  fuhrmann
 * Handling 3 planes, function evaluation, glrGetPoint
 *
 * Revision 2.20  1998/07/06  10:45:10  fuhrmann
 * Increased compiler warning level for all PDELIB-1.7 modules,
 * removed all bugs, some warnings remain (rcsids, ld_init, unused parameters)
 *
 * Revision 2.19  1998/03/06  16:45:29  fuhrmann
 * tiff->ppm
 * print command
 * video recording
 *
 * Revision 2.18  1998/03/03  10:07:29  fuhrmann
 * color stuff changed a bit...
 *
 * Revision 2.17  1997/11/04  15:53:29  fuhrmann
 * color scales ; crash with glrnd
 *
 * Revision 2.16  1997/10/28  09:37:17  fuhrmann
 * color scale cycling
 *
 * Revision 2.15  1997/10/08  16:29:04  fuhrmann
 * glmDrawInfo non-
 *
 * Revision 2.14  1997/09/26  10:03:30  fuhrmann
 * glmDraw is not allowed to overwrite info callback because it
 * cannot know if it is called diretly by the renderer with the
 * mesh as data parameter.
 *
 * Revision 2.13  1997/09/22  17:37:27  fuhrmann
 * One info callback
 *
 * Revision 2.12  1997/09/22  17:05:45  fuhrmann
 * beta code for isolevel printing
 *
 * Revision 2.11  1997/05/15  10:58:07  fuhrmann
 * minor bug in declarations
 *
 * Revision 2.10  1997/04/18  10:26:34  fuhrmann
 * eps =0.0 in intersection  calculations
 *
 * Revision 2.9  1997/04/15  08:09:44  fuhrmann
 * new type of color callback
 *
 * Revision 2.8  1997/03/20  18:18:07  fuhrmann
 * bounding box + function min/max -> state
 *
 * Revision 2.7  1997/03/19  11:05:49  fuhrmann
 * transparency (beta code)
 *
 * Revision 2.6  1997/02/28  14:12:34  fuhrmann
 * flux viewer (alpha state)
 *
 * Revision 2.5  1997/02/24  19:44:00  fuhrmann
 * Hey Hey - Isolines work now !!!
 *
 * Revision 2.4  1997/02/05  16:59:27  fuhrmann
 * introduced  -DUSE_PRAGMA_INLINE to wrap AXP inline declarations
 *
 * Revision 2.3  1996/09/06  18:17:07  fuhrmann
 * added comments, min/max handling
 * changed code for 2d flat shading
 *
 * Revision 2.2  1996/09/04  19:51:21  fuhrmann
 * - improved code for 3D sections (isolines will be possible etc.)
 * - isolines for 2d work
 *
 * Revision 2.1  1996/09/03  09:07:47  fuhrmann
 * vscale /-vscale stuff
 *
 * Revision 2.0  1996/02/15  19:57:10  fuhrmann
 * First meta-stable distribution
 *
 * Revision 1.1  1995/10/20  15:42:03  fuhrmann
 * Initial revision
 *
 */

/* TODO:

rendering von Cell/Vertexfkt.
rendering von vektorf.
volume rendering

* Welche Gitterfunkt. werden wie dargestellt ?
  glmSetv(m,GLM_COLOR_FUNCT,val);
  glmSetv(m,GLM_HEIGHT_FUNCT,val);
  glmSetf(m,GLM_COLOR_FUNCT,f(x,y,z));
  glmSetf(m,GLM_HEIGHT_FUNCT,f(x,y,z));
  glmSetf(m,GLM_SECTION_FUNCT,(f(x,y,z));( section mit f(x,y,z)=0 )
  glmSetv(m,GLM_SECTION_FUNCT,(f(x,y,z));
  glmSetv(m,GLM_LEVEL_FUNCT,(f(x,y,z));


* Koordinatenuebergabe ???
* Gouraud - shading in sections, level surfaces
* Wunsch von Kunibert


*/
