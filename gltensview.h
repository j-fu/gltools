#include <stdio.h>
/*
 * Tensor Product Mesh Function Visualizer API for gltools.
 * Derived from old glview.
 * Has F77 API.
 */

typedef double (*gltvFunc)(double x, double y, double z,double t);
typedef void (*gltvFlux)(double x, double y, double z, double t,double *v);
typedef struct gl_tensor_mesh_struct *GLTensorMesh;


GLTensorMesh gltvCreate(int nx,int ny,int nz,
		     double *xc, double *yc, double *zc);


GLTensorMesh gltvCreateSimple(int nx,int ny,int nz, 
		       double xmin,double xmax,
		       double ymin, double ymax,
		       double zmin, double zmax);

void gltvSetTitle(GLTensorMesh m, char *title);

GLTensorMesh  gltvCreateFile(FILE *in);
void gltvDestroy(GLTensorMesh m);
void gltvCalcNodeFunc(GLTensorMesh m,gltvFunc f, double t);
void gltvCalcCellflux(GLTensorMesh m, gltvFlux f,double t);
int gltvData(GLTensorMesh m);
int gltvNodeFuncf(GLTensorMesh m,gltvFunc f, double t, double fmin, double fmax);
int gltvCellFluxf(GLTensorMesh m,gltvFlux f, double t, double fmin, double fmax);
int gltvNodeFuncv(GLTensorMesh m, double *f, double t, double fmin, double fmax);
int gltvCellFluxv(GLTensorMesh m,double *f, double t, double fmin, double fmax);

#ifdef fortran 
#define F77(x) x
#else
#define F77(x) x##_
#endif

int F77(gltv_create)(int *nx, int *ny, int *nz, double *xc,double *yc,double *zc);
int F77(gltv_create_simple)(int *nx, int *ny, int *nz, 
			    double *xmin,double *xmax,
			    double *ymin, double *ymax,
			    double *zmin, double *zmax);
			    
void F77(gltv_destroy)(int *handle);
int  F77(gltv_node_func)(int *handle, double *f, double *t, double *fmin, double *fmax);
int  F77(gltv_cell_flux)(int *handle, double *f, double *t, double *fmin, double *fmax);
void F77(gltv_title)(int *handle, char *title, int *l_title);

/* Subroutine headers for fortran freaks:
	INTEGER FUNCTION gltv_create(nx,ny,nz,xc,yc,zc)
	integer*4 nx,ny,nz
	real *8 xc(nx), yc(ny),zc(nz)
	integer handle
	...
	gltv_create=handle
	end

	INTEGER FUNCTION gltv_create_simple(nx,ny,nz,xmin,xmax,ymin,ymax,zmin,zmax)
	integer*4 nx,ny,nz
	real *8 xmin,xmax,ymin,ymax,zmin,zmax
	integer handle
	...
	gltv_create=handle
	end

	SUBROUTINE glv_destroy(handle)
	end

	SUBROUTINE gltv_node_func(handle, f,time,fmin,fmax)
	integer *4 handle
	real*8 time,fmin,fmax
	real*8 f(1)
	end

	SUBROUTINE gltv_cell_flux(handle, f,time,fmin,fmax)
	integer *4 handle
	real*8 time,fmin,fmax
	real*8 f(1)
	end

	SUBROUTINE gltv_title(handle, title, l_title)
	integer *4 handle
	integer*4 title(1),l_title
	end

 */

/*
 *  $Log: gltensview.h,v $
 *  Revision 2.4  2001/05/21 14:27:19  fuhrmann
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
