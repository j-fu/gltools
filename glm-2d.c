static char *rcsid="$Id: glm-2d.c,v 2.7 2000/10/18 08:41:49 fuhrmann Exp $";
#include <GL/gl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "glmesh.h"
#include "glm-internal.h"

void glm_2d_simplex_eval(glMesh m, int num, int mat, double *nodefunc, int *nodes,  double **coord)
{

  register int n,d;
  double barycentricCoord[3],v[3][3],w[3][3],det,invdet;
  int   i,j,k,e;
  if (nodefunc==0) nodefunc=m->nodefunc;

  for(i=1; i<=2; i++) for(j=0; j<2; j++)
    v[i][j]= coord[i][j]-coord[0][j];

  det         =   (v[1][0]*v[2][1] - v[1][1]*v[2][0]);


  if (det==0) return;
  invdet = 1/det;
  w[1][0]=  v[2][1]*invdet;
  w[1][1]= -v[2][0]*invdet;

  w[2][0]= -v[1][1]*invdet;
  w[2][1]=  v[1][0]*invdet;

  for(j=0; j<2; j++)
    {
      w[0][j]=0.0;
      for(i=1; i<=2; i++) w[0][j] -= w[i][j];
    }

  barycentricCoord[0]= 1.0;
  for(n=1; n<3; n++)
    {
      barycentricCoord[n]= 0.0;
      for(d=0; d<2; d++)
	{
	  barycentricCoord[n]+=
	    w[n][d]*(m->pointCoord[d] - coord[0][d]);
	}
      barycentricCoord[0]-= barycentricCoord[n];
    }

  if (barycentricCoord[0]<0.0) return;
  if (barycentricCoord[1]<0.0) return;
  if (barycentricCoord[2]<0.0) return;

  m->pointValue=
    barycentricCoord[0]*nodefunc[nodes[0]]+
    barycentricCoord[1]*nodefunc[nodes[1]]+
    barycentricCoord[2]*nodefunc[nodes[2]];
}



/* plot isolines of a function on a triangle */
void  glm_2d_simplex_isolines(glMesh m, int num, int mat, double *nodefunc, int *nodes, double **coord)
{
  register int  i, i0,i1,i2;
  double alpha;
  double x0,y0,x1,y1,z0,z1;
  double df20,df21,df10,dx20,dx21,dx10,dy21,dy20,dy10,dz21,dz20,dz10;
  double f[5];
  int l0,l1;
  double xlev;
  float rgb[3];
  double f_scaled[5];
  if (nodefunc==0) nodefunc=m->nodefunc;

  f[0]=nodefunc[nodes[0]];
  f[1]=nodefunc[nodes[1]];
  f[2]=nodefunc[nodes[2]];

  f_scaled[0]=glm_scale_func(m,f[0]);
  f_scaled[1]=glm_scale_func(m,f[1]);
  f_scaled[2]=glm_scale_func(m,f[2]);


  if (f[0] <=f[1]) {i0=0; i1=1 ;} else {i0=1; i1=0;};
  if (f[i1]<=f[2]) {i2=2       ;} else {i2=i1;i1=2;};
  if (f[i0] >f[i1]){i=i0; i0=i1; i1=i ;};

  if (f[i2]!=f[i0]) df20=1.0/(f[i2]-f[i0]); else df20=0.0;
  if (f[i1]!=f[i0]) df10=1.0/(f[i1]-f[i0]); else df10=0.0;
  if (f[i2]!=f[i1]) df21=1.0/(f[i2]-f[i1]); else df21=0.0;



  dx20=coord[i2][0]-coord[i0][0];
  dx10=coord[i1][0]-coord[i0][0];
  dx21=coord[i2][0]-coord[i1][0];

  dy20=coord[i2][1]-coord[i0][1];
  dy10=coord[i1][1]-coord[i0][1];
  dy21=coord[i2][1]-coord[i1][1];

  /*  dz20=coord[i2][2]-coord[i0][2];
      dz10=coord[i1][2]-coord[i0][2];
      dz21=coord[i2][2]-coord[i1][2];
  */
  dz20=f_scaled[i2]-f_scaled[i0];
  dz10=f_scaled[i1]-f_scaled[i0];
  dz21=f_scaled[i2]-f_scaled[i1];

  l0=(int)((f[i0])/m->isoline_distance)-1;
  l1=(int)((f[i2])/m->isoline_distance)+1;

  for( i=l0 ; i< l1; i++ )
    {
      xlev=((double)i)*m->isoline_distance;

      if( (f[i0]<=xlev) && (xlev<f[i2]) )
	{
	  alpha=(xlev-f[i0])*df20;
	  x0=coord[i0][0]+alpha*dx20;
	  y0=coord[i0][1]+alpha*dy20;
	  z0=f_scaled[i0]+alpha*dz20;

	  if (xlev<f[i1])
	    {
	      alpha=(xlev-f[i0])*df10;
	      x1=coord[i0][0]+alpha*dx10;
	      y1=coord[i0][1]+alpha*dy10;
	      z1=f_scaled[i0]+alpha*dz10;
	    }
	  else
	    {
	      alpha=(xlev-f[i1])*df21;
	      x1=coord[i1][0]+alpha*dx21;
	      y1=coord[i1][1]+alpha*dy21;
	      z1=f_scaled[i1]+alpha*dz21;
	    }

	  glColor3fv(m->color(m,xlev,rgb));
	  glVertex3d(x0,y0,z0);
	  glVertex3d(x1,y1,z1);
	}
    }

}


