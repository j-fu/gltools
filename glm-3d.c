static char *rcsid="$Id: glm-3d.c,v 2.13 2001/05/21 14:27:19 fuhrmann Exp $";
#include <GL/gl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "glrnd.h"
#include "glmesh.h"
#include "glprimitives.h"
#include "glm-internal.h"

/* courtesy Hartmut Langmach */

void glm_3d_simplex_eval(glMesh m, int num, int mat, double *nodefunc, int *nodes, double **coord)
{


  register int n,d;
  double barycentricCoord[4],v[4][4],w[4][4],det,invdet;
  int   i,j,k,e;

  if (nodefunc==NULL) nodefunc=m->nodefunc;
  
  for(i=1; i<=3; i++) for(j=0; j<3; j++)
    v[i][j]= coord[i][j]-coord[0][j];


  det =   v[3][0] * (v[1][1]*v[2][2] - v[1][2]*v[2][1])
         +v[3][1] * (v[1][2]*v[2][0] - v[1][0]*v[2][2])
         +v[3][2] * (v[1][0]*v[2][1] - v[1][1]*v[2][0]);

  if (det==0.0) return;
  invdet = 1.0/det;
  w[1][0]=  (v[2][1]*v[3][2] - v[2][2]*v[3][1])*invdet;
  w[1][1]=  (v[2][2]*v[3][0] - v[2][0]*v[3][2])*invdet;
  w[1][2]=  (v[2][0]*v[3][1] - v[2][1]*v[3][0])*invdet;

  w[2][0]=  (v[3][1]*v[1][2] - v[3][2]*v[1][1])*invdet;
  w[2][1]=  (v[3][2]*v[1][0] - v[3][0]*v[1][2])*invdet;
  w[2][2]=  (v[3][0]*v[1][1] - v[3][1]*v[1][0])*invdet;

  w[3][0]=  (v[1][1]*v[2][2] - v[1][2]*v[2][1])*invdet;
  w[3][1]=  (v[1][2]*v[2][0] - v[1][0]*v[2][2])*invdet;
  w[3][2]=  (v[1][0]*v[2][1] - v[1][1]*v[2][0])*invdet;


  for(j=0; j<3; j++)
    {
      w[0][j]=0.0;
      for(i=1; i<=2; i++) w[0][j] -= w[i][j];
    }
  barycentricCoord[0]= 1.0;
  for(n=1; n<4; n++)
    {
      barycentricCoord[n]= 0.0;
      for(d=0; d<3; d++) {
	barycentricCoord[n]+=
	  w[n][d]*(m->pointCoord[d] - coord[0][d]);
      }
      barycentricCoord[0]-= barycentricCoord[n];
    }

  if (barycentricCoord[0]<0.0) return;
  if (barycentricCoord[1]<0.0) return;
  if (barycentricCoord[2]<0.0) return;
  if (barycentricCoord[3]<0.0) return;

  m->pointValue=
    barycentricCoord[0]*nodefunc[nodes[0]]+
    barycentricCoord[1]*nodefunc[nodes[1]]+
    barycentricCoord[2]*nodefunc[nodes[2]]+
    barycentricCoord[3]*nodefunc[nodes[3]];
}




