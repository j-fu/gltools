#include "glwin.h"
#include "stdio.h"



int main(int argc, char *argv[])
{
  glWindow w;

  w=glwCreate();

  glBegin(GL_LINE_LOOP);
  glVertex2d(-0.9,-0.9);
  glVertex2d(0.9,-0.9);
  glVertex2d(0.9,0.9);
  glVertex2d(-0.9,0.9);
  glEnd();
  glFlush();
  glwSwapBuffers(w);
  getchar();
  glwDestroy(w);


  w=glwCreate();

  glBegin(GL_LINE_LOOP);
  glVertex2d(-0.9,-0.9);
  glVertex2d(0.9,-0.9);
  glVertex2d(0.9,0.9);
  glVertex2d(-0.9,0.9);
  glEnd();
  glFlush();
  glwSwapBuffers(w);
  getchar();

}
