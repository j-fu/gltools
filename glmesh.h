/*
% $Id: glmesh.h,v 2.19 2000/10/18 16:43:32 fuhrmann Exp $

\section{glmesh - Function Drawing on Simplex Meshes}
Author:J\"urgen Fuhrmann\\
$Revision: 2.19 $\\
$Date: 2000/10/18 16:43:32 $\\
\bigskip



{\em  glmesh}   contains rendering   routines   for   triangular   and
tetrahedreal  meshes based on  callback  functions invoking loops over
simplices of a mesh  in a data structure given  by the user. No  extra
data structure  has  to be generated.   Sure, this  causes performance
drawbacks, but instead the user gains  great flexibility in using this
interface on his/her data  strucutures.  

{\em glmesh} manages plane sections and  level sets for 3D tetrahedral
meshes. 

The basic principle  is a double  callback mechanism - a loop callback
function gets  user data and a simplex  callback function as parameter
which  has to be fed with  number, node numbers  and  coordinates of a
simplex. 

%--------------------------------------------------------------------
\subsection{Imported packages}
*/

#ifndef GLMESH_H
#define GLMESH_H


#include "glrnd.h"
/*
%--------------------------------------------------------------------
\subsection{Data Types}
*/


/*\func{glMeshStruct}*/
typedef struct glMeshStruct *glMesh;

/*
This structure contains all necessary mesh data and is hidden from the
user.
*/


/*subsection{:Constructors and Destructors}*/

/*\func{glmSimplexCallback}*/
typedef void (*glmSimplexCallback)
     (
      glMesh m,
      int  number_of_this_simplex,
      int material_of_this_simplex,
      double *function_defined_on_this_this_simplex,
      int *index_in_funtion_on_this_simplex, 
      double **coordinates_of_the_nodes
      );

/*\func{glmLoopCallback}*/
typedef void (*glmLoopCallback)
     (
      glMesh m,
      void *user_data, 
      glmSimplexCallback call_this_on_every_simplex
      );

/*\func{glmCreate}*/
glMesh glmCreate(
		 int number_of_mesh_nodes, 
                 int number_of_mesh_simplices, 
                 int space_dimension, 
                 void *user_data, 
                 glmLoopCallback loop_over_all_simplices
		 );
/*

Generate an instance of {\tt  glMesh}. This is cheap.
*/

/*\func{glmDestroy}*/
void glmDestroy(
		glMesh m
		);

/*
Destroy an instance of {\tt  glMesh}.
*/


/*\subsection{Setting Data}*/

/*\func{glmSetFunction}*/
void glmSetFunction(
		    glMesh m, 
		    double *f,
		    double min,
		    double max
		    );
/*

 Set piecewise linear function to plot, its minimum and its maximum.  
 If $\mbox{\tt min}>\mbox{\tt max}$, they are automatically calculated. 

*/
double *glmGetNodeFunc(glMesh m);

void *glmGetUserData(glMesh m);

/*\func{glmSetFunction}*/
void glmSetCellFlux(glMesh m, double *values, double min, double max);
/*
  Set piecewise constant flux to plot
 */
/*\func{glmSetVoffset}*/
void glmSetVoffset(
		   glMesh m,
		   int voffset
		   );
/*
Set vector offset (0 or 1) 
*/


/*\func{glmColorCallback}*/
typedef float* (*glmColorCallback)
     (
      glMesh m,
      double fval,
      float *rgb
      );

/*\func{glmSetColorCallback}*/
void glmSetColorCallback(
			 glMesh m, 
			 glmColorCallback col
			 );

/*
 Set color calculation function. {\tt glmRBColor} is the default value.
*/

/*\func{glmMaterialColorCallback}*/
typedef float* (*glmMaterialColorCallback)
     (
      glMesh m,
      int imat,
      float *rgb
      );

/*\func{glmSetMaterialColorCallback}*/
void glmSetMaterialColorCallback(
			 glMesh m, 
			 glmMaterialColorCallback col
			 );

/*
 Set material color calculation function. {\tt glmDefaultMaterialColor} is the default value.
*/

/*\func{glmDrawInfo}*/
void glmDrawInfo(glRenderer rnd, glMesh m);
/*
Info call back for mesh data.
*/

/*\subsection{Invocation}*/
/*\func{glmDraw}*/
void glmDraw(
	     glRenderer rnd, 
	     glMesh m
	     );
/*

The glmesh draw routines are invoked using {\tt glmDraw} as
a callback for {\tt glRender}.

*/
#endif
/*
 %
 % $Log: glmesh.h,v $
 % Revision 2.19  2000/10/18 16:43:32  fuhrmann
 % scl_3d_gl works
 %
 % Revision 2.18  2000/10/18 08:41:50  fuhrmann
 %  scl_3d next step
 %
 % Revision 2.17  2000/10/17 16:46:36  fuhrmann
 % started scl 3d visualization
 %
 % Revision 2.16  2000/02/14 15:53:51  fuhrmann
 %  user material color callback
 %
 % Revision 2.15  1999/04/09 12:20:29  fuhrmann
 % 3d cell flux running, will need raster point method
 %
 % Revision 2.14  1999/04/08 15:03:15  fuhrmann
 % glmesh split up
 %
 % Revision 2.13  1999/01/26 17:14:49  fuhrmann
 % time depenedent demo mode in glview
 % color interpolation for 3D sections in glmesh
 %
 % Revision 2.12  1998/07/27  19:10:03  fuhrmann
 % material stuff in glmesh
 %
 % Revision 2.11  1998/07/06  10:45:11  fuhrmann
 % Increased compiler warning level for all PDELIB-1.7 modules,
 % removed all bugs, some warnings remain (rcsids, ld_init, unused parameters)
 %
 % Revision 2.10  1997/11/24  17:51:35  fuhrmann
 % revision history out of doc
 %
 % Revision 2.9  1997/10/27  14:39:43  fuhrmann
 % doc stuff
 %
 % Revision 2.8  1997/09/26  10:02:38  fuhrmann
 % glmDraw is not allowed to overwrite info callback because it
 % cannot know if it is called diretly by the renderer with the
 % mesh as data parameter.
 %
 % Revision 2.7  1997/05/20  15:59:38  fuhrmann
 % no more rcs style, standalone compilation
 %
 % Revision 2.6  1997/05/19  18:08:53  fuhrmann
 % func,xref
 %
 % Revision 2.5  1997/05/19  15:46:22  fuhrmann
 % no standalone compilation of doc
 %
 % Revision 2.4  1997/04/15  08:09:55  fuhrmann
 % new type of color callback
 %
 % Revision 2.3  1997/04/04  12:27:20  fuhrmann
 % c2latex for documentation
 %
 % Revision 2.2  1996/09/23  16:59:23  fuhrmann
 % Documentation using xdoc
 %
 % Revision 2.1  1996/09/06  18:16:40  fuhrmann
 % added comments, min/max handling
 %
 % Revision 2.0  1996/02/15  19:57:10  fuhrmann
 % First meta-stable distribution
 %
 % Revision 1.1  1995/10/20  15:42:38  fuhrmann
 % Initial revision
 %
 */

