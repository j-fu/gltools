static char *rcsid="$Id: gltensview.c,v 2.5 2001/05/21 14:27:19 fuhrmann Exp $";

#include "gltensview.h"
#include "glmesh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct gl_tensor_mesh_struct
{
  int dim;
  int nn;
  int nc;
  int nx,ny,nz;
  double *xc,*yc,*zc;
  double *cellflux;
  double *nodefunc;
  int owncoord,owncflux,ownnfunc;
  char axisname[4][64];
  glMesh glm;
  glRenderer rnd;
};


static double *create_1d_mesh(int n, double min, double max)
{ 
  int i;
  double h;
  double *c=malloc(n*sizeof(double));
  
  if (n>1) h=(max-min)/(double)(n-1);
  c[0]=min;
  for(i=1;i<n;i++)
    c[i]=(c[i-1]+h);

  return c;
}

GLTensorMesh gltvCreate(int nx,int ny,int nz,
		     double *xc, double *yc, double *zc)
{
  GLTensorMesh m;
  if (!(nx*ny*nz)) 
    { 
      printf("error: not all coordinates given\n");
      return NULL;
    }
  m=calloc(1,sizeof(struct gl_tensor_mesh_struct));
  m->nx=nx;
  m->ny=ny;
  m->nz=nz;
  m->nn=nx*ny*nz;
  m->xc=xc;
  m->yc=yc;
  m->zc=zc;
  m->owncoord=0;
  m->ownnfunc=0;
  m->owncflux=0;
  strcpy(m->axisname[0],"x");
  strcpy(m->axisname[1],"y");
  strcpy(m->axisname[2],"z");
  strcpy(m->axisname[3],"f");
  
  if (m->nz==1)
    {
      m->dim=2;
      m->nc= 2*(nx-1)*(ny-1);
    }
  else
    {
      m->dim=3;
      m->nc=6*(nx-1)*(ny-1)*(nz-1);
    }
  m->rnd=0;
  return m;
}

GLTensorMesh gltvCreateSimple(int nx,int ny,int nz, 
		       double xmin,double xmax,
		       double ymin, double ymax,
		       double zmin, double zmax)
{
  GLTensorMesh m;
  m=gltvCreate(nx,ny,nz,
	      create_1d_mesh(nx,xmin,xmax),
	      create_1d_mesh(ny,ymin,ymax),
	      create_1d_mesh(nz,zmin,zmax));
  m->owncoord=1;
  return m;
}
static double read_double(FILE*f, int *error)
{
    float x;
    *error=0;
    if (feof(f))      
	{
	    fprintf(stderr,"unexpected end of file\n");
	    *error=1;
	    return 0.0;
	}
    fscanf(f,"%e\n",&x);
    return (double)x;
}

GLTensorMesh  gltvCreateFile(FILE *in)
{

  int i, read_err;
  GLTensorMesh m;
  
   
  m=calloc(1,sizeof(struct gl_tensor_mesh_struct));
  fscanf(in,"%s\n",m->axisname[0]);
  fscanf(in,"%s\n",m->axisname[1]);
  fscanf(in,"%s\n",m->axisname[2]);
  fscanf(in,"%s\n",m->axisname[3]);
  fscanf(in,"%d %d %d\n", &m->nx,&m->ny,&m->nz);
  if (!(m->nx*m->ny*m->nz)) 
    { 
      fprintf(stderr,"error: not all coordinates given\n");
      fclose(in);
      free(m);
      return NULL;

    }

  m->xc=calloc(m->nx,sizeof(double));
  m->yc=calloc(m->ny,sizeof(double));
  m->zc=calloc(m->nz,sizeof(double));
  m->nodefunc=calloc(m->nx*m->ny*m->nz,sizeof(double));

  m->owncoord=1;
  m->ownnfunc=1;
  m->owncflux=0;
  
  read_err=0;
  for(i=0;(i<m->nx)&&(!read_err);i++) m->xc[i]=read_double(in,&read_err);
  for(i=0;(i<m->ny)&&(!read_err);i++) m->yc[i]=read_double(in,&read_err);
  for(i=0;(i<m->nz)&&(!read_err);i++) m->zc[i]=read_double(in,&read_err);
  for(i=0;(i<m->nx*m->ny*m->nz)&&(!read_err);i++) 
    {
      m->nodefunc[i]=read_double(in,&read_err);
      /*      printf("%d %e\n",i, m->nodefunc[i]);*/
    }
  if (read_err) 
    {
      free(m->nodefunc);
      free(m);
      m=NULL;
      fprintf(stderr,"Error reading input file\n");
      exit(1);
    }

  m->nn=m->nx*m->ny*m->nz;
  if (m->nz==1)
    {
      m->dim=2;
      m->nc= 2*(m->nx-1)*(m->ny-1);
    }
  else
    {
      m->dim=3;
      m->nc=6*(m->nx-1)*(m->ny-1)*(m->nz-1);
    }
  return m;
}