/* plot isolines of a function on a triangle */
void  glm_3d_simplex_isect_isolines(glMesh m,
			      double *xs,
			      double *ys,
			      double *zs,
			      double *f
			      )
{
  register int  i, i0,i1,i2;
  double alpha;
  double x0,y0,x1,y1,z0,z1;
  double df20,df21,df10,dx20,dx21,dx10,dy21,dy20,dy10,dz21,dz20,dz10;
  int l0,l1;
  double xlev;
  float rgb[3];

  if (f[0] <=f[1]) {i0=0; i1=1 ;} else {i0=1; i1=0;};
  if (f[i1]<=f[2]) {i2=2       ;} else {i2=i1;i1=2;};
  if (f[i0] >f[i1]){i=i0; i0=i1; i1=i ;};

  if (f[i2]!=f[i0]) df20=1.0/(f[i2]-f[i0]); else df20=0.0;
  if (f[i1]!=f[i0]) df10=1.0/(f[i1]-f[i0]); else df10=0.0;
  if (f[i2]!=f[i1]) df21=1.0/(f[i2]-f[i1]); else df21=0.0;


  dx20=xs[i2]-xs[i0];
  dx10=xs[i1]-xs[i0];
  dx21=xs[i2]-xs[i1];

  dy20=ys[i2]-ys[i0];
  dy10=ys[i1]-ys[i0];
  dy21=ys[i2]-ys[i1];

  dz20=zs[i2]-zs[i0];
  dz10=zs[i1]-zs[i0];
  dz21=zs[i2]-zs[i1];

  /*
  dz20=v[n2]-v[n0];
  dz10=v[n1]-v[n0];
  dz21=v[n2]-v[n1];
  */
  l0=(int)((f[i0])/m->isoline_distance)-1;
  l1=(int)((f[i2])/m->isoline_distance)+1;

  for( i=l0 ; i< l1; i++ )
    {
      xlev=((double)i)*m->isoline_distance;

      if( (f[i0]<=xlev) && (xlev<f[i2]) )
	{
	  alpha=(xlev-f[i0])*df20;
	  x0=xs[i0]+alpha*dx20;
	  y0=ys[i0]+alpha*dy20;
	  z0=zs[i0]+alpha*dz20;

	  if (xlev<f[i1])
	    {
	      alpha=(xlev-f[i0])*df10;
	      x1=xs[i0]+alpha*dx10;
	      y1=ys[i0]+alpha*dy10;
	      z1=zs[i0]+alpha*dz10;
	    }
	  else
	    {
	      alpha=(xlev-f[i1])*df21;
	      x1=xs[i1]+alpha*dx21;
	      y1=ys[i1]+alpha*dy21;
	      z1=zs[i1]+alpha*dz21;
	    }
	  glColor3fv(m->color(m,xlev,rgb));
	  glVertex3d(x0,y0,z0);
	  glVertex3d(x1,y1,z1);
	}

    }
}

#define v1 nodefunc[nodes[0]]
#define v2 nodefunc[nodes[1]]
#define v3 nodefunc[nodes[2]]
#define v4 nodefunc[nodes[3]]

#define plane_equation(x,y,z) (m->pa*(x)+m->pb*(y)+m->pc*(z)+m->pd)
#define x1   coord[0][0]
#define y1   coord[0][1]
#define z1   coord[0][2]

#define x2   coord[1][0]
#define y2   coord[1][1]
#define z2   coord[1][2]

#define x3   coord[2][0]
#define y3   coord[2][1]
#define z3   coord[2][2]

#define x4   coord[3][0]
#define y4   coord[3][1]
#define z4   coord[3][2]




/* draw one triangle in wireframe mode */
void glm_3d_simplex_isect_plain(double xs[], double  ys[],double zs[])
{
  glVertex3f(xs[0],ys[0],zs[0]);
  glVertex3f(xs[1],ys[1],zs[1]);
  glVertex3f(xs[2],ys[2],zs[2]);
}

void glm_3d_simplex_isect_xplain(double xs[], double  ys[],double zs[])
{
  glVertex3f(xs[0],ys[0],zs[0]);
  glVertex3f(xs[1],ys[1],zs[1]);
  glVertex3f(xs[1],ys[1],zs[1]);
  glVertex3f(xs[2],ys[2],zs[2]);
  glVertex3f(xs[2],ys[2],zs[2]);
  glVertex3f(xs[0],ys[0],zs[0]);
}


