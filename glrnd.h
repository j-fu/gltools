/*
% $Id: glrnd.h,v 2.32 2001/05/21 14:27:19 fuhrmann Exp $


\section{glrnd - Rendering Volume Management }
$Revision: 2.32 $\\
$Date: 2001/05/21 14:27:19 $\\
Author: J\"urgen Fuhrmann, Hartmut Langmach\\
\bigskip


{\em  glrnd}  provides a  framework   for  rendering data in  a  given
rectangular   rendering   volume and   manages   all  transformations,
intersection  planes  and  light sources,   so that  actual  rendering
modules do not have to care about this stuff.  It is controlled by the
keyboard and the mouse. 


\subsubsection{Keyboard User Interface}

The main key to know when working  with {\em glrnd}  is the {\em state
control} key.  It  allows to toggle between application-controlled and
user-controlled  mode.  If the window  is in user-controlled state, by
pressing the state control  key together with  the shift key, you give
control back to the application only until the next invocation of {\tt
glRender()}. 

The following table may be incomplete. You can get the actual keyboard
layout by pressing the help-Key. 

\vskip\baselineskip
\input glrnd-keys.tex


\subsubsection{Mouse interface} 
All actions can be performed with the  left mouse button pressed down.
Which action is performed  depends on the state  change mode.  One can
cycle through the state change mode using the next/prev keyboard keys.
Wich state change mode is active, is shown in the window title. 

Selected state change modes are  bound to other  mouse buttons and the
combination  of   the shift   key   and  a  mouse button.  Again,  the
information which mode  is active when  pressing a button is given  in
the window title. 

\subsubsection{Graphical User Interface}
When the MOTIF option is active  during installation, a graphical user
interface    can  be  used  with   glrnd.   See  the documentation  of
{\em glgui}.

*/

/*%---------------------------------------------------------------%*/
/*
\subsection{Imported packages}
*/

#ifndef GLRND_H 
#define GLRND_H 
#include "glwin.h"

/*%---------------------------------------------------------------%*/

/*\subsection{Data Types}*/
/*\func{glRenderer}*/
typedef struct glRendererStruct *glRenderer;
/*
 Hidden Data type which contains rendering data.                       
*/

/*\func{glrApplicationOption}*/
#define GLR_MAX_APPLICATION_OPTIONS 10
typedef struct
{
  char key[32];
  int val;
} glrApplicationOption;


/*\func{glRendererState}*/
#define GLR_MAX_OBJECTS 9

#define GLR_DIR_X 0
#define GLR_DIR_Y 1
#define GLR_DIR_Z 2

typedef struct glRendererStateStruct
{
  double rotx;  /* rotation around x axis */
  double roty;  /* rotation around y axis */
  double rotz;  /* rotation around z axis */
  double tranx; /* translation in x direction */
  double trany; /* translation in y direction */ 
  double tranz; /* translation in z direction */
  double vscale; /* value scale [0.0-1.0] */
  double zoom;   /* zoom factor */
  double ctrl_prm;   /* control parameter for color scale */
  double ctrl_fac;   /* control parameter for color scale */
  int wireframe; /* show data as wire frame */
  int show_frame; /* show frame */
  int move_model;    /*switch on  rendering when moving */ 
  double asp;         
  int gouraud_shading; /* use gouraud shading */
  double sensitivity;  /* mouse sensitvity */
  int bg_black;        /* background color black */
  double ltx;          /* light position */
  double lty;          /* light position */
  double ltz;          /* light position */
  double plane_d[3];   /* depending on direction, between *min and *max */
  double scale[3];     /* scale factors for rendering volumes*/
  int plane_dir;       /* plane direction (x orth/y orth z orth)*/
  int level_surf;      /* show isolevel surfaces */
  double level;        /* isolevel (between fmin and fmax)*/
  int ortho;           /* orthogonal projection */
  int what_done;            /* what has been changed last */ 
  double isoline_distance; /* distance between isolines - obsolete*/
  int isoline_mode;        /* show isolines */ 
  int show_object[GLR_MAX_OBJECTS+1];       /* show remembered object list */

  double min[3],max[3];        /* renderer volume; read only! */
  
  double fmin,fmax;        /* min,max of current function */

  int transparency;
  int spacedim;
  int show_info;

  glrApplicationOption options[GLR_MAX_APPLICATION_OPTIONS];
  int noptions;
}  *glRendererState;


/* 
This public structure is desinged to hold all state
data which are necessary for the interaction with the
renderer.

*/



#define GLR_ROTATE    1
#define GLR_TRANSLATE 2
#define GLR_LIGHT     3
#define GLR_ISOLEVEL  5
#define GLR_PLANE     6
#define GLR_INPLANE   7
#define GLR_PLANE_ASPECT    8
#define GLR_INPLANE_ASPECT    9
#define GLR_N_DO         10
/* this are the possible values for what\_done */