void gltvDestroy(GLTensorMesh m)
{
  if (m->ownnfunc&&m->nodefunc)free(m->nodefunc);
  if (m->owncflux&&m->cellflux)free(m->cellflux);
  if (m->owncoord) 
    {
      free(m->xc);
      free(m->yc);
      free(m->zc);
    }
  if (m->rnd) glrDestroy(m->rnd);
  free(m);
}


void gltvCalcNodeFunc(GLTensorMesh m,gltvFunc f, double t)
{
  register int i,j,k,l;
  double*xc=m->xc;
  double*yc=m->yc;
  double*zc=m->zc;
  double*v;
  int nx=m->nx;
  int ny=m->ny;
  int nz=m->nz;
  if (m->nodefunc==0) m->nodefunc=calloc(m->nn,sizeof(double));
  m->ownnfunc=1;
  v=m->nodefunc;
  l=0;
  for(k=0;k<nz;k++)
    for(j=0;j<ny;j++)
      for(i=0;i<nx;i++,l++)
	{  
	  v[l]=f(xc[i],yc[j],zc[k],t);
	}
}

void gltvCalcCellFlux(GLTensorMesh m, gltvFlux flux,double t)
{
  register int i,j,k,l;
  double*xc=m->xc;
  double*yc=m->yc;
  double*zc=m->zc;
  double*flx;
  int nx=m->nx;
  int ny=m->ny;
  int nz=m->nz;
  m->owncflux=1;
  if (m->cellflux==0)
    {
      if (nz==1)
	m->cellflux=calloc(2*m->nc+10,sizeof(double));
      else
        m->cellflux=calloc(3*m->nc,sizeof(double));
    }
  

  flx=m->cellflux;
  if (nz==1)
    {
      l=0;
      k=0;
      for(j=0;j<ny;j++)
	for(i=0;i<nx;i++,l++)
	  {  
	    if (i>0&&j>0)
	    {
	      flux(xc[i],yc[j],zc[k],t,flx);
	      flx+=2;
	      flux(xc[i],yc[j],zc[k],t,flx);
	      flx+=2;
	    }
	}
    }
  else
    {
      l=0;
       for(k=0;k<nz;k++)
	for(j=0;j<ny;j++)
	  for(i=0;i<nx;i++,l++)
	    {  
	      if (i>0&&j>0&&k>0)
		{
		  flux(xc[i],yc[j],zc[k],t,flx);
		  flx+=3;
		  flux(xc[i],yc[j],zc[k],t,flx);
		  flx+=3;
		  flux(xc[i],yc[j],zc[k],t,flx);
		  flx+=3;
		  flux(xc[i],yc[j],zc[k],t,flx);
		  flx+=3;
		  flux(xc[i],yc[j],zc[k],t,flx);
		  flx+=3;
		  flux(xc[i],yc[j],zc[k],t,flx);
		  flx+=3;
		}
	    }
    }
  
}





static void set_mesh_volume(glRenderer rnd, GLTensorMesh m)
{
  double zmax;
  zmax=m->zc[m->nz-1];
  if (m->nz==1) zmax=m->zc[0]+1.0;
  glrSetVolume(
	       rnd,  
	       m->xc[0],
	       m->xc[m->nx-1],
	       m->yc[0],
	       m->yc[m->ny-1],
	       m->zc[0],
	       zmax
	       );
}