/* draw cell flux  */
void glm_3d_simplex_isect_cellflux(glMesh m, int num, double xs[], double  ys[],double zs[])
{
  double x,y,z;
  double *q=m->cellflux+m->voffset+3*(num-1);
  double ql=sqrt(q[0]*q[0]+q[1]*q[1]+q[2]*q[2]);
  float rgb[4];
  double scale=5;
  x=0.3333333*(xs[0]+xs[1]+xs[2]);
  y=0.3333333*(ys[0]+ys[1]+ys[2]);
  z=0.3333333*(zs[0]+zs[1]+zs[2]);
  if (ql>0.0)
    {
      glColor3fv(m->color(m,ql,rgb));
      glVertex3d(x,y,z);
      glVertex3d(x+scale*q[0],y+scale*q[1],z+scale*q[2]);
    }
  else
    {
      glColor3fv(m->color(m,ql,rgb));
      glVertex3d(x,y,z);
      glVertex3d(x,y,z);
    }
}



/* draw one triangle in full mode  */
void glm_3d_simplex_isect_shaded(double xs[], double  ys[],double zs[], float **rgb)
{
  double ax,ay,az,bx,by,bz,nx,ny,nz;

  ax=xs[0]-xs[1];
  ay=ys[0]-ys[1];
  az=zs[0]-zs[1];

  bx=xs[0]-xs[2];
  by=ys[0]-ys[2];
  bz=zs[0]-zs[2];

  nx=ay*bz-by*az;
  ny=az*bx-bz*ax;
  nz=ax*by-bx*ay;

  glNormal3f(nx,ny,nz);
  glColor3fv(rgb[0]);
  glVertex3f(xs[0],ys[0],zs[0]);
  glColor3fv(rgb[1]);
  glVertex3f(xs[1],ys[1],zs[1]);
  glColor3fv(rgb[2]);
  glVertex3f(xs[2],ys[2],zs[2]);
}

void glm_3d_simplex_isect_flat(double xs[], double  ys[],double zs[])
{
  double ax,ay,az,bx,by,bz,nx,ny,nz;

  ax=xs[0]-xs[1];
  ay=ys[0]-ys[1];
  az=zs[0]-zs[1];

  bx=xs[0]-xs[2];
  by=ys[0]-ys[2];
  bz=zs[0]-zs[2];

  nx=ay*bz-by*az;
  ny=az*bx-bz*ax;
  nz=ax*by-bx*ay;

  glNormal3f(nx,ny,nz);
  glVertex3f(xs[0],ys[0],zs[0]);
  glVertex3f(xs[1],ys[1],zs[1]);
  glVertex3f(xs[2],ys[2],zs[2]);
}


/* draw one one or two triangles of plane intersection of thetrahedron */
/* this should be split!!! */
void glm_3d_simplex_isect_draw(glMesh m,
			       int num,
			       int is, 
			       double xs[], 
			       double ys[],
			       double zs[],
			       double vs[])
{
  float rgbx[4][3];
  float *rgb[4];
  int i;
  rgb[0]=&rgbx[0][0];
  rgb[1]=&rgbx[1][0];
  rgb[2]=&rgbx[2][0];
  rgb[3]=&rgbx[3][0];

  if (m->cellflux)
    {
      if (is>=3)
	glm_3d_simplex_isect_cellflux(m,num,xs,ys,zs);
      if (is==4)
	glm_3d_simplex_isect_cellflux(m,num,xs+1,ys+1,zs+1);
     }
  else if (m->isoline_mode%2 && vs !=NULL)
    {
      if (is>=3)
	glm_3d_simplex_isect_isolines(m,xs,ys,zs,vs);
      if (is==4)
	glm_3d_simplex_isect_isolines(m,xs+1,ys+1,zs+1,vs+1);
    }
  else if (m->wireframe)
    {
      if (is>=3)
	glm_3d_simplex_isect_plain(xs,ys,zs);
      if (is==4)
	glm_3d_simplex_isect_plain(xs+1,ys+1,zs+1);
    }
  else
    {
      if (vs!=NULL)
	{
	  for (i=0;i<is;i++)
	    {
	      m->color(m,vs[i],rgb[i]);
	    }
	  if (is>=3)
	    glm_3d_simplex_isect_shaded(xs,ys,zs,rgb);
	  if (is==4)
	    glm_3d_simplex_isect_shaded(xs+1,ys+1,zs+1,rgb+1);
	}
      else /* level surface */
	{
	  if (is>=3)
	    glm_3d_simplex_isect_flat(xs,ys,zs);
	  if (is==4)
	    glm_3d_simplex_isect_flat(xs+1,ys+1,zs+1);
	}
      
    }
  
}

