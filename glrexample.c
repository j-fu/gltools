#include <stdlib.h>
#include <GL/gl.h> 
#include "glrnd.h" 

#define NUMBER_OF_POINTS 1000
typedef double Coord[3];
Coord Points[NUMBER_OF_POINTS];
static int quit_application=0; 

static void draw(glRenderer rnd, void * user_data)
{ 
  /*Cast data to actual type. */
  Coord* points=(Coord*)user_data;   
  int i;
  glPointSize(2);
  glBegin(GL_POINTS);
  for(i=0;i<NUMBER_OF_POINTS;i++)
    glVertex3dv(points[i]);
  glEnd();
}

int  do_quit_application(glRenderer rnd, int kbd_mod_mask)
{ 
  quit_application=1; 
  return GL_FALSE;    
}

void main(int argc, char * argv[])
{ 
  glRenderer rnd;
  int i,j;
  
  /* Create renderer in 500x500 window in the 
     upper left corner. */
  rnd=glrCreate("Brownian Motion",0,0,500,500);
  
  /* Set the 3D volume. */
  glrSetVolume(rnd,0.0,1.0,0.0,1.0,0.0,1.0);
  
  /* Register key action callback for quitting the appliacation. */
  glrRegisterKeyAction(rnd,GLW_q,do_quit_application,"q:Quit");
  
  /* Initialize random number generator and calculate
     the initial positions of the points. */
  srand(100);
  for(i=0;i<NUMBER_OF_POINTS;i++)
    for(j=0;j<3;j++)
      Points[i][j]=(double)rand()/(double)RAND_MAX;

  /* Put renderer into application controlled (non-dialog) mode. */
  glrSetDialog(rnd,0);
  
  glLineWidth(2.0);
  while (!quit_application)
    {
      /* Process pending events, call the draw callback once
	 (according to the application controlled mode)
	 and swap buffers. 
	 */
      glRender(rnd,draw,Points);
      
      /* Let the points do some random steps. */
      for(i=0;i<NUMBER_OF_POINTS;i++)
	for(j=0;j<3;j++)
	  Points[i][j]+=0.01*((double)rand()/(double)RAND_MAX-0.5);
    }
  
  /* Free the memory.*/
  glrDestroy(rnd);
}