/*%---------------------------------------------------------------%*/
/*\subsection{Constructors and Destructors}*/

/*\func{glrCreate}*/
glRenderer glrCreate(
                     char *title, 
                     int xpos,
                     int ypos,
                     int width,
                     int height
                     );
/*
Create renderer. It also calls {\tt glwin} 
to create a  window with corresponding  data. 

*/
                           

/*\func{glrDestroy}*/
void glrDestroy(
                glRenderer rnd
                );
/*

 Destroy renderer (and corresponding window).

*/

/*%---------------------------------------------------------------%*/
/*\subsection{ Setting/Getting Data}*/
/*\func{glrSetTitle}*/
void glrSetTitle(
                 glRenderer rnd, 
                 char *title
                 );
/*
Set title.
*/

/*\func{glrReset}*/
void glrReset(
              glRenderer rnd
              );

/*
Reset to default all rotations etc.

*/
/*\func{glrSetVolume}*/
void glrSetVolume(
                  glRenderer rnd,
                  double xmin, 
                  double xmax, 
                  double ymin,
                  double ymax,
                  double zmin,
                  double zmax
                  );
/*
 Define rendering volume.
 Everything drawn by the user function has to be placed within this 
 volume to be visible.
*/

/*\func{glrSetUserInfo}*/
void glrSetUserInfo(
                    glRenderer rnd, 
                    char *user_info, /* format string */
                    ...              /* data according to user\_info */ 
                    );
/* 
 Set user information to be printed in the cooresponding field
 of the renderer. 
*/

/*\func{void glrGetPoint}*/
void glrGetPoint(
                 glRenderer rnd, 
                 double *x, double *y, double *z
                 );
/* get intersection point of main planes */

/*\func{void glrGetPlane}*/
void glrGetPlane(
                 glRenderer rnd, 
                 double *a, 
                 double *b, 
                 double *c,
                 double *d
                 );

/*\func{void glrSetPlane}*/
void glrSetPlane(
                 glRenderer rnd, 
                 double a, 
                 double b,
                 double c, 
                 double d
                 );
/*
Get/set data of intersection plane for 3D plane sections:
the plane is defined by the equation
$$
ax+by+cz+d=0
$$
in the three-dimensional space.
*/

/*\func{glrXSetPlane}*/
void glrXSetPlane(
                  glRenderer rnd, 
                  int dir, 
                  double val
                  );
/*

Alternative way to set intersection plane  data: {\tt dir} denotes the
direction ( 0 = x, 1=y, 2=z) the plane should be  orthogonal to, and val
denotes the distanze from zero. 

*/

/*\func{glrSetAxisName}*/
void glrSetAxisName(
                 glRenderer rnd, 
                 char dir, char *name
                 );
/*
Set title.
*/
/*\func{glrSetAxisTics}*/
void glrSetAxisTics(glRenderer rnd, char dir, int ntics, double *tics);
/*
  Set axis tics;
 */


/*\func{glrArrowList}*/
int glrArrowList(glRenderer rnd);

/*%---------------------------------------------------------------%*/
/*\subsection{Callbacks and Event Processing }*/

/*\func{glrDrawCallback}*/
typedef void (*glrDrawCallback)(
                                glRenderer rnd, 
                                void *data
                                );
/*
Callback function for drawing data.
*/

/*\func{glRender}*/
void glRender(
              glRenderer rnd, 
              glrDrawCallback f, 
              void *data
              );

void glRenderWithGUI(glRenderer rnd, 
		     glrDrawCallback f, 
		     void *info);
/*
Render data with given callback function.
*/


/*\func{glrSetSecondaryCallback}*/
void glrSetSecondaryCallback(glRenderer rnd,glrDrawCallback scb);
/*
The user can call a secondary callback which e.g. calls a 
slave renderer.
*/

/*\func{glrSetInfoCallback}*/
void glrSetInfoCallback(glRenderer rnd,glrDrawCallback info);
/*
The user can draw information into the upper and right info 
areas.
*/
 		 
/*\func{glrDefaultInfoCallback}*/
void glrDefaultInfoCallback(glRenderer rnd,void * thrash);
/* 
This is the default info callback, a user info callback can
call this.
*/		 


/*\func{glrSetDataValid}*/
void glrSetDataValid(
                     glRenderer rnd, 
                     int valid
                     );
/*
Tell renderer that all display lists it had compiled are still 
valid (for dumping and interaction with the GUI).

*/

/*\func{glrShowModel}*/
void glrShowModel(glRenderer rnd,int ishow);
/*
Tell renderer that it should not draw the model. This function
can be used by the gui code. On slow displays this may make sense.
*/