/* calculate intersection between plane and tetrahedron if
  given  distances p of the nodes from that plane */
int glm_3d_simplex_isect_calc(glMesh m, double *nodefunc, int *nodes,  double **coord, double *p,
				 double *xs,double *ys,double *zs,double *vs)
{
  int is;
  double t;
  if ( (p[0]<0.0)&&(p[1]<0.0)&&(p[2]<0.0)&&(p[3]<0.0)) return 0;
  if ( (p[0]>0.0)&&(p[1]>0.0)&&(p[2]>0.0)&&(p[3]>0.0)) return 0;
  is=0;
  if (p[0]*p[1]<m->eps)
    {
      t= p[0]/(p[0]-p[1]);
      xs[is]=x1+t*(x2-x1);
      ys[is]=y1+t*(y2-y1);
      zs[is]=z1+t*(z2-z1);
      if (vs) vs[is]=v1+t*(v2-v1);
      is++;
    }
  if (p[0]*p[2]<m->eps)
    {
      t= p[0]/(p[0]-p[2]);
      xs[is]=x1+t*(x3-x1);
      ys[is]=y1+t*(y3-y1);
      zs[is]=z1+t*(z3-z1);
      if (vs)  vs[is]=v1+t*(v3-v1);
      is++;
    }
  if (p[0]*p[3]<m->eps)
    {
      t= p[0]/(p[0]-p[3]);
      xs[is]=x1+t*(x4-x1);
      ys[is]=y1+t*(y4-y1);
      zs[is]=z1+t*(z4-z1);
      if (vs) vs[is]=v1+t*(v4-v1);
      is++;
    }

  if (p[1]*p[2]<m->eps)
    {
      t= p[1]/(p[1]-p[2]);
      xs[is]=x2+t*(x3-x2);
      ys[is]=y2+t*(y3-y2);
      zs[is]=z2+t*(z3-z2);
      if (vs) vs[is]=v2+t*(v3-v2);
      is++;
    }
  if (p[1]*p[3]<m->eps)
    {
      t= p[1]/(p[1]-p[3]);
      xs[is]=x2+t*(x4-x2);
      ys[is]=y2+t*(y4-y2);
      zs[is]=z2+t*(z4-z2);
      if (vs) vs[is]=v2+t*(v4-v2);
      is++;
    }
  if (p[2]*p[3]<m->eps)
    {
      t= p[2]/(p[2]-p[3]);
      xs[is]=x3+t*(x4-x3);
      ys[is]=y3+t*(y4-y3);
      zs[is]=z3+t*(z4-z3);
      if (vs) vs[is]=v3+t*(v4-v3);
      is++;
    }
  return is;
}

/* draw level surface of level lev */
 void glm_3d_simplex_levelsurface(glMesh m,int num, int mat, double *nodefunc,int *nodes, double **coord)
{
  int is;
  double xs[5],ys[5],zs[5],p[5];
  if (nodefunc==NULL) nodefunc=m->nodefunc;
  /* plane distances are given by function values */
  p[0]=v1-m->lev;
  p[1]=v2-m->lev;
  p[2]=v3-m->lev;
  p[3]=v4-m->lev;
  is=glm_3d_simplex_isect_calc(m,nodefunc,nodes,coord,p,xs,ys,zs,NULL);
  if (is<3) {return;}
  glm_3d_simplex_isect_draw(m,num,is,xs,ys,zs,NULL);
}


