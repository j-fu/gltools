/*$Id: glm-internal.h,v 2.6 2001/05/15 15:23:45 petzoldt Exp $*/
#include "glmesh.h"
#include <math.h>   

typedef float floatrgb[3];

struct glMeshStruct
{
  int cell_num;
  int cell_mat;
  int node_num;
  int meshdim;
  void * user_data;
  /* only one of those can be used!*/
  double *nodefunc;
  double *cellfunc;
  double *cellflux;
  double *nodeflux;
  int voffset;
  glmLoopCallback loop;
  glmColorCallback color;
  glmMaterialColorCallback matcolor;
  double  min,max;
  double xtics[11],ytics[11],ztics[11],ftics[11];
  double glm_color_ctrl;
  double glm_color_fac;
  int first_time;


  double gmin,gmax,cdelta,lev;

  
  float  *n_x,*n_y,*n_z;
  floatrgb *rgb;
  int nisolev;
  double vscale;
  double plane;
  double isoline_distance;
  int isoline_mode;

  int icol;

  double pointCoord[3],pointValue;
  double *val;


double pa,pb,pc,pd;
int wireframe,flat;

double eps;



};

double glm_scale_func(glMesh m,double f);

/* color stuff */
float* glmDefaultMaterialColor(glMesh m, int mat, float *rgbx);
float* glmRBColor(glMesh m,double f, float *rgbx);
float* glmThreePhaseColor(glMesh m,double f, float *rgbx);
float* glmContrastColor(glMesh m,double f, float *rgbx);
float* glmBWColor(glMesh m,double f, float *rgbx);
float*  glmSteelColor(glMesh m,double f, float *rgbx);
float*  glmGrayColor(glMesh m,double f, float *rgbx);
float*  glmMapColor(glMesh m,double f, float *rgbx);


/* 3D stuff */
void glm_3d_mesh_plain(glRenderer rnd, glMesh m);
void glm_3d_mesh_levelsurface(glRenderer rnd, glMesh m);
void glm_3d_mesh_planesection(glRenderer rnd, glMesh m);
void glm_3d_simplex_eval(glMesh m, int num, int mat, double *values,int *nodes, double **coord);
void glm_3d_mesh_cellflux(glRenderer rnd, glMesh m);


/*2D stuff */


void glm_2d_simplex_eval(glMesh m, int num, int mat, double *values, int *nodes, double **coord);
void glm_2d_simplex_isolines(glMesh m, int num, int mat, double *values, int *nodes, double **coord);
void glm_2d_simplex_normals(glMesh m, int num, int mat, double *values, int *nodes, double **coord);
void glm_2d_simplex_flat(glMesh m,int num, int mat, double *values, int *nodes, double **coord);
void glm_2d_simplex_gouraud(glMesh m, int num , int mat, double *values,int *nodes, double **coord);
void glm_2d_simplex_wireframe(glMesh m, int num, int mat,double *values, int *nodes, double **coord);
void glm_2d_simplex_plain(glMesh m, int num, int mat, double *values, int *nodes, double **coord);
void glm_2d_mesh_isolines(glRenderer rnd, glMesh m);
void glm_2d_mesh_flat(glRenderer rnd, glMesh m);
void glm_2d_mesh_gouraud(glRenderer rnd, glMesh m);
void glm_2d_mesh_wireframe(glRenderer rnd, glMesh m);
void glm_2d_mesh_plain(glRenderer rnd, glMesh m);
void glm_2d_simplex_cellflux(glMesh m, int num, int mat,double *values, int *nodes, double **coord);
void glm_2d_mesh_cellflux(glRenderer rnd, glMesh m);

extern int glm_showmat;

/*
 * $Log: glm-internal.h,v $
 * Revision 2.6  2001/05/15 15:23:45  petzoldt
 * Martins Farbskala: Konflikte beseitigt
 *
 * Revision 2.5  2000/10/17 16:46:36  fuhrmann
 * started scl 3d visualization
 *
 * Revision 2.4  2000/02/14 15:53:51  fuhrmann
 *  user material color callback
 *
 * Revision 2.3  1999/04/13 11:43:31  fuhrmann
 * more thorough checks, k&r stuff deleted
 *
 * Revision 2.2  1999/04/09 12:20:29  fuhrmann
 * 3d cell flux running, will need raster point method
 *
 * Revision 2.1  1999/04/08 15:03:14  fuhrmann
 * glmesh split up
 *
 */