/* calculate triangle normal and add them to node normals */
void glm_2d_simplex_normals(glMesh m, int num, int mat, double *nodefunc, int *nodes, double **coord)
{
  double ax,ay,az,bx,by,bz,xnx,xny,xnz;
  int n1,n2,n3;
  n1=nodes[0];
  n2=nodes[1];
  n3=nodes[2];
  if (nodefunc==NULL) nodefunc=m->nodefunc;
  ax=coord[0][0]-coord[1][0];
  ay=coord[0][1]-coord[1][1];
  az=    glm_scale_func(m,nodefunc[n1])-glm_scale_func(m,nodefunc[n2]);
  
  bx=coord[0][0]-coord[2][0];
  by=coord[0][1]-coord[2][1];
  bz=    glm_scale_func(m,nodefunc[n1])-glm_scale_func(m,nodefunc[n3]);

  xnx=ay*bz-by*az;
  xny=az*bx-bz*ax;
  xnz=ax*by-bx*ay;
  m->n_x[n1]+=xnx;
  m->n_x[n2]+=xnx;
  m->n_x[n3]+=xnx;

  m->n_y[n1]+=xny;
  m->n_y[n2]+=xny;
  m->n_y[n3]+=xny;

  m->n_z[n1]+=xnz;
  m->n_z[n2]+=xnz;
  m->n_z[n3]+=xnz;

}

/* draw flat shaded triangle in 2d */
void glm_2d_simplex_flat(glMesh m,int num, int mat, double *nodefunc, int *nodes, double **coord)
{
  float rgb[3];
  double ax,ay,az,bx,by,bz;
  double n_x,n_y,n_z,f_scaled[5];
  int n1,n2,n3;
  n1=nodes[0];
  n2=nodes[1];
  n3=nodes[2];
  if (nodefunc==0) nodefunc=m->nodefunc;

  f_scaled[0]=glm_scale_func(m,nodefunc[nodes[0]]);
  f_scaled[1]=glm_scale_func(m,nodefunc[nodes[1]]);
  f_scaled[2]=glm_scale_func(m,nodefunc[nodes[2]]);
  
  ax=coord[0][0]-coord[1][0];
  ay=coord[0][1]-coord[1][1];
  az=    f_scaled[0]-f_scaled[1];

  bx=coord[0][0]-coord[2][0];
  by=coord[0][1]-coord[2][1];
  bz=    f_scaled[0]-f_scaled[2];

  n_x=ay*bz-by*az;
  n_y=az*bx-bz*ax;
  n_z=ax*by-bx*ay;

  glNormal3f(n_x,n_y,n_z);

  glColor3fv(m->color(m,nodefunc[n1],rgb));
  glVertex3f(coord[0][0],coord[0][1],f_scaled[0]);
  glColor3fv(m->color(m,nodefunc[n2],rgb));
  glVertex3f(coord[1][0],coord[1][1],f_scaled[1]);
  glColor3fv(m->color(m,nodefunc[n3],rgb));
  glVertex3f(coord[2][0],coord[2][1],f_scaled[2]);

}