/* draw intersection with plane */
void glm_3d_simplex_planesection(glMesh m, int num, int mat, double *nodefunc, int *nodes, double **coord)
{
  double vs[5];
  int is;
  float rgb[5];
  double xs[5],ys[5],zs[5],p[5];
  register int ii;
  if (nodefunc==NULL) nodefunc=m->nodefunc;
  /* plane distances are given by plane equation */
  p[0]=plane_equation(x1,y1,z1);
  p[1]=plane_equation(x2,y2,z2);
  p[2]=plane_equation(x3,y3,z3);
  p[3]=plane_equation(x4,y4,z4);
  is=glm_3d_simplex_isect_calc(m,nodefunc, nodes,coord,p,xs,ys,zs,vs);
  if (is<3) {return;}

 if (glm_showmat)
   {
     glColor3fv(m->matcolor(m,mat,rgb));
     glm_3d_simplex_isect_draw(m,num,is,xs,ys,zs,NULL);
   }
 else
  {
    for(ii=0;ii<is;ii++)
      {
	xs[ii]-=(vs[ii]-m->min)*m->pa*m->vscale;
	ys[ii]-=(vs[ii]-m->min)*m->pb*m->vscale;
	zs[ii]-=(vs[ii]-m->min)*m->pc*m->vscale;
      }
     glm_3d_simplex_isect_draw(m,num,is,xs,ys,zs,vs);
   }
}

/* draw 3d simplex */
void glm_3d_simplex_plain(glMesh m, int num, int mat,double *nodefunc, int *nodes, double **coord)
{
  double p1,p2,p3,p4;
  if (nodefunc==NULL) nodefunc=m->nodefunc;

  p1=plane_equation(x1,y1,z1);if (p1>0.0) return;
  p2=plane_equation(x2,y2,z2);if (p2>0.0) return;
  p3=plane_equation(x3,y3,z3);if (p3>0.0) return;
  p4=plane_equation(x4,y4,z4);if (p4>0.0) return;


  glVertex3f(x1,y1,z1);
  glVertex3f(x2,y2,z2);

  glVertex3f(x1,y1,z1);
  glVertex3f(x3,y3,z3);

  glVertex3f(x1,y1,z1);
  glVertex3f(x4,y4,z4);

  glVertex3f(x2,y2,z2);
  glVertex3f(x3,y3,z3);

  glVertex3f(x2,y2,z2);
  glVertex3f(x4,y4,z4);

  glVertex3f(x3,y3,z3);
  glVertex3f(x4,y4,z4);

}

/* draw 3d level surface */
void glm_3d_mesh_levelsurface(glRenderer rnd, glMesh m)
{
  float rgb[3];
  glrGetVScale(rnd, &m->vscale);
  m->vscale= -m->vscale;
  m->vscale=m->vscale/(m->max-m->min);
  glrGetLevel(rnd,&m->lev);
  glrGetWireframe(rnd,&m->wireframe);
  glrGetFlatshading(rnd, &m->flat);

  m->eps=0.0;

  /*
  if (!m->wireframe)
  */
    {
      glColor3fv(m->color(m,m->lev,rgb));
    }
  glBegin(GL_TRIANGLES);
  m->loop(m,m->user_data,glm_3d_simplex_levelsurface);
  glEnd();

}

/* draw 3d plane section surface */
void glm_3d_mesh_planesection(glRenderer rnd, glMesh m)
{
  glRendererState state=glrGetRendererState(rnd);
  m->isoline_distance=state->isoline_distance*(m->max-m->min);

  m->eps=0.0;
    /*-1.0e-16; make it relative!!!*/

  glrGetVScale(rnd, &m->vscale);
  m->vscale= -m->vscale;

  m->vscale=m->vscale/(m->max-m->min);
  glrGetPlane(rnd,&m->pa,&m->pb,&m->pc,&m->pd);
  glrGetWireframe(rnd,&m->wireframe);
  glrGetFlatshading(rnd, &m->flat);

  if (m->isoline_mode%2)
    {
      glLineWidth(2.0);
      glBegin(GL_LINES);
    }
  else
    glBegin(GL_TRIANGLES);
  m->loop(m,m->user_data,glm_3d_simplex_planesection);
  glEnd();
  glLineWidth(1.0);
}