/*%---------------------------------------------------------------%*/

/*\subsection{State file  management}*/
/*\func{glrSaveState}*/
void  glrSaveState(glRenderer rnd, char *filename);
/*
Save actual transformation state using a state file.
A default filename is generated when the second parameter is zero.
*/
/*\func{glrRestoreState}*/
void  glrRestoreState(glRenderer rnd,char *filename);
/*
Restore actual transformation state using a state file.
A default filename is generated when the second parameter is zero.
*/

/*\func{glrSetStateFileNameStub}*/
void glrSetStateFileNameStub(glRenderer rnd, char *name);

/*%---------------------------------------------------------------%*/
/*\subsection{ Key actions}*/
/*\func{glrKeyAction}*/
typedef int (*glrKeyAction)(
                            glRenderer rnd,
                            int mask
                            );
/*\func{glrRegisterKeyAction}*/
void glrRegisterKeyAction(
                          glRenderer rnd, 
                          int key, 
                          glrKeyAction action, 
                          char *help
                          );
/*
 Register key action callback routine
*/

/*\func{glrDumpHelpFile}*/
void glrDumpHelpFile(glRenderer rnd);
/* Create help info for keys in LaTeX format */


/*%---------------------------------------------------------------%*/
/*\subsection{Frame dump}*/
/*\func{glrSetDumpFileNameStub}*/
void glrSetDumpFileNameStub(
                            glRenderer rnd, 
                            char *name
                            );
/*

Set file name stub for data dump. Subsequent dumps get the
corresponding number in the file name. The default name stub is
derived from the title. 
*/

/*\func{glrSetDumpPixmapSize}*/
void glrSetDumpPixmapSize(
                          glRenderer rnd, 
                          int w, 
                          int h
                          );
/*
Set size of dump pixmap.
*/

/*\func{glrDumpNext}*/
void glrDumpNext(
                 glRenderer rnd
                 );
/*
Dump data when invoking {\tt glRender} next time.
*/




/*%---------------------------------------------------------------%*/
/*\subsection{Graphical User Interface}*/

/*\func{glrGUI}*/
typedef void (*glrGUI)(glRenderer rnd);
/*\func{glrSetGUI}*/
void glrSetGUI(glrGUI gui);
/*
 Bring up a graphical user interface. It should interact
with the renderer via the state structure \xref{glRendererState} and the 
\xref{glrSetDataValid} call.
*/

/*\func{glrGetRendererState}*/
glRendererState  glrGetRendererState(glRenderer rnd);

/*\func{glrGetWindow}*/
glWindow glrGetWindow(glRenderer rnd);
/*\func{glrGetInfo}*/
void *glrGetInfo(glRenderer rnd);

/*%---------------------------------------------------------------%*/
/*\subsection{Application options}*/
/*Application options could be set using a keyboard callback. Alternatively,
a pulldown menu in the GUI is defined which contains all the application
options. They should be used to interactively 
influence user data which are application
specific.*/

/*\func{glrSetApplicationOption}*/
void glrSetApplicationOption(glRenderer rnd,char *key, int val);
/*\func{glrGetApplicationOption}*/
int glrGetApplicationOption(glRenderer rnd,char *key);


/*%---------------------------------------------------------------%*/

/*\subsection{Obsolete functions}*/
/* These functions are considered to be obsolete. They are still
maintained for backward compatibility.*/


void glrSetFlatshading(glRenderer rnd, int flat);
void glrGetFlatshading(glRenderer rnd,int *flat);

void   glrGetVScale(glRenderer rnd,double* vscale);
void   glrSetVScale(glRenderer rnd, double vscale);

void glrGetWireframe(glRenderer rnd, int *wireframe);

void glrGetLevelSurface(glRenderer rnd, int* mode);
void glrSetLevelSurface(glRenderer rnd,int mode);

void glrGetLevel(glRenderer rnd, double* lev);
void glrSetLevel(glRenderer rnd, double lev);

void glrGetIsolineMode(glRenderer rnd, int* mode);
void glrSetIsolineMode(glRenderer rnd,int mode);

void glrMoveWireframe(glRenderer rnd);
void glrMoveFrame(glRenderer rnd);
void glrMoveModel(glRenderer rnd);

void glrGetDialog(glRenderer rnd, int* dialog);
void glrSetDialog(glRenderer,int dialog);

typedef void (*glrDrawCallback2)(
                                 glRenderer rnd, 
                                 void *data1, 
                                 void *data2
                                 );

void glRender2(glRenderer rnd, glrDrawCallback2 f, 
               void *data1,void *data2);


int  glrLoadFont(glRenderer rnd, int font_number, char *fontName);
void glrSelectFont(glRenderer rnd, int font_number);
void glrSetFontSize(glRenderer rnd, double font_size);
void glrPrint(glRenderer rnd, char *text);
void glrPrintf(glRenderer rnd, char *format, ...);