/* draw Gouraud shaded triangle in 2d with normals*/
 void glm_2d_simplex_gouraud(glMesh m, int num , int mat, double *nodefunc, int *nodes, double **coord)
{
  register int in;
  if (nodefunc==0) nodefunc=m->nodefunc;

  in=nodes[0];
  glColor3f(0,1,0);
  glColor3fv(m->rgb[in]);
  glNormal3f(m->n_x[in],m->n_y[in],m->n_z[in]);
  glVertex3f(coord[0][0],coord[0][1],glm_scale_func(m,nodefunc[in]));

  in=nodes[1];
  glColor3fv(m->rgb[in]);
  glNormal3f(m->n_x[in],m->n_y[in],m->n_z[in]);
  glVertex3f(coord[1][0],coord[1][1],glm_scale_func(m,nodefunc[in]));

  in=nodes[2];
  glColor3fv(m->rgb[in]);
  glNormal3f(m->n_x[in],m->n_y[in],m->n_z[in]);
  glVertex3f(coord[2][0],coord[2][1],glm_scale_func(m,nodefunc[in]));

}


/* draw flat wireframe triangle in 2d */
void glm_2d_simplex_wireframe(glMesh m, int num, int mat,  double *nodefunc, int *nodes, double **coord)
{
  if (nodefunc==0) nodefunc=m->nodefunc;
  glVertex3f(coord[0][0],coord[0][1],glm_scale_func(m,nodefunc[nodes[0]]));
  glVertex3f(coord[1][0],coord[1][1],glm_scale_func(m,nodefunc[nodes[1]]));
  glVertex3f(coord[2][0],coord[2][1],glm_scale_func(m,nodefunc[nodes[2]]));
}

/* draw plain simplex in 2d */
void glm_2d_simplex_plain(glMesh m, int num, int mat, double *nodefunc, int *nodes, double **coord)
{
  float rgb[4];
  glColor3fv(m->matcolor(m,mat,rgb));
  glVertex2dv(coord[0]);
  glVertex2dv(coord[1]);
  glVertex2dv(coord[2]);
}
/*----------------------------------------------------------------------*/
/* mesh loops */
/* draw 2d function with isolines */
void glm_2d_mesh_isolines(glRenderer rnd, glMesh m)
{
  register int l;
  double dummy;
  int nn=m->node_num;
  glRendererState state=glrGetRendererState(rnd);
  glrGetPlane(rnd,&dummy,&dummy,&dummy,&m->plane);
  glrGetVScale(rnd, &m->vscale);
  m->vscale= m->vscale;
  m->plane= -m->plane;
  m->isoline_distance=state->isoline_distance*(m->max-m->min);
  m->vscale=m->vscale/(m->max-m->min);

  glLineWidth(2.0);
  glBegin(GL_LINES);
  m->loop(m,m->user_data,glm_2d_simplex_isolines);
  glEnd();

  if (glm_showmat)
    {
      glBegin(GL_TRIANGLES);
      m->loop(m,m->user_data,glm_2d_simplex_plain);
      glEnd();
    }
    {
      glBegin(GL_LINES);
      m->loop(m,m->user_data,glm_2d_simplex_isolines);
      glEnd();

    }
  glLineWidth(1.0);
}


/* draw 2d function flat shaded (no additional memory!) */
void glm_2d_mesh_flat(glRenderer rnd, glMesh m)
{
  double dummy;
  glrGetPlane(rnd,&dummy,&dummy,&dummy,&m->plane);
  glrGetVScale(rnd, &m->vscale);
  m->vscale= m->vscale;
  m->plane= -m->plane;
  m->vscale=m->vscale/(m->max-m->min);
  if (glm_showmat)
    {
      glBegin(GL_TRIANGLES);
      m->loop(m,m->user_data,glm_2d_simplex_plain);
      glEnd();
    }
    {
      glBegin(GL_TRIANGLES);
      m->loop(m,m->user_data,glm_2d_simplex_flat);
      glEnd();
    }
}