void glm_3d_mesh_plain(glRenderer rnd, glMesh m)
{
  glrGetPlane(rnd,&m->pa,&m->pb,&m->pc,&m->pd);
  glBegin(GL_LINES);
  m->loop(m,m->user_data,glm_3d_simplex_plain);
  glEnd();
}







void glm_3d_simplex_ycellflux(glMesh m, int num, int mat, double *nodefunc, int *nodes, double **coord)
{
  double vs[5];
  int is;
  double xs[5],ys[5],zs[5],p[5];
  register int ii;
  if (nodefunc==NULL) nodefunc=m->nodefunc;

  /* plane distances are given by plane equation */
  p[0]=plane_equation(x1,y1,z1);
  p[1]=plane_equation(x2,y2,z2);
  p[2]=plane_equation(x3,y3,z3);
  p[3]=plane_equation(x4,y4,z4);
  is=glm_3d_simplex_isect_calc(m,nodefunc,nodes,coord,p,xs,ys,zs,NULL);
  if (is<3) {return;}
  glm_3d_simplex_isect_draw(m,num,is,xs,ys,zs,NULL);
}


void glm_3d_simplex_xcellflux(glMesh m,int num, int mat, double *nodefunc, int *nodes, double **coord)
{
  int is;
  double xs[5],ys[5],zs[5],p[5];
  /* plane distances are given by function values */

  double *q1=m->cellflux+m->voffset+3*(nodes[0]-1);
  double *q2=m->cellflux+m->voffset+3*(nodes[1]-1);
  double *q3=m->cellflux+m->voffset+3*(nodes[2]-1);
  double *q4=m->cellflux+m->voffset+3*(nodes[3]-1);

  if (nodefunc==NULL) nodefunc=m->nodefunc;
  p[0]=sqrt(q1[0]*q1[0]+q1[1]*q1[1]+q1[2]*q1[2])-m->lev;
  p[1]=sqrt(q2[0]*q2[0]+q2[1]*q2[1]+q2[2]*q2[2])-m->lev;
  p[2]=sqrt(q3[0]*q3[0]+q3[1]*q3[1]+q3[2]*q3[2])-m->lev;
  p[3]=sqrt(q4[0]*q4[0]+q4[1]*q4[1]+q4[2]*q4[2])-m->lev;

  is=glm_3d_simplex_isect_calc(m,nodefunc,nodes,coord,p,xs,ys,zs,NULL);
  if (is<3) {return;}
  glm_3d_simplex_isect_draw(m,num,is,xs,ys,zs,NULL);
}


