#include "glwin.h"
#include "stdio.h"

typedef struct mydatastruct
{
  double rotx,roty;
  int  lastx,lasty;
} *mydata;

static void Reshape(glWindow w, void *info,int width, int height)
{
  glViewport(0, 0, (GLint)width, (GLint)height);
}

static GLenum MouseMove(glWindow w, void * data,int x,int y, GLenum mask)
{
  mydata md=(mydata) data;

  md->roty+=(double)(x-md->lastx);
  md->rotx+=(double)(y-md->lasty);
  md->lastx=x;
  md->lasty=y;
  return GL_TRUE;
}

static GLenum MouseDown(glWindow w, void * data,int x,int y, GLenum mask)
{
  mydata md=(mydata) data;
  glwSetMouseMoveFunc(w,MouseMove);
  md->lastx=x;
  md->lasty=y;
  return GL_FALSE;
}
static GLenum MouseUp(glWindow w, void * data,int x,int y, GLenum mask)
{
  glwSetMouseMoveFunc(w,NULL);
  return GL_FALSE;
}

static GLenum Key(glWindow w, void * info,int key, GLenum mask)
{
  if ((key==GLW_Escape)||(key==GLW_q)) glwQuit(w);
  return GL_FALSE;
}


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
}



int main(int argc, char *argv[])
{
  struct mydatastruct mydata;
  glWindow w;

  w=glwCreate();
  glwSetKeyDownFunc(w,Key);
  glwSetMouseDownFunc(w,MouseDown);
  glwSetMouseUpFunc(w,MouseUp);
  glwSetReshapeFunc(w,Reshape);
  glwSetRedrawFunc(w,draw);

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

  glwProcess(w,&mydata);

  glwDestroy(w);
}