/* draw 2d function gouraud shaded (lots of additional memory!) */
void glm_2d_mesh_gouraud(glRenderer rnd, glMesh m)
{
  register int l;
  double dummy;
  int nn=m->node_num;

  glrGetPlane(rnd,&dummy,&dummy,&dummy,&m->plane);
  glrGetVScale(rnd, &m->vscale);
  m->vscale= m->vscale;
  m->plane= -m->plane;
  m->vscale=m->vscale/(m->max-m->min);


  m->rgb=malloc((m->node_num+m->voffset)*sizeof(floatrgb));
  m->n_x=calloc((m->node_num+m->voffset),sizeof(float));
  m->n_y=calloc((m->node_num+m->voffset),sizeof(float));
  m->n_z=calloc((m->node_num+m->voffset),sizeof(float));


  for(l=m->voffset;l<nn+m->voffset;l++)
    {
      m->color(m,m->nodefunc[l],m->rgb[l]);
    }

  m->loop(m,m->user_data,glm_2d_simplex_normals);

  if (glm_showmat)
    {
      glBegin(GL_TRIANGLES);
      m->loop(m,m->user_data,glm_2d_simplex_plain);
      glEnd();
    }
    {
      glBegin(GL_TRIANGLES);
      m->loop(m,m->user_data,glm_2d_simplex_gouraud);
      glEnd();

    }

  free(m->rgb);
  free(m->n_x);
  free(m->n_y);
  free(m->n_z);

}


/* Draw 2d function in wireframe mode */
void glm_2d_mesh_wireframe(glRenderer rnd, glMesh m)
{
  double dummy;
  int nn=m->node_num;
  register int l;
  glrGetPlane(rnd,&dummy,&dummy,&dummy,&m->plane);
  glrGetVScale(rnd, &m->vscale);
  m->vscale= m->vscale;
  m->plane= -m->plane;
  m->vscale=m->vscale/(m->max-m->min);


  if (glm_showmat)
    {
      glBegin(GL_TRIANGLES);
      m->loop(m,m->user_data,glm_2d_simplex_plain);
      glEnd();
    }
    {
      glBegin(GL_TRIANGLES);
      m->loop(m,m->user_data,glm_2d_simplex_wireframe);
      glEnd();

    }
}

void glm_2d_mesh_plain(glRenderer rnd, glMesh m)
{

  glBegin(GL_LINES);
  m->loop(m,m->user_data,glm_2d_simplex_plain);
  glEnd();
}

void glm_2d_simplex_cellflux(glMesh m, int num, int mat, double *nodefunc, int *nodes, double **coord)
{
  double x,y;
  double *q=m->cellflux+2*(num-m->voffset);
  double ql=sqrt(q[0]*q[0]+q[1]*q[1]);
  float rgb[4];
  x=(coord[0][0]+coord[1][0]+coord[2][0])*0.333333333333;
  y=(coord[0][1]+coord[1][1]+coord[2][1])*0.333333333333;

  if (ql>0.0)
    {
      glColor3fv(m->color(m,ql,rgb));
      glVertex3d(x,y,0);
      glVertex3d(x+0.04*q[0]/ql,y+0.04*q[1]/ql,0);
    }
  else
    {
      glColor3fv(m->color(m,ql,rgb));
      glVertex3d(x,y,0);
      glVertex3d(x,y,0);
    }
}

void glm_2d_mesh_cellflux(glRenderer rnd, glMesh m)
{
  glrGetVScale(rnd, &m->vscale);
  m->vscale/=25*m->max;
  glBegin(GL_LINES);
  m->loop(m,m->user_data,glm_2d_simplex_cellflux);
  glEnd();
}



/*
 * $Log: glm-2d.c,v $
 * Revision 2.7  2000/10/18 08:41:49  fuhrmann
 *  scl_3d next step
 *
 * Revision 2.6  2000/10/17 16:46:36  fuhrmann
 * started scl 3d visualization
 *
 * Revision 2.5  2000/02/14 15:53:51  fuhrmann
 *  user material color callback
 *
 * Revision 2.4  1999/04/13 11:43:31  fuhrmann
 * more thorough checks, k&r stuff deleted
 *
 * Revision 2.3  1999/04/09 12:20:28  fuhrmann
 * 3d cell flux running, will need raster point method
 *
 * Revision 2.2  1999/04/09 11:25:08  fuhrmann
 * first examples running with flux stuff
 *
 * Revision 2.1  1999/04/08 15:03:14  fuhrmann
 * glmesh split up
 *
 */