static void mesh_loop(glMesh glm, GLTensorMesh m, glmSimplexCallback sxf)
{
  register int isx,i,j,k;
  double*xc=m->xc;
  double*yc=m->yc;
  double*zc=m->zc;
  int nx=m->nx;
  int ny=m->ny;
  int imat;
  
  if (m->dim==2)
    {
      int nodes[5];
      double coord0[3],coord1[3],coord2[3],coord3[3];
      double *coord[5];
      coord[0]=coord0;
      coord[1]=coord1;
      coord[2]=coord2;
      coord[3]=coord3;
      coord[4]=coord0;
      
      coord0[2]=coord1[2]=coord2[2]=coord3[2]=zc[0];
      isx=0;
      for (j=0;j<m->ny-1;j++)
	for (i=0;i<m->nx-1;i++)
	  {
	    if (isx<100) imat=1;else imat=2;
	    nodes[0]=i+j*nx;       coord[0][0]=xc[i];   coord[0][1]=yc[j];
	    nodes[1]=nodes[0]+1;   coord[1][0]=xc[i+1]; coord[1][1]=yc[j];
	    nodes[2]=nodes[0]+nx+1;coord[2][0]=xc[i+1]; coord[2][1]=yc[j+1];
	    nodes[3]=nodes[0]+nx;  coord[3][0]=xc[i];   coord[3][1]=yc[j+1];
	    nodes[4]=nodes[0]; 
	    sxf(glm,isx++,imat,NULL,nodes,coord);
	    sxf(glm,isx++,imat,NULL,nodes+2,coord+2);
	  }
    }
  if (m->dim==3)
    {
      int nodes[5];
      int nxy=nx*ny;
      double coord0[3],coord1[3],coord2[3],coord3[3];
      double *coord[4];
      coord[0]=coord0;
      coord[1]=coord1;
      coord[2]=coord2;
      coord[3]=coord3;
      
      isx=0;
      for (k=0;k<m->nz-1;k++)
	for (j=0;j<m->ny-1;j++)
	  for (i=0;i<m->nx-1;i++)
	    {
	    if (isx<100) imat=1;else imat=2;
	      
#define SET_NODE(inode,i,j,k) nodes[inode]=(i)+(j)*nx+(k)*nxy,coord[inode][0]=xc[i],coord[inode][1]=yc[j],coord[inode][2]=zc[k]
		
	      SET_NODE(0,i,  j,  k  );
	      SET_NODE(1,i+1,j,  k  );
	      SET_NODE(2,i+1,j+1,k  );
	      SET_NODE(3,i+1,j+1,k+1);
	      sxf(glm,isx++,imat,NULL,nodes,coord);


	      SET_NODE(0,i,  j,  k  );
	      SET_NODE(1,i+1,j,  k  );
	      SET_NODE(2,i+1,j+1,k+1);
	      SET_NODE(3,i+1,j,  k+1);
	      sxf(glm,isx++,imat,NULL,nodes,coord);

	      SET_NODE(0,i,  j,  k+1);
	      SET_NODE(1,i+1,j,  k+1);
	      SET_NODE(2,i+1,j+1,k+1);
	      SET_NODE(3,i  ,j,  k  );
	      sxf(glm,isx++,imat,NULL,nodes,coord);


	      SET_NODE(0,i,  j,  k+1);
	      SET_NODE(1,i,  j+1,k+1);
	      SET_NODE(2,i+1,j+1,k+1);
	      SET_NODE(3,i  ,j  ,k  );
	      sxf(glm,isx++,imat,NULL,nodes,coord);

	      SET_NODE(0,i,  j,  k  );
	      SET_NODE(1,i,  j+1,k  );
	      SET_NODE(2,i+1,j+1,k+1);
	      SET_NODE(3,i  ,j+1,k+1);
	      sxf(glm,isx++,imat,NULL,nodes,coord);

	      SET_NODE(0,i,  j,  k  );
	      SET_NODE(1,i+1,j+1,k  );
	      SET_NODE(2,i,  j+1,k);
	      SET_NODE(3,i+1,j+1,k+1);
	      sxf(glm,isx++,imat,NULL,nodes,coord);
	    }
    }
  
}

static int quit_application=0;

int  do_quit_application(glRenderer rnd, int kbd_mod_mask)
{ 
  quit_application=1; 
  return GL_FALSE;    
}

void gltvSetTitle(GLTensorMesh m, char *title)
{
 if (m->rnd!=0) 
   {
     glrSetTitle(m->rnd,title);
     return;
   } 
}

static void gltvCreateRenderer(GLTensorMesh m)
{
 if (m->rnd) return;
 m->rnd=glrCreate("gltensview",0,0,500,500);
 set_mesh_volume(m->rnd,m);
 glrSetAxisName(m->rnd,'x',m->axisname[0]);
 glrSetAxisName(m->rnd,'y',m->axisname[1]);
 glrSetAxisName(m->rnd,'z',m->axisname[2]);
 glrSetAxisName(m->rnd,'f',m->axisname[3]);
 m->glm=glmCreate(m->nn,m->nc,m->dim,m,(glmLoopCallback)mesh_loop);
 glrSetVScale(m->rnd,1.0);
 glrSetInfoCallback(m->rnd,(glrDrawCallback)glmDrawInfo);
 glrRegisterKeyAction(m->rnd,GLW_q,do_quit_application,"q:Quit");
 glrSetDialog(m->rnd,0);
}

