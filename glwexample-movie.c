/* Example for creating a movie in an offscreen rendering
context */

#include "glwin.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct mydatastruct
{
  double rotx,roty;
  int  lastx,lasty;
} *mydata;

void draw(glWindow w, void *data)
{
  mydata md=(mydata) data;
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  glTranslatef(0.0,0.0,-5.0);
  glScalef(0.5,0.5,0.5);
  glRotatef(md->rotx,1.0,0.0,0.0);
  glRotatef(md->roty,0.0,1.0,0.0);
 
  glBegin(GL_TRIANGLES);
  glColor3d(1.0,0.0,0.0);
  glVertex3d(-1.0,-1.0,-1.0);
  glVertex3d(1.0,-1.0,-1.0);
  glVertex3d(-1.0,1.0,-1.0);
  
  glColor3d(0.0,1.0,0.0);
  glVertex3d(-1.0,-1.0,-1.0);
  glVertex3d(1.0,-1.0,-1.0);
  glVertex3d(-1.0,-1.0,1.0);
  
  glColor3d(0.0,0.0,1.0);
  glVertex3d(-1.0,-1.0,-1.0);
  glVertex3d(-1.0,1.0,-1.0);
  glVertex3d(-1.0,-1.0,1.0);
  glEnd();
  glFlush();
  glwSwapBuffers(w);
  glwQuit(w);
}



int main(int argc, char *argv[])
{
  int i,nframes;
  
  struct mydatastruct mydata;
  glWindow w;
  glwInitOffscreen();
  if (argc==1) nframes=10;
  else
    nframes=atoi(argv[1]);

  w=glwCreate();
  glwSetRedrawFunc(w,draw);
  glwSetControlMode(w,GLW_APPLICATION_DRIVEN);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glClearColor(0.0,0.0,0.0,1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHT0);
  glMatrixMode (GL_PROJECTION);   
  glLoadIdentity ();  
  glFrustum (-1.0, 1.0, -1.0, 1.0, 4.0,6.0); 
  glMatrixMode (GL_MODELVIEW);  
  glViewport(0, 0, 300.0, 300.0);
  mydata.rotx=0.0;
  mydata.roty=0.0;
  glwStartRecording(w,"test",1);
  for(i=0;i<nframes;i++)
    {
      glwProcess(w,&mydata);
      mydata.rotx+=1.0;
      mydata.roty+=1.0;
    }
  /*  glwStopRecording(w);*/
  glwDestroy(w);
}
