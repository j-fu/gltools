#include <GL/gl.h>
#include <math.h>


void arrowhead(int n, double l, double r)
{
  double h,phi,sphi,cphi,lr, ll,rr;
  int i;
  h=6.2832/(double)n;
  lr=sqrt(l*l+r*r);
  ll=l/lr;
  rr=l/lr;
  glBegin(GL_TRIANGLE_FAN);
  glVertex3d(0,0,0);
  for (i=0;i<=n;i++)
    {
      phi=(double)i*h;
      sphi=sin(phi);
      cphi=cos(phi);
      glNormal3d(rr*sphi,rr*cphi,ll);
      glVertex3d(r*sphi,r*cphi,-l);
    }
  glEnd();
}

void arrow(int n, double l, double r)
{
  double h,phi,sphi,cphi,lr, ll,rr;
  int i;
  h=6.2832/(double)n;
  glPushMatrix();
  glTranslated(0,0,l);
  arrowhead(n,0.333*l,1.5*r);
  glPopMatrix();

  glBegin(GL_QUAD_STRIP);
  for (i=0;i<=n;i++)
    {
      phi=(double)i*h;
      sphi=sin(phi);
      cphi=cos(phi);
      glNormal3d(sphi,cphi,0);
      glVertex3d(r*sphi,r*cphi,0.0);
      glVertex3d(0.5*r*sphi,0.5*r*cphi,0.666*l);
    }
  glEnd();
}