#endif

/*
 %
 % $Log: glrnd.h,v $
 % Revision 2.32  2001/05/21 14:27:19  fuhrmann
 % some warnings
 % gltensview back from Gunther
 %
 % Revision 2.31  2001/05/15 15:23:45  petzoldt
 % Martins Farbskala: Konflikte beseitigt
 %
 % Revision 2.30  2000/01/12 17:50:59  fuhrmann
 % ci stuff repaired
 %
 % Revision 2.29  1999/02/05 20:24:34  fuhrmann
 % tics introduced
 %
 % Revision 2.28  1998/12/18 13:55:56  fuhrmann
 % make keydoc stuff
 %
 % Revision 2.27  1998/07/27 18:04:29  fuhrmann
 % Handling 3 planes, function evaluation, glrGetPoint
 %
 % Revision 2.26  1998/07/06  10:45:11  fuhrmann
 % Increased compiler warning level for all PDELIB-1.7 modules,
 % removed all bugs, some warnings remain (rcsids, ld_init, unused parameters)
 %
 % Revision 2.25  1998/03/06  16:45:30  fuhrmann
 % tiff->ppm
 % print command
 % video recording
 %
 % Revision 2.24  1997/11/27  19:03:09  fuhrmann
 % glwRecord stuff, PAL-Format, tex-file for keys
 %
 % Revision 2.23  1997/11/24  17:51:36  fuhrmann
 % revision history out of doc
 %
 % Revision 2.22  1997/10/27  14:39:44  fuhrmann
 % doc stuff
 %
 % Revision 2.21  1997/10/15  15:17:25  fuhrmann
 % secondary callback stuff
 %
 % Revision 2.20  1997/09/22  17:37:47  fuhrmann
 % info callback stuff
 %
 % Revision 2.19  1997/05/20  15:59:46  fuhrmann
 % no more rcs style, standalone compilation
 %
 % Revision 2.18  1997/05/19  18:09:06  fuhrmann
 % func,xref
 %
 % Revision 2.17  1997/05/19  15:46:05  fuhrmann
 % no standalone compilation of doc
 %
 % Revision 2.16  1997/05/15  10:52:57  fuhrmann
 % glrShowModel stuff
 %
 % Revision 2.14  1997/04/21  09:42:15  fuhrmann
 % user_data stuff - this should be the kernel of the database module
 %
 % Revision 2.13  1997/04/15  08:11:26  fuhrmann
 % spacedim -> renderer state
 %
 % Revision 2.12  1997/04/10  13:55:33  fuhrmann
 % selective display of objects
 %
 % Revision 2.11  1997/04/04  12:26:19  fuhrmann
 % c2latex for documentation
 %
 % Revision 2.10  1997/03/20  18:17:48  fuhrmann
 % bounding box + function min/max -> state
 %
 % Revision 2.9  1997/03/19  11:05:29  fuhrmann
 % transparency (beta code)
 %
 % Revision 2.8  1997/03/18  17:57:20  fuhrmann
 % wireframe bugs corrected
 % save/restore state filename
 % roty; get/set perspective deleted
 %
 % Revision 2.7  1997/02/28  14:13:26  fuhrmann
 % show_list_toggle -> state
 % state commented
 %
 % Revision 2.6  1997/02/24  20:38:27  fuhrmann
 % list remembering beta code
 %
 % Revision 2.5  1997/02/18  09:37:44  fuhrmann
 % introduced: glrState struct
 %             glrGetWindow (for gui)
 %             glrSetGUI
 % allows modular extension by any gui procedure (JAVA, motif etc.)
 %
 % Revision 2.4  1996/10/11  18:04:53  fuhrmann
 % Save/Restore state exported
 %
 % Revision 2.3  1996/09/23  17:03:30  fuhrmann
 % documentation using xdoc
 %
 % Revision 2.2  1996/09/17  09:31:49  langmach
 % text output from glwin is now available
 %
 % Revision 2.1  1996/09/04  19:53:56  fuhrmann
 % glrXSetPlane
 %
 % Revision 2.0  1996/02/15  19:57:10  fuhrmann
 % First meta-stable distribution
 %
 % Revision 1.4  1996/02/09  16:49:39  fuhrmann
 % - started to comment header
 %
 % Revision 1.3  1996/02/06  09:56:14  fuhrmann
 % glrDump stuff introduced
 %
 % Revision 1.2  1995/11/08  12:20:22  fuhrmann
 % glrSetTitle introduced
 % glRender2 for structure and data pointer usage introduced
 %
 % Revision 1.1  1995/10/20  15:42:38  fuhrmann
 % Initial revision
 %
 %
 */