int intri(double *xs,double *ys,double *zs, double x, double y,double z, int dir)
{


  register int n,d;
  double barycentricCoord[3],v[3][3],w[3][3],coord[4][4],det,invdet;
  int   i,j,k,e;
  double u[3];
  /*do this once in a triangle...*/
  switch(dir)
    {
    case 2:  
      coord[0][0]=xs[0];
      coord[1][0]=xs[1];
      coord[2][0]=xs[2];

      coord[0][1]=ys[0];
      coord[1][1]=ys[1];
      coord[2][1]=ys[2];
      u[0]=x;
      u[1]=y;
      break;

    case 1:  
      coord[0][0]=xs[0];
      coord[1][0]=xs[1];
      coord[2][0]=xs[2];

      coord[0][1]=zs[0];
      coord[1][1]=zs[1];
      coord[2][1]=zs[2];
      u[0]=x;
      u[1]=z;
      break;

    case 0:  
      coord[0][0]=ys[0];
      coord[1][0]=ys[1];
      coord[2][0]=ys[2];

      coord[0][1]=zs[0];
      coord[1][1]=zs[1];
      coord[2][1]=zs[2];
      u[0]=y;
      u[1]=z;
      break;
    }

  for(i=1; i<=2; i++) 
    for(j=0; j<2; j++)
      v[i][j]= coord[i][j]-coord[0][j];
  
  det         =   (v[1][0]*v[2][1] - v[1][1]*v[2][0]);
  

  if (det==0) return 0;
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
	    w[n][d]*(u[d] - coord[0][d]);
	}
      barycentricCoord[0]-= barycentricCoord[n];
    }
  
  if (barycentricCoord[0]<0.0) return 0;
  if (barycentricCoord[1]<0.0) return 0;
  if (barycentricCoord[2]<0.0) return 0;
  return 1;

  /*
  m->pointValue=
    barycentricCoord[0]*nodefunc[nodes[0]]+
    barycentricCoord[1]*nodefunc[nodes[1]]+
    barycentricCoord[2]*nodefunc[nodes[2]];
    */
}
/* draw intersection with plane */
static double dd;
static double fscale;
static int arrow_l;
void glm_3d_simplex_cellflux(glMesh m, int num, int mat, double *nodefunc,int *nodes, double **coord)
{
  double vs[5];
  int is;
  float rgb[5];
  double xs[5],ys[5],zs[5],p[5];
  register int ii;
  double eps=1.0e-10*dd;
   if (nodefunc==NULL) nodefunc=m->nodefunc;

  /* plane distances are given by plane equation */
  p[0]=plane_equation(x1,y1,z1);
  p[1]=plane_equation(x2,y2,z2);
  p[2]=plane_equation(x3,y3,z3);
  p[3]=plane_equation(x4,y4,z4);
  is=glm_3d_simplex_isect_calc(m,nodefunc,nodes,coord,p,xs,ys,zs,NULL);
  if (is<3) {return;}
  if (glm_showmat)
    {
      if (mat>1)
	{
	  glBegin(GL_TRIANGLES);
	  glColor3fv(m->matcolor(m,mat,rgb));
	  if (is>=3)
	    glm_3d_simplex_isect_plain(xs,ys,zs);
	  if (is==4)
	    glm_3d_simplex_isect_plain(xs+1,ys+1,zs+1);
	  glEnd();
	}
      glBegin(GL_LINES);
    }
  {
    double xmin,xmax,ymin,ymax,zmin,zmax,x,y,z,x0,y0,z0;
    int i0,i1,j0,j1,k0,k1,i,j,k,dir;
    double *q=m->cellflux+m->voffset+3*(num-m->voffset);
    double ql;
    float rgb[4];
    ql=sqrt(q[0]*q[0]+q[1]*q[1]+q[2]*q[2]);

        glColor3fv(m->color(m,ql,rgb));

    xmin=xmax=xs[0]; for (ii=1;ii<is;ii++) { if (xs[ii]<xmin) xmin=xs[ii]; else if (xs[ii]>xmax) xmax=xs[ii];}
    ymin=ymax=ys[0]; for (ii=1;ii<is;ii++) { if (ys[ii]<ymin) ymin=ys[ii]; else if (ys[ii]>ymax) ymax=ys[ii];}
    zmin=zmax=zs[0]; for (ii=1;ii<is;ii++) { if (zs[ii]<zmin) zmin=zs[ii]; else if (zs[ii]>zmax) zmax=zs[ii];}

    if (fabs(xmax-xmin)<1.0e-14) dir=0;
    else if (fabs(ymax-ymin)<1.0e-14) dir=1;
    else if (fabs(zmax-zmin)<1.0e-14) dir=2;
    
    xmin=xmin-eps;
    xmax=xmax+eps;
    ymin=ymin-eps;
    ymax=ymax+eps;
    zmin=zmin-eps;
    zmax=zmax+eps;

    i0=(int)(xmin/dd); i1=(int)(xmax/dd);
    j0=(int)(ymin/dd); j1=(int)(ymax/dd);
    k0=(int)(zmin/dd); k1=(int)(zmax/dd);

    x0=i0*dd;
    y0=j0*dd;
    z0=k0*dd;

    switch(dir)
      {
      case 0: i1=i0; break;
      case 1: j1=j0; break;
      case 2: k1=k0; break;
      }
    

    for (z=z0,k=k0;k<=k1;k++,z+=dd)
      for (y=y0,j=j0;j<=j1;j++,y+=dd)
	for (x=x0,i=i0;i<=i1;i++,x+=dd)
	  {
	    if (intri(xs,ys,zs,x,y,z,dir)||(is==4&&intri(xs+1,ys+1,zs+1,x,y,z,dir)))
	      { 
		double qn[5],wd;
		qn[0]=q[0]/ql;
		qn[1]=q[1]/ql;
		qn[2]=q[2]/ql;
		wd=fscale*ql;
		glPushMatrix();
		glTranslatef(x,y,z);
		if (qn[1]>0.0)
		  glRotatef(360*acos(qn[0])/6.2832,0.0,0.0,1.0);
		else
		  glRotatef(-360*acos(qn[0])/6.2832,0.0,0.0,1.0);
		glRotatef(-360*asin(qn[2])/6.2832,0.0,1.0,0.0);
		glRotatef(90.0,0.0,1.0,0.0);
		glScalef(fscale*ql,fscale*ql,2*dd);
		glCallList(arrow_l);
		glPopMatrix();
		/*glBegin(GL_LINES);
		glVertex3d(x,y,z);
		glVertex3d(x+fscale*q[0],y+fscale*q[1],
			   z+fscale*q[2]);
		glEnd();*/
	     }
	 }

  }
  if (glm_showmat)
    glEnd();
}