int gltvData(GLTensorMesh m)
{
  gltvCreateRenderer(m);
  glrSetDialog(m->rnd,1);
  glmSetFunction(m->glm,m->nodefunc,1,-1);
  glRender(m->rnd,(glrDrawCallback)glmDraw,m->glm);       
  return !quit_application;
}

int gltvNodeFuncf(GLTensorMesh m,gltvFunc f, double t, double fmin, double fmax)
{
  gltvCreateRenderer(m);
  gltvCalcNodeFunc(m,f,t);
  glmSetFunction(m->glm,m->nodefunc,fmin,fmax);
  glRender(m->rnd,(glrDrawCallback)glmDraw,m->glm);       
  return !quit_application;
}

int gltvNodeFuncv(GLTensorMesh m,double * f, double t, double fmin, double fmax)
{
  gltvCreateRenderer(m);
  glrSetDialog(m->rnd,1);
  glmSetFunction(m->glm,f,fmin,fmax);
  glRender(m->rnd,(glrDrawCallback)glmDraw,m->glm);       
  return !quit_application;
}


int gltvCellFluxf(GLTensorMesh m,gltvFlux f, double t, double fmin, double fmax)
{
  gltvCreateRenderer(m);
  gltvCalcCellFlux(m,f,t);
  glmSetCellFlux(m->glm,m->cellflux,fmin,fmax);
  glRender(m->rnd,(glrDrawCallback)glmDraw,m->glm); 
  return !quit_application;
}

int gltvCellFluxv(GLTensorMesh m,double *f, double t, double fmin, double fmax)
{
  gltvCreateRenderer(m);
  glmSetCellFlux(m->glm,f,fmin,fmax);
  glRender(m->rnd,(glrDrawCallback)glmDraw,m->glm); 
  return !quit_application;
}

#define MAX_MESHES 100
static int initialized=0;
static GLTensorMesh meshes[MAX_MESHES];

int new_handle(GLTensorMesh m)
{

  int i;
  if (!initialized)
    {
      for (i=1;i<MAX_MESHES;i++)
	meshes[i]=0;
      initialized=1;
    }

  for (i=1;i<MAX_MESHES;i++)
    {
      if (!meshes[i])
	{
	  meshes[i]=m;
	  return i;
	}
    }
  assert(0);
}

GLTensorMesh mesh(int i)
{
  assert(i>=0);
  assert(i<MAX_MESHES);
  assert(meshes[i]);
  return meshes[i];
}

void del_handle(int i)
{
  assert(i>=0);
  assert(i<MAX_MESHES);
  assert(meshes[i]);
  meshes[i]=0;
}

int F77(gltv_create)(int *nx, int *ny, int *nz, double *xc,double *yc,double *zc)
{
  GLTensorMesh m;
  m=gltvCreate(*nx, *ny,*nz, xc,yc,zc);
  return new_handle(m);
}

int F77(gltv_createsimple)(int *nx, int *ny, int *nz, 
			    double *xmin,double *xmax,
			    double *ymin, double *ymax,
			    double *zmin, double *zmax)
{

  GLTensorMesh m;
  m=gltvCreateSimple(*nx, *ny,*nz, 			    
		     *xmin,*xmax,
		     *ymin,*ymax,
		     *zmin,*zmax);
  return new_handle(m);
}
			    
void F77(gltv_destroy)(int *handle)
{
  GLTensorMesh m;
  m=mesh(*handle);
  if (m)
    gltvDestroy(m);
  del_handle(*handle);
}

void F77(gltv_title)(int *handle, char *title, int * l_title)
{
  char titbuf[128];
  int i;
  for (i=0;i< (*l_title); i++)
    titbuf[i]=title[i];
  gltvSetTitle(mesh(*handle),titbuf);
}

int  F77(gltv_nodefunc)(int *handle,double *f, double *t, double *fmin, double *fmax)
{
  GLTensorMesh m;
  m=mesh(*handle);
  return gltvNodeFuncv(m,f, *t, *fmin, *fmax);
}

int  F77(gltv_cellfflux)(int *handle,double *f, double *t, double *fmin, double *fmax)
{
  GLTensorMesh m;
  m=mesh(*handle);
  return gltvCellFluxv(m,f, *t, *fmin, *fmax);
}

/*
 *  $Log: gltensview.c,v $
 *  Revision 2.5  2001/05/21 14:27:19  fuhrmann
 *  some warnings
 *  gltensview back from Gunther
 *
 *  Revision 2.3  2001/03/09 13:20:20  fuhrmann
 *   An Gunter
 *
 *  Revision 2.2  2001/03/08 17:37:28  fuhrmann
 *  First running version of gltensview stuff
 *
 */




