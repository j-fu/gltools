/*
$Log: glsimplex.c,v $
Revision 1.1  2000/02/16 00:16:03  fuhrmann
First ideas


 */

/* Features of this code:
Allow piecewise linear, but not necessarily
continuous functions. This allows for rendering of piecewise
constant functions.

Further, this code does not need any vector data structure for
the function values, they can be calculated on the fly.

The basic rendering item is a linear function with linear
color data on one simplex.
This allows to render e.g. two functions on the same mesh
with different colors. It also allows to render mesh material.

??? Lower dimensional simplices ???
*/

#define MAXDIM 3
#define MAXCOL 3

typedef struct glsx_context
{
  int dim;
  glRenderer rnd;

  double coords[MAXDIM+1][MAXDIM];
  double values[MAXDIM+1];
  float colors[MAXDIM+1][MAXCOL];
  int icoord;  
  int icolor;
  int ivalue;
  double pa,pb,pc,pd;

} *glSimplexContext;

/*
Begin
initialize vertex=0, cols=1
*/

glSimplexContext glsxCreate(glRenderer rnd)
{
  glSimplexContext sx=calloc(1,sizeof(struct glsx_context));
  sx->rnd=rnd;
}

void glsxBeginSimplices(glSimplexContext sx, int dim)
{
  sx->dim=dim;
  /* get planes etc */
}

void glsxScalar(glSimplexContext sx, double value)
{
  sx->values[sx->ivalue]=value;
  sx->ivalue++;
}

void glsxColor3dv(glSimplexContext sx, double *rgb)
{
  sx->colors[sx->icolor][0]=rgb[0];
  sx->colors[sx->icolor][1]=rgb[1];
  sx->colors[sx->icolor][2]=rgb[2];
  sx->icolor++;
}

void glsxVertexdv(glSimplexContext sx, double *coord)
{
 int i;
 for (i=0;i<sx->dim;i++)
   sx->coords[sx->icoord][i]=coord[i];
 sx->icoord++;
 if (sx->icoord==sx->dim)
   {
     sx->render(sx);
     sx->icoord=0;
     sx->icolor=0;
     sx->ivalue=0;
   }
}


#define v1 sx->value[0]
#define v2 sx->value[1]
#define v3 sx->value[2]
#define v4 sx->value[3]

#define plane_equation(x,y,z) (sx->pa*(x)+sx->pb*(y)+sx->pc*(z)+sx->pd)
#define x1   sx->coord[0][0]
#define y1   sx->coord[0][1]
#define z1   sx->coord[0][2]

#define x2   sx->coord[1][0]
#define y2   sx->coord[1][1]
#define z2   sx->coord[1][2]

#define x3   sx->coord[2][0]
#define y3   sx->coord[2][1]
#define z3   sx->coord[2][2]

#define x4   sx->coord[3][0]
#define y4   sx->coord[3][1]
#define z4   sx->coord[3][2]


/* calculate intersection between plane and tetrahedron if
  given  distances p of the nodes from that plane */
int glm_3d_simplex_isect_calc(glSimplexContext sx,
			      double *p,
			      double *xs,double *ys,double *zs,
			      double *vs)
{
  int is;
  double t;
  if ( (p[0]<0.0)&&(p[1]<0.0)&&(p[2]<0.0)&&(p[3]<0.0)) return 0;
  if ( (p[0]>0.0)&&(p[1]>0.0)&&(p[2]>0.0)&&(p[3]>0.0)) return 0;
  is=0;
  if (p[0]*p[1]<sx->eps)
    {
      t= p[0]/(p[0]-p[1]);
      xs[is]=x1+t*(x2-x1);
      ys[is]=y1+t*(y2-y1);
      zs[is]=z1+t*(z2-z1);
      if (vs) vs[is]=v1+t*(v2-v1);
      is++;
    }
  if (p[0]*p[2]<sx->eps)
    {
      t= p[0]/(p[0]-p[2]);
      xs[is]=x1+t*(x3-x1);
      ys[is]=y1+t*(y3-y1);
      zs[is]=z1+t*(z3-z1);
      if (vs)  vs[is]=v1+t*(v3-v1);
      is++;
    }
  if (p[0]*p[3]<sx->eps)
    {
      t= p[0]/(p[0]-p[3]);
      xs[is]=x1+t*(x4-x1);
      ys[is]=y1+t*(y4-y1);
      zs[is]=z1+t*(z4-z1);
      if (vs) vs[is]=v1+t*(v4-v1);
      is++;
    }

  if (p[1]*p[2]<sx->eps)
    {
      t= p[1]/(p[1]-p[2]);
      xs[is]=x2+t*(x3-x2);
      ys[is]=y2+t*(y3-y2);
      zs[is]=z2+t*(z3-z2);
      if (vs) vs[is]=v2+t*(v3-v2);
      is++;
    }
  if (p[1]*p[3]<sx->eps)
    {
      t= p[1]/(p[1]-p[3]);
      xs[is]=x2+t*(x4-x2);
      ys[is]=y2+t*(y4-y2);
      zs[is]=z2+t*(z4-z2);
      if (vs) vs[is]=v2+t*(v4-v2);
      is++;
    }
  if (p[2]*p[3]<sx->eps)
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
void glm_3d_simplex_levelsurface(glSimplexContext sx)
{
  int is;
  double xs[5],ys[5],zs[5],p[5];
  /* plane distances are given by function values */
  p[0]=v1-sx->lev;
  p[1]=v2-sx->lev;
  p[2]=v3-sx->lev;
  p[3]=v4-sx->lev;
  is=glm_3d_simplex_isect_calc(sx,p,xs,ys,zs,NULL);
  if (is<3) {return;}
  glm_3d_simplex_isect_draw(sx,is,xs,ys,zs,NULL);
}


/* draw intersection with plane */
void glm_3d_simplex_planesection(glSimplexContext sx)
{
  double vs[5];
  int is;
  float rgb[5];
  double xs[5],ys[5],zs[5],p[5];
  register int ii;
  /* plane distances are given by plane equation */
  p[0]=plane_equation(x1,y1,z1);
  p[1]=plane_equation(x2,y2,z2);
  p[2]=plane_equation(x3,y3,z3);
  p[3]=plane_equation(x4,y4,z4);
  is=glm_3d_simplex_isect_calc(sx,p,xs,ys,zs,vs);
  if (is<3) {return;}

  /* scale into plane orthogonal direction */
  for(ii=0;ii<is;ii++)
    {
      xs[ii]-=(vs[ii]-m->min)*m->pa*m->vscale;
      ys[ii]-=(vs[ii]-m->min)*m->pb*m->vscale;
      zs[ii]-=(vs[ii]-m->min)*m->pc*m->vscale;
    }
  glm_3d_simplex_isect_draw(sx,is,xs,ys,zs,vs);
}

/*
---> completely new code!!!

glsxBeginSimplices(dim)
glsxVertexv
glsxCoord1d,2d,3d
glsxValue
glsxColor3d
glsxColorv
glsxColorDefault
glsxEndSimplices()


g
glsxVertex* gets vertex data



glsxScalar(double val)
sx->actval=val;

glsxVector(double val)

glsxColor(col)
sx->actcol=col
glsxColorDefault()
sx->actcol=defaultcol(sx->actval)

glsxVertex
ivtx++
sx->vertex(ivtx)=vtx
sx->col(ivtx)=sx->actcol
sx->val(ivtx)=sx->acval

if ivtx==dim
render(method)
ivtx=0

*/
