/* draw cellflux in plane section */
void glm_3d_mesh_cellflux(glRenderer rnd, glMesh m)
{
  float rgb[3];
  glRendererState state=glrGetRendererState(rnd);

  m->eps=0.0;
    /*-1.0e-16; make it relative!!!*/

  glrGetVScale(rnd, &m->vscale);
  m->vscale= fabs(m->vscale);

  glrGetPlane(rnd,&m->pa,&m->pb,&m->pc,&m->pd);
  glrGetWireframe(rnd,&m->wireframe);
  glrGetFlatshading(rnd, &m->flat);

  dd=state->max[0]-state->min[0];
  if ((state->max[1]-state->min[1])<dd) dd=state->max[1]-state->min[1];
  if ((state->max[2]-state->min[2])<dd) dd=state->max[2]-state->min[2];

  dd=dd*0.05;
  fscale=0.99*dd/m->max;
 

  if (!m->wireframe)
    {
      glColor3fv(m->color(m,m->lev,rgb));
    }
  glLineWidth(1.0);
  arrow_l=glrArrowList(rnd);
  m->loop(m,m->user_data,glm_3d_simplex_cellflux);
}




/*
 * $Log: glm-3d.c,v $
 * Revision 2.13  2001/05/21 14:27:19  fuhrmann
 * some warnings
 * gltensview back from Gunther
 *
 * Revision 2.12  2001/05/15 15:23:44  petzoldt
 * Martins Farbskala: Konflikte beseitigt
 *
 * Revision 2.11  2000/12/13 16:17:43  fuhrmann
 * pfeilchen
 *
 * Revision 2.10  2000/12/13 15:30:26  fuhrmann
 * further fltk hacking
 * glprimitives
 * arrows for fluxvisualization
 *
 * Revision 2.9  2000/10/17 16:46:36  fuhrmann
 * started scl 3d visualization
 *
 * Revision 2.8  2000/02/14 15:53:51  fuhrmann
 *  user material color callback
 *
 * Revision 2.7  1999/06/13 12:58:37  fuhrmann
 * interim check-in
 *
 * Revision 2.6  1999/04/29 17:46:33  fuhrmann
 * various changes
 *
 * Revision 2.5  1999/04/28 13:32:09  fuhrmann
 * bugs in cellflux stuff removed
 *
 * Revision 2.4  1999/04/20 17:32:15  fuhrmann
 * sgi warnings, 3D flux test
 *
 * Revision 2.3  1999/04/13 11:43:31  fuhrmann
 * more thorough checks, k&r stuff deleted
 *
 * Revision 2.2  1999/04/09 12:20:28  fuhrmann
 * 3d cell flux running, will need raster point method
 *
 * Revision 2.1  1999/04/08 15:03:14  fuhrmann
 * glmesh split up
 *
 */
