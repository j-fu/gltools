static char *rcsid= "$Id: glrnd.c,v 2.72 2001/05/18 10:54:13 fuhrmann Exp $";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include "glwin.h"
#include "gleps.h"
#include "glrnd.h"
#include "glprimitives.h"

#define SQRT_SQRT_2  1.1892071
#define SQRT_2       1.4142136
#define TENTHROOT_10 1.2589254
#define GLR_MAX_KEY  256

static glrGUI glr_gui=0;

static char plane_names[3]={'x','y','z'};
static int odir[3][2]={ {1,2}, { 0,2}, {0,1}};

void glrSetGUI(glrGUI gui)
{
  glr_gui=gui;
}

struct glRendererStruct
{
  glWindow win;
  void *user_data;
  void *user_data2;
  glrDrawCallback draw;
  glrDrawCallback secondary_callback;
  glrDrawCallback2 draw2;
  glrDrawCallback draw_info;
    
  
  
  glrKeyAction key_action_table[GLR_MAX_KEY];
  char * key_help_table[GLR_MAX_KEY];

  int dialog;
  int next_dialog;
  int switched;
  

  int show_model;
  int lastxpos,lastypos;


  GLint list_base;
  GLint lists[10];
  int list_valid;
  int data_valid;
  int firstcall;

  int dump;
  int dump_w,dump_h,dump_i;
  char dump_name_stub[128];
  char name[128];

  int recording;

  struct glRendererStateStruct state;
  int refresh_user_info;
  int wwidth,wheight;
  int gui_called;
  int render_with_gui;
  char user_info[256];

  char axis_name[4][32];
  int  axis_ntics[4];
  double axis_tics[4][64];
};



glRendererState  glrGetRendererState(glRenderer rnd)
{
  return &rnd->state;
}

/*
static char *   glrGetRendererName(glRenderer rnd)
{
  return rnd->name;
}
*/

glWindow glrGetWindow(glRenderer rnd)
{
  return rnd->win;
}

void *glrGetInfo(glRenderer rnd)
{
  return rnd->user_data;
}


void glrShowModel(glRenderer rnd,int ishow)
{
  if (ishow)
    rnd->show_model=1;
  else
  {
    if (!rnd->state.move_model) rnd->show_model=0;
  }
}

void glrSetInfoCallback(glRenderer rnd,glrDrawCallback info)
{
  rnd->draw_info=info;
}
void glrSetSecondaryCallback(glRenderer rnd,glrDrawCallback info)
{
  rnd->secondary_callback=info;
}
		 

void glrSetApplicationOption(glRenderer rnd,char *key, int val)
{
  int i;
  for(i=0;i<rnd->state.noptions;i++)
    if (strcmp(rnd->state.options[i].key,key)==0)
      {
	rnd->state.options[i].val=val;
	return;
      }
  if (rnd->state.noptions<GLR_MAX_APPLICATION_OPTIONS)
    {
      rnd->state.options[i].val=val;
      strcpy(rnd->state.options[i].key,key);
      rnd->state.noptions++;
    }
}

int glrGetApplicationOption(glRenderer rnd,char *key)
{
  int i;
  for(i=0;i<rnd->state.noptions;i++)
    if (strcmp(rnd->state.options[i].key,key)==0)
      {
	return rnd->state.options[i].val;
      }
  return -1;
}

void glrDumpHelpFile(glRenderer rnd)
{
  int i;
  FILE *helpfile=fopen("glrnd-keys.tex","w");
  fprintf(helpfile,"\\begin{tabular}{|l|}\\hline\n");
  fprintf(helpfile,"glrnd key table, \\verb|$Revision: 2.72 $ $Date: 2001/05/18 10:54:13 $|\\\\\\hline\n");
  for (i=0;i<256;i++)
    {
      if (rnd->key_help_table[i])
	fprintf(helpfile,"%s\\\\\\hline\n",rnd->key_help_table[i]);
    }
  fprintf(helpfile,"\\end{tabular}\n");
  fclose(helpfile);
}

/* initialization, state file handling */
/* ensure that
               1) every item is handled by state file
               2) the state file reading/writing sequence is always right
*/
#define GLR_RESTORE 1
#define GLR_INIT 2
#define GLR_SAVE 3



#define GLR_LTSRC_LIST GLR_MAX_OBJECTS+1
#define GLR_USER_INFO_LIST GLR_MAX_OBJECTS+2
#define GLR_ARROW_LIST GLR_MAX_OBJECTS+3
#define GLR_N_LISTS    GLR_MAX_OBJECTS+4

static float lt_position[] = {0.0, 10, 1,  0.0};

static void put_get_init_int(int mode, FILE *f, int *x, int val)
{
  switch (mode)
    {
    case GLR_RESTORE: fscanf(f,"%d\n",x); break;
    case GLR_SAVE: fprintf(f,"%d\n",*x);break;
    case GLR_INIT: *x=val;
    }
}

static void put_get_init_char(int mode, FILE *f, char *x, char val)
{
  switch (mode)
    {
    case GLR_RESTORE: fscanf(f,"%c\n",x); break;
    case GLR_SAVE: fprintf(f,"%c\n",*x);break;
    case GLR_INIT: *x=val;
    }
}

static void put_get_init_real(int mode, FILE *f, double *x, double val)
{
  float xx;
  switch (mode)
    {
    case GLR_RESTORE: fscanf(f,"%e\n",&xx);*x=xx; break;
    case GLR_SAVE: fprintf(f,"%e\n",*x);break;
    case GLR_INIT: *x=val;
    }
}


static void put_get_init(glRenderer rnd,FILE *state_file, int mode)
{

  put_get_init_real(mode,state_file,&rnd->state.scale[0],1.0);
  put_get_init_real(mode,state_file,&rnd->state.scale[1],1.0);
  put_get_init_real(mode,state_file,&rnd->state.scale[2],1.0);
  put_get_init_real(mode,state_file,&rnd->state.rotx,0.0);
  put_get_init_real(mode,state_file,&rnd->state.roty,0.0);
  put_get_init_real(mode,state_file,&rnd->state.rotz,0.0);
  put_get_init_real(mode,state_file,&rnd->state.tranx,0.0);
  put_get_init_real(mode,state_file,&rnd->state.trany,0.0);
  put_get_init_real(mode,state_file,&rnd->state.tranz,0.0);
  put_get_init_real(mode,state_file,&rnd->state.vscale,1.0);
  put_get_init_real(mode,state_file,&rnd->state.zoom,1.0);
  put_get_init_real(mode,state_file,&rnd->state.ctrl_prm,0.0);
  put_get_init_real(mode,state_file,&rnd->state.ctrl_fac,1.0);
  put_get_init_int(mode,state_file,&rnd->state.wireframe,0);
  put_get_init_int(mode,state_file,&rnd->state.move_model,0);
  put_get_init_int(mode,state_file,&rnd->state.show_frame,1);
  put_get_init_real(mode,state_file,&rnd->state.asp,1.0);
  put_get_init_int(mode,state_file,&rnd->state.gouraud_shading,0);
  put_get_init_real(mode,state_file,&rnd->state.sensitivity,0.5);
  put_get_init_int(mode,state_file,&rnd->state.bg_black,1);
  put_get_init_real(mode,state_file,&rnd->state.ltx,0.0);
  put_get_init_real(mode,state_file,&rnd->state.lty,0.0);
  put_get_init_real(mode,state_file,&rnd->state.ltz,0.0);
  put_get_init_real(mode,state_file,&rnd->state.plane_d[0],-0.000001);
  put_get_init_real(mode,state_file,&rnd->state.plane_d[1],-0.000001);
  put_get_init_real(mode,state_file,&rnd->state.plane_d[2],-0.000001);
  put_get_init_int(mode,state_file,&rnd->state.plane_dir,2);
  put_get_init_int(mode,state_file,&rnd->state.level_surf,0);
  put_get_init_real(mode,state_file,&rnd->state.level,0.0);
  put_get_init_int(mode,state_file,&rnd->state.ortho,0);
  put_get_init_int(mode,state_file,&rnd->state.what_done,0);
  put_get_init_real(mode,state_file,&rnd->state.isoline_distance,0.05);
  put_get_init_int(mode,state_file,&rnd->state.isoline_mode,0);
  put_get_init_int(mode,state_file,&rnd->state.transparency,0);
  put_get_init_int(mode,state_file,&rnd->state.show_info,1);

  if (mode!=GLR_SAVE)
    {
      rnd->list_valid=0;
    }
}


void glrReset(glRenderer rnd)
{
  put_get_init(rnd,NULL,GLR_INIT);
}


void glrSaveState(glRenderer rnd,char *state_file_name)
{
  char xstate_file_name[128];
  FILE *state_file;
  if (state_file_name==NULL)
    {
      state_file_name=xstate_file_name;
      sprintf(state_file_name,".%s-rndstate",rnd->name);
    }
  
  state_file=fopen(state_file_name,"w");

  if (state_file==NULL)
    {
      fprintf(stderr,"glrnd: cannot open state file %s\n",state_file_name);
      return;
    }
  fprintf(state_file,"$Revision: 2.72 $ of glrnd.c\n");
  put_get_init(rnd,state_file,GLR_SAVE);
  fclose(state_file);
  fprintf(stderr,"glrnd: state saved into %s\n",state_file_name);
}

void glrRestoreState(glRenderer rnd,char *state_file_name)
{
  char xstate_file_name[128];
  char version[64];

  FILE *state_file;

  if (state_file_name==NULL)
    {
      state_file_name=xstate_file_name;
      sprintf(state_file_name,".%s-rndstate",rnd->name);
    }
  
  state_file=fopen(state_file_name,"r");
  if (state_file==NULL) return;
  fgets(version,64,state_file);
  if (strcmp("$Revision: 2.72 $ of glrnd.c\n",version)!=0)
    {
      fprintf(stderr,"Wrong version of state file %s\n",state_file_name);
      fclose(state_file);
      return;
    }
  put_get_init(rnd,state_file,GLR_RESTORE);
  fclose(state_file);
  fprintf(stderr,"glrnd: state restored from %s\n",state_file_name);
}


void glrDefaultInfoCallback(glRenderer rnd,void *thrash)
{
  double ypos,dypos;
  
  if (rnd->state.bg_black)
    glColor3f(1.0,1.0,1.0);
  else
    glColor3f(0.0,0.0,0.0);
  
  ypos=0.95;
  dypos=-0.05;
  if (rnd->state.what_done)
    {
      glRasterPos2d(0.01,ypos);
      switch(rnd->state.what_done)
	{
	case GLR_ROTATE:
	  glwPrintf(rnd->win,"rotation: x=%.2f y=%.2f z=%.2f",rnd->state.rotx,rnd->state.roty,rnd->state.rotz);
	  break;
	  
	case GLR_TRANSLATE:
	  glwPrintf(rnd->win,"translation: x=%.2f y=%.2f",rnd->state.tranx,rnd->state.trany);
	  break;
	  
	case GLR_LIGHT:
	  glwPrintf(rnd->win,"light position: x=%.2f y=%.2f",rnd->state.ltx,rnd->state.lty);
	  break;
	  
	case GLR_ISOLEVEL:
	  glwPrintf(rnd->win,"isosurface level: %.4g",rnd->state.level);
	  break;
	  
	case GLR_PLANE_ASPECT:
	case GLR_INPLANE_ASPECT:
	  glwPrintf(rnd->win,"aspect ratio: x=%.2g y=%.2g, z=%.2g",
		    rnd->state.scale[0],
		    rnd->state.scale[1],
		    rnd->state.scale[2]);
	  break;
	  
	case GLR_PLANE:
	case GLR_INPLANE:
	  glwPrintf(rnd->win,"%c%c-plane, x=%.4g  y=%.4g z=%.4g",
		    plane_names[odir[rnd->state.plane_dir][0]],
		    plane_names[odir[rnd->state.plane_dir][1]],
		    -rnd->state.plane_d[0],
		    -rnd->state.plane_d[1],
		    -rnd->state.plane_d[2]);
	  break;
      
	}
      ypos+=dypos;
    }  
  if(rnd->user_info)
    {
      glRasterPos2d(0.01,ypos);
      ypos+=dypos;
      glwPrintf(rnd->win,rnd->user_info);
    }

}
  





static void glrChangeState(glRenderer rnd, double dx, double dy)
{
  int dir;
  switch(rnd->state.what_done)
    {
    case GLR_ROTATE:
      rnd->state.rotz+=0.5*dx*rnd->state.sensitivity;
      rnd->state.rotx+=0.5*dy*rnd->state.sensitivity;
      break;

    case GLR_TRANSLATE:
      rnd->state.tranx+=0.01*dx*rnd->state.sensitivity;
      rnd->state.trany-=0.01*dy*rnd->state.sensitivity;
      break;

    case GLR_LIGHT:
      rnd->state.lty+=dx*rnd->state.sensitivity;
      rnd->state.ltx+=dy*rnd->state.sensitivity;
       break;

    case GLR_ISOLEVEL:
      rnd->state.level-=0.05*dy*rnd->state.sensitivity;
      if (rnd->state.level<rnd->state.fmin) rnd->state.level=rnd->state.fmin;
      if (rnd->state.level>rnd->state.fmax) rnd->state.level=rnd->state.fmax;
      rnd->list_valid=0;
      break;

    case GLR_PLANE_ASPECT:
      dir=rnd->state.plane_dir;
      rnd->state.scale[dir]/=pow(2.0,dy*rnd->state.sensitivity);
      break;

    case GLR_INPLANE_ASPECT:
      dir=odir[rnd->state.plane_dir][0];
      rnd->state.scale[dir]/=pow(2.0,dx*rnd->state.sensitivity);
      dir=odir[rnd->state.plane_dir][1];
      rnd->state.scale[dir]/=pow(2.0,dy*rnd->state.sensitivity);
      break;


    case GLR_PLANE:
      if (rnd->state.spacedim==2) break;
      dir=rnd->state.plane_dir;
      rnd->state.plane_d[dir]+= 0.1*dy*rnd->state.sensitivity*(rnd->state.max[dir]-rnd->state.min[dir]);
      if ( -rnd->state.plane_d[dir] < rnd->state.min[dir]) rnd->state.plane_d[dir]= -rnd->state.min[dir];
      if ( -rnd->state.plane_d[dir] > rnd->state.max[dir]) rnd->state.plane_d[dir]= -rnd->state.max[dir];
      rnd->list_valid=0;
      break;

    case GLR_INPLANE:
      dir=odir[rnd->state.plane_dir][0];
      rnd->state.plane_d[dir]-= 0.1*dx*rnd->state.sensitivity*(rnd->state.max[dir]-rnd->state.min[dir]);
      if ( -rnd->state.plane_d[dir] < rnd->state.min[dir]) rnd->state.plane_d[dir]= -rnd->state.min[dir];
      if ( -rnd->state.plane_d[dir] > rnd->state.max[dir]) rnd->state.plane_d[dir]= -rnd->state.max[dir];

      dir=odir[rnd->state.plane_dir][1];
      rnd->state.plane_d[dir]+= 0.1*dy*rnd->state.sensitivity*(rnd->state.max[dir]-rnd->state.min[dir]);
      if ( -rnd->state.plane_d[dir] < rnd->state.min[dir]) rnd->state.plane_d[dir]= -rnd->state.min[dir];
      if ( -rnd->state.plane_d[dir] > rnd->state.max[dir]) rnd->state.plane_d[dir]= -rnd->state.max[dir];

      rnd->list_valid=0;

      break;
    }
}






void glrRegisterKeyAction(glRenderer rnd, int key, glrKeyAction action, char *help)
{
  if (key>=GLR_MAX_KEY)
    {
      fprintf(stderr,"glrnd: unable to register keysym %d>%d\n",
	      key,GLR_MAX_KEY);
      fprintf(stderr,"\"%s\"\n",help);
      
    }
  else
    {
      rnd->key_action_table[key]=action;
      if (help!=NULL)
	rnd->key_help_table[key]=help;
    }
}





static void glrRememberObject(glRenderer rnd, int ilist)
{
  
      GLint xlist;
      int i;
      if (ilist==0)
	{
	  for(i=1;i<=9;i++)
	    {
	      glNewList(rnd->list_base+rnd->lists[i],GL_COMPILE);
	      glEndList();
	    }
	}
      else
	{
	  printf("list %d\n",ilist);
	  xlist=rnd->lists[0];
	  rnd->lists[0]=rnd->lists[ilist];
	  rnd->lists[ilist]=xlist;
	  rnd->list_valid=0;
	}
}

static GLenum glrKey(glWindow w, void * info,int key, GLenum mask)
{
  glRenderer rnd=(glRenderer)info;
  switch(key)
    {
    case GLW_1: glrRememberObject(rnd,1);break;
    case GLW_2: glrRememberObject(rnd,2);break;
    case GLW_3: glrRememberObject(rnd,3);break;
    case GLW_4: glrRememberObject(rnd,4);break;
    case GLW_5: glrRememberObject(rnd,5);break;
    case GLW_6: glrRememberObject(rnd,6);break;
    case GLW_7: glrRememberObject(rnd,7);break;
    case GLW_8: glrRememberObject(rnd,8);break;
    case GLW_9: glrRememberObject(rnd,9);break;
    case GLW_0: glrRememberObject(rnd,0);break;
    default:
      if ((key<GLR_MAX_KEY) && (rnd->key_action_table[key]))
	return (GLenum) rnd->key_action_table[key](rnd,mask);
      else
	return GL_FALSE;
    }
  return GL_TRUE;
}

static int glrKeyAction_c(glRenderer rnd, int mask)
{
  int i;
  /* this is dirty...*/
  rnd->state.show_object[0]=!rnd->state.show_object[0];
  for(i=1;i<=GLR_MAX_OBJECTS;i++) 
    rnd->state.show_object[i]=rnd->state.show_object[0];
  return GL_TRUE;
}


static int glrKeyAction_D(glRenderer rnd, int mask)
{
  glwGrabPPMAndPrint(rnd->win);
  if (rnd->dialog) fprintf(stderr,"glrnd: frame printed\n");
  return GL_FALSE;
}

static int glrKeyAction_d(glRenderer rnd, int mask)
{
  char fname[128];
  FILE *f;
  sprintf(fname,"%s-%05d.ppm",rnd->dump_name_stub,rnd->dump_i);
  f=fopen(fname,"w");
  glwSwapBuffers(rnd->win);/* Is this machine dependent ??? */
  glwGrabPPM(rnd->win,f);
  glwSwapBuffers(rnd->win);
  fclose(f);
  if (rnd->dialog) fprintf(stderr,"glrnd: frame dumped to %s\n",fname);
  rnd->dump_i++;
  return GL_FALSE;
}

static int glrKeyAction_p(glRenderer rnd, int mask)
{
  char fname[128];
  FILE *f;
  sprintf(fname,"%s-%05d.eps",rnd->dump_name_stub,rnd->dump_i);
  f=fopen(fname,"w");
  rnd->list_valid=0;
  glepsDumpSorted(rnd->win,f,0);
  fclose(f);
  if (rnd->dialog) fprintf(stderr,"glrnd: frame dumped to %s\n",fname);
  rnd->dump_i++;
  rnd->list_valid=0;
  return GL_TRUE;
}


static int glrKeyAction_v(glRenderer rnd, int mask)
{
  if (rnd->state.vscale==0.0) 
    rnd->state.vscale=0.5;
  else   if (rnd->state.vscale==0.5) 
    rnd->state.vscale=1.0;
  else
    rnd->state.vscale=0.0;
  return GL_TRUE;
}

static int glrKeyAction_V(glRenderer rnd, int mask)
{
  if (!rnd->recording)
    {
      glwStartMPEGRecording(rnd->win,rnd->dump_name_stub,0);
      if (rnd->dialog) fprintf(stderr,"glrnd: recording to %s.mpg started\n",rnd->dump_name_stub);
      rnd->recording=1;
    }
  else
    {
      glwStopMPEGRecording(rnd->win);
      if (rnd->dialog) fprintf(stderr,"glrnd: recording to %s.mpg stopped\n",rnd->dump_name_stub);
      rnd->recording=0;
    }
  return GL_FALSE;
}




static int  glrKeyAction_mode_control(glRenderer rnd, int mask)
{
  rnd->dialog=!rnd->dialog;
  if (mask & GLW_SHIFT ) rnd->next_dialog=1;
  if (rnd->dialog==1) {rnd->switched=1; return GL_FALSE;} else 
    return GL_TRUE;
}

static int  glrKeyAction_continue(glRenderer rnd, int mask)
{
  rnd->dialog=0;
  if (mask & GLW_SHIFT ) rnd->next_dialog=1;
    return GL_TRUE;
}

static int  glrKeyAction_stop(glRenderer rnd, int mask)
{
  rnd->dialog=1;
  rnd->switched=1;
  return GL_FALSE;
}


static int glrKeyAction_a(glRenderer rnd, int mask)
{
  if (glr_gui&&(!rnd->gui_called)) 
  {
    rnd->gui_called=1;
    glr_gui(rnd);
    rnd->gui_called=0;
    return GL_TRUE;
  }
    return GL_FALSE;
}

/*
static int glrKeyAction_t(glRenderer rnd, int mask)
{
  rnd->state.transparency=!rnd->state.transparency;
  if (rnd->state.transparency)
    {
      glEnable(GL_BLEND);
      glBlendFunc(GL_ONE,GL_ONE);
      
    }
  else
      glDisable(GL_BLEND);
  rnd->list_valid=0;
  return GL_TRUE;
}
*/

static int glrKeyAction_g(glRenderer rnd, int mask)
{
  rnd->state.gouraud_shading=!rnd->state.gouraud_shading;
  return GL_TRUE;
}

static int glrKeyAction_F(glRenderer rnd, int mask)
{
  rnd->state.show_frame=(rnd->state.show_frame+1 )%3;
  return GL_TRUE;
}
static int glrKeyAction_B(glRenderer rnd, int mask)
{
  rnd->state.bg_black=!rnd->state.bg_black;
   return GL_TRUE;
}

static int glrKeyAction_m(glRenderer rnd, int mask)
{
  rnd->state.move_model=!rnd->state.move_model;
  return GL_FALSE;
}


static int glrKeyAction_l(glRenderer rnd, int mask)
{
  rnd->state.level_surf=!rnd->state.level_surf;
  if (rnd->state.level_surf) rnd->state.isoline_mode=0;
  rnd->list_valid=0;
  return GL_TRUE;
}

static int glrKeyAction_i(glRenderer rnd, int mask)
{
  rnd->state.isoline_mode=!rnd->state.isoline_mode;
  if (rnd->state.isoline_mode) rnd->state.level_surf=0;
  rnd->list_valid=0;
  return GL_TRUE;
}


static int glrKeyAction_I(glRenderer rnd, int mask)
{
  if (rnd->state.isoline_distance==0.1)
    rnd->state.isoline_distance=0.05;
  else if (rnd->state.isoline_distance==0.05)
    rnd->state.isoline_distance=0.025;
  else
    rnd->state.isoline_distance=0.1;
  rnd->list_valid=0;
  return GL_TRUE;
}



static int glrKeyAction_inc_mouse(glRenderer rnd, int mask)
{
  rnd->state.sensitivity*=SQRT_2;
  return GL_FALSE;
}
static int glrKeyAction_dec_mouse(glRenderer rnd, int mask)
{
  rnd->state.sensitivity/=SQRT_2;
  return GL_FALSE;
}

void glrXSetPlane(glRenderer rnd, int dir, double val)
{

  rnd->state.plane_dir=dir;
  rnd->state.plane_d[dir]=val;
  if ( -rnd->state.plane_d[dir] < rnd->state.min[dir]) rnd->state.plane_d[dir]= -rnd->state.min[dir];
  if ( -rnd->state.plane_d[dir] > rnd->state.max[dir]) rnd->state.plane_d[dir]= -rnd->state.max[dir];
}

static int glrKeyAction_x(glRenderer rnd, int mask)
{
  if (rnd->state.spacedim==2) return GL_FALSE;
  rnd->list_valid=0;
  glrXSetPlane(rnd,0,-rnd->state.plane_d[0]);
  return GL_TRUE;
}


static int glrKeyAction_y(glRenderer rnd, int mask)
{
  if (rnd->state.spacedim==2) return GL_FALSE;
  rnd->list_valid=0;
  glrXSetPlane(rnd,1,-rnd->state.plane_d[1]);
  return GL_TRUE;
}

static int glrKeyAction_z(glRenderer rnd, int mask)
{
  if (rnd->state.spacedim==2) return GL_FALSE;
  rnd->list_valid=0;
  glrXSetPlane(rnd,2,-rnd->state.plane_d[2]);
  return GL_TRUE;
}


static int glrKeyAction_zoom_in(glRenderer rnd, int mask)
{
  rnd->state.zoom*=1.1;
  return GL_TRUE;
}
static int glrKeyAction_zoom_out(glRenderer rnd, int mask)
{
  rnd->state.zoom/=1.1;
  return GL_TRUE;
}

static int glrKeyAction_increase_ctrl_prm(glRenderer rnd, int mask)
{
  rnd->state.ctrl_prm+=0.1;
/*   printf("ctrl_prm %g\n",rnd->state.ctrl_prm); */
  return GL_TRUE;
}

static int glrKeyAction_decrease_ctrl_prm(glRenderer rnd, int mask)
{
  rnd->state.ctrl_prm-=0.1;
/*   printf("ctrl_prm %g\n",rnd->state.ctrl_prm); */
  return GL_TRUE;
}

static int glrKeyAction_increase_ctrl_fac(glRenderer rnd, int mask)
{
  rnd->state.ctrl_fac*=1.1;
/*   printf("ctrl_fac %g\n",rnd->state.ctrl_fac); */
  return GL_TRUE;
}

static int glrKeyAction_decrease_ctrl_fac(glRenderer rnd, int mask)
{
  rnd->state.ctrl_fac/=1.1;
/*   printf("ctrl_fac %g\n",rnd->state.ctrl_fac); */
  return GL_TRUE;
}


static int glrKeyAction_r(glRenderer rnd, int mask)
{
  glrReset(rnd);
  glrRestoreState(rnd,NULL);
  return GL_TRUE;
}

static int glrKeyAction_R(glRenderer rnd, int mask)
{
  glrReset(rnd);
  return GL_TRUE;
}


static int glrKeyAction_S(glRenderer rnd, int mask)
{
  glrSaveState(rnd,NULL);
  return GL_FALSE;
}


static int glrKeyAction_help(glRenderer rnd, int mask)
{
  int i;
  if (mask&GLW_CONTROL)
    {
      glrDumpHelpFile(rnd);
    }
  else
    {
      fprintf(stderr,"-------------------------------------\n");
      fprintf(stderr,"---      GLRender Key Table       ---\n");
      for (i=0;i<256;i++)
	{
	  if (rnd->key_help_table[i])
	    fprintf(stderr,"%s\n",rnd->key_help_table[i]);
	}
      fprintf(stderr,"-------------------------------------\n");
      return GL_FALSE;
    }
  return GL_FALSE;
}





static int glrKeyAction_left(glRenderer rnd, int mask)
{
  glrChangeState(rnd, -10, 0);


  rnd->dialog=0;
  rnd->next_dialog=1;

  return GL_TRUE;
}

static int glrKeyAction_right(glRenderer rnd, int mask)
{
  glrChangeState(rnd, 5, 0);
  return GL_TRUE;
}

static int glrKeyAction_up(glRenderer rnd, int mask)
{
  glrChangeState(rnd, 0, -5);
  return GL_TRUE;
}

static int glrKeyAction_down(glRenderer rnd, int mask)
{
  glrChangeState(rnd, 0, 5);
  return GL_TRUE;
}

static GLenum glrMouse(glWindow w, void * info,int x,int y, GLenum mask)
{
  glRenderer rnd=(glRenderer)info;
  if (mask & GLW_CONTROL)
    glrChangeState(rnd,0.1*(double)(x-rnd->lastxpos),0.1*(double)(y-rnd->lastypos));
  else
    glrChangeState(rnd,(double)(x-rnd->lastxpos),(double)(y-rnd->lastypos));

  rnd->lastxpos=x;
  rnd->lastypos=y;
  return GL_TRUE;
}


static int glrKeyAction_next(glRenderer rnd, int mask)
{
  rnd->state.what_done = 1+ (rnd->state.what_done+1) % (GLR_N_DO-1);
  return GL_TRUE;
}

static int glrKeyAction_prev(glRenderer rnd, int mask)
{
  rnd->state.what_done =1+ (rnd->state.what_done-1) % (GLR_N_DO-1);
  return GL_TRUE;
}


static int glrKeyAction_tab(glRenderer rnd, int mask)
{
  rnd->state.show_info=!(rnd->state.show_info);
  return GL_TRUE;
}

static int glrKeyAction_O(glRenderer rnd, int mask)
{
  rnd->state.ortho=!rnd->state.ortho;
  if (!rnd->state.ortho)
    if (rnd->state.zoom<0.0) rnd->state.zoom=2.0;
  return GL_TRUE;
}


static int glrKeyAction_wireframe(glRenderer rnd, int mask)
{
   if(rnd->state.wireframe)
     {
      glEnable(GL_LIGHTING);
      glEnable(GL_COLOR_MATERIAL);
      glEnable(GL_DEPTH_TEST);
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      rnd->state.wireframe=0;
     }
   else  
     {
       rnd->state.wireframe=1;
       glDisable(GL_LIGHTING);
       glDisable(GL_COLOR_MATERIAL);
       glDisable(GL_DEPTH_TEST);
       glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
     }
   rnd->list_valid=0;
    return GL_TRUE;
}

static int glrKeyAction_info(glRenderer rnd, int mask)
{
   fprintf(stderr,"-------------------------------------\n");
   fprintf(stderr,"---     GL State information      ---\n");
   fprintf(stderr,"-------------------------------------\n");

   if (rnd->dialog)
     fprintf(stderr,"state control: user controlled\n");
   else
     fprintf(stderr,"state control: application controlled\n");

   fprintf(stderr,"box:\n");
   fprintf(stderr,"  xmin= %+8.2e, xmax= %+8.2e\n",rnd->state.min[0],rnd->state.max[0]);
   fprintf(stderr,"  ymin= %+8.2e, ymax= %+8.2e\n",rnd->state.min[1],rnd->state.max[1]);
   fprintf(stderr,"  zmin= %+8.2e, zmax= %+8.2e\n",rnd->state.min[2],rnd->state.max[2]);

   fprintf(stderr,"object rotation:    rotx= %+8.2e,  roty=%+8.2e rotz= %+8.2e\n",
	   rnd->state.rotx, rnd->state.roty,rnd->state.rotz);
   fprintf(stderr,"object translation: x= %+8.2e, y= %+8.2e, z= %+8.2e\n",
	   rnd->state.tranx, rnd->state.trany, rnd->state.tranz);
   fprintf(stderr,"object distance:    %+8.2e\n", rnd->state.zoom);
   fprintf(stderr,"aspect ratio:       %+8.2e\n", rnd->state.asp);
   fprintf(stderr,"light position:     x= %+8.2e, y= %+8.2e, z= %+8.2e\n",
	   rnd->state.ltx, rnd->state.lty, rnd->state.ltz);

   fprintf(stderr,"sensitivity:        %+8.2e\n", rnd->state.sensitivity);

   if (rnd->state.level_surf)
     fprintf(stderr,"show isosurface:  level= %+8.2e\n",rnd->state.level);
   else  
     {
       /*
     fprintf(stderr,"show values on a plane:\n");
      fprintf(stderr,"  orthogonal with respect to the axis:\n");
      fprintf(stderr,"  a= %+8.2e, b= %+8.2e, c= %+8.2e\n",
	      rnd->state.plane_a,rnd->state.plane_b, rnd->state.plane_c);
      fprintf(stderr,"  shift= %+8.2e\n", -rnd->state.plane_d);
      fprintf(stderr,"  pmin= %+8.2e, pmax= %+8.2e\n",rnd->p_min, rnd->p_max);
      
      fprintf(stderr,"scale factor for values: %+8.2e\n",rnd->state.vscale);
      fprintf(stderr,"isoline distance: %+8.2e\n",rnd->state.isoline_distance);
      */
     }
   
   fprintf(stderr,"---\n");
   if (rnd->state.wireframe)  fprintf(stderr,"show edges, wireframe\n");
   else  {
      if (rnd->state.gouraud_shading)
      	fprintf(stderr,"show faces, gouraud shading\n");
      else
      	fprintf(stderr,"show faces, flat shading\n");
   }
   if (rnd->state.show_frame)
	  fprintf(stderr,"show bounding box(frame)\n");

   fprintf(stderr,"-------------------------------------\n");
   return GL_FALSE;
}




/****************************************************************************/

static void glrGenLightSource(glRenderer rnd)
{
  double d=0.02,t;
  double x=lt_position[0],y=lt_position[1],z=lt_position[2];
  glNewList(rnd->list_base+GLR_LTSRC_LIST,GL_COMPILE);
  glBegin(GL_POLYGON);
  for (t=0;t<1.0;t+=0.05)
	{
	  glVertex3d(x+d*sin(6.2832*t),y+d*cos(6.2832*t),z);
	}
  glEnd();
  glEndList();
}


static void glrDrawFrame(glRenderer rnd)
{
  int i;
  double zmax,eps,ticlen;
  double vscale=1.0;
  if (rnd->state.fmax>rnd->state.fmin) vscale=rnd->state.vscale/(rnd->state.fmax-rnd->state.fmin);
  if (rnd->state.spacedim==2)
    zmax=rnd->state.min[2]+rnd->state.vscale;
  else
    zmax=rnd->state.max[2];
  
   if (rnd->state.bg_black)
    glColor3f(1.0,1.0,1.0);
  else
    glColor3f(0.0,0.0,0.0);
   /*glwSetFontSize(rnd->win,0.035);*/
   
   {
     double dx=rnd->state.max[0]-rnd->state.min[0];
     double dy=rnd->state.max[1]-rnd->state.min[1];
     double dz=rnd->state.max[2]-rnd->state.min[2];
     ticlen=0;
     if (dx>ticlen) ticlen=dx;
     if (dy>ticlen) ticlen=dy;
     if (rnd->state.spacedim>2)
       if (dz>ticlen) ticlen=dz;
     ticlen=0.025*ticlen;
   }
   
  glBegin(GL_LINES);
  glVertex3f(rnd->state.min[0],rnd->state.min[1],rnd->state.min[2]);
  glVertex3f(rnd->state.max[0],rnd->state.min[1],rnd->state.min[2]);
  glVertex3f(rnd->state.min[0],rnd->state.min[1],rnd->state.min[2]);
  glVertex3f(rnd->state.min[0],rnd->state.max[1],rnd->state.min[2]);
  glVertex3f(rnd->state.min[0],rnd->state.min[1],rnd->state.min[2]);
  glVertex3f(rnd->state.min[0],rnd->state.min[1],           zmax);
  glEnd();

  
  if (rnd->axis_name[0][0]!='\0')
    {
      glRasterPos3f(rnd->state.max[0]+2*ticlen,rnd->state.min[1]-ticlen,rnd->state.min[2]);
      glwPrintf(rnd->win,rnd->axis_name[0]);
    }

  if (rnd->axis_ntics[0]!='\0' && rnd->state.show_frame==2) 
    {
      glBegin(GL_LINES);
      for (i=0;i<rnd->axis_ntics[0];i++)
	{
	  glVertex3d(rnd->axis_tics[0][i],0,0);
	  glVertex3d(rnd->axis_tics[0][i],-ticlen,-ticlen);
	}
      glEnd();
      for (i=0;i<rnd->axis_ntics[0];i++)
	{
	  glRasterPos3d(rnd->axis_tics[0][i]-ticlen,-3*ticlen,-3*ticlen);
	  glwPrintf(rnd->win,"%.3g",rnd->axis_tics[0][i]);
	}
    }
  
  if (rnd->axis_name[1][0]!='\0')
    {
      glRasterPos3f(rnd->state.min[0]-ticlen,rnd->state.max[1]+2*ticlen,rnd->state.min[2]);
      glwPrintf(rnd->win,rnd->axis_name[1]);
    }
  
  if (rnd->axis_ntics[1]!='\0' && rnd->state.show_frame==2)
    {
      glBegin(GL_LINES);
      for (i=0;i<rnd->axis_ntics[1];i++)
	{
	  glVertex3d(0,rnd->axis_tics[1][i],0);
	  glVertex3d(-ticlen,rnd->axis_tics[1][i],-ticlen);
	}
      glEnd();
     for (i=0;i<rnd->axis_ntics[1];i++)
	{
	  glRasterPos3d(-4*ticlen,rnd->axis_tics[1][i]-0.5*ticlen,-2*ticlen);
	  glwPrintf(rnd->win,"%.3g",rnd->axis_tics[1][i]);
	}
    }


  if(rnd->state.spacedim==2)
    {
      if ((rnd->axis_name[3][0]!='\0')&& rnd->state.vscale !=0.0)
	{
	  glRasterPos3f(rnd->state.min[0],rnd->state.min[1],zmax+ticlen);
	  glwPrintf(rnd->win,rnd->axis_name[3]);
	}

  if (rnd->axis_ntics[3]!='\0'&&rnd->state.vscale>0 && rnd->state.show_frame==2)
    {
      glBegin(GL_LINES);
      for (i=0;i<rnd->axis_ntics[3];i++)
	{
	  glVertex3d(0,0,vscale*(rnd->axis_tics[3][i]-rnd->state.fmin));
	  glVertex3d(-ticlen,-ticlen,vscale*(rnd->axis_tics[3][i]-rnd->state.fmin));
	}
      glEnd();
     for (i=0;i<rnd->axis_ntics[3];i++)
	{
	  glRasterPos3d(-3*ticlen,-3*ticlen,vscale*(rnd->axis_tics[3][i]-rnd->state.fmin)-0.5*ticlen);
	  glwPrintf(rnd->win,"%.3g",rnd->axis_tics[3][i]);
	}
    }


    }
  else
    if(rnd->state.spacedim==3)
      {
	if (rnd->axis_name[2][0]!='\0')
	{
	  glRasterPos3f(rnd->state.min[0],rnd->state.min[1],zmax+ticlen);
	  glwPrintf(rnd->win,rnd->axis_name[2]);
	}
	if (rnd->axis_ntics[2]!='\0'&&rnd->state.vscale>0 && rnd->state.show_frame==2)
	  {
	    glBegin(GL_LINES);
	    for (i=0;i<rnd->axis_ntics[2];i++)
	      {
		glVertex3d(0,0,rnd->axis_tics[2][i]);
		glVertex3d(-ticlen,-ticlen,rnd->axis_tics[2][i]);
	      }
	    glEnd();
	    for (i=0;i<rnd->axis_ntics[2];i++)
	      {
		glRasterPos3d(-3*ticlen,-3*ticlen,rnd->axis_tics[2][i]-0.5*ticlen);
		glwPrintf(rnd->win,"%.3g",rnd->axis_tics[2][i]);
	      }
	  }

    }

  glColor3f(0.5,0.5,0.5);

  if(rnd->state.spacedim==3 && !rnd->state.level_surf) 
    switch(rnd->state.plane_dir)
      {
	
      case 2:
	eps=1.0e-2*(rnd->state.max[2]-rnd->state.min[2]);
	glBegin(GL_LINE_LOOP);
	glVertex3f(rnd->state.min[0],rnd->state.min[1],-rnd->state.plane_d[2]+eps);
	glVertex3f(rnd->state.max[0],rnd->state.min[1],-rnd->state.plane_d[2]+eps);
	glVertex3f(rnd->state.max[0],rnd->state.max[1],-rnd->state.plane_d[2]+eps);
	glVertex3f(rnd->state.min[0],rnd->state.max[1],-rnd->state.plane_d[2]+eps);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(rnd->state.min[0],-rnd->state.plane_d[1],-rnd->state.plane_d[2]+eps);
	glVertex3f(rnd->state.max[0],-rnd->state.plane_d[1],-rnd->state.plane_d[2]+eps);
	glVertex3f(-rnd->state.plane_d[0],rnd->state.min[1],-rnd->state.plane_d[2]+eps);
	glVertex3f(-rnd->state.plane_d[0],rnd->state.max[1],-rnd->state.plane_d[2]+eps);
	glEnd();
	if (rnd->state.vscale!=0.0)
	  {
	    glBegin(GL_LINES);
	    glVertex3f(rnd->state.max[0],rnd->state.max[1],-rnd->state.plane_d[2]);
	    glVertex3f(rnd->state.max[0],rnd->state.max[1],-rnd->state.plane_d[2]+rnd->state.vscale);
	    glEnd();
	    if (rnd->axis_name[3][0]!='\0')
	      {
		glRasterPos3f(rnd->state.max[0],rnd->state.max[1],-rnd->state.plane_d[2]+rnd->state.vscale+ticlen);
		glwPrintf(rnd->win,rnd->axis_name[3]);
	      }
	    if (rnd->axis_ntics[3]!=0 && rnd->state.show_frame==2)
	      {
		glBegin(GL_LINES);
		for (i=0;i<rnd->axis_ntics[3];i++)
		  {
		    glVertex3d(rnd->state.max[0],rnd->state.max[1],
			       -rnd->state.plane_d[2]+vscale*(rnd->axis_tics[3][i]-rnd->state.fmin));
		    glVertex3d(rnd->state.max[0]+ticlen,
			       rnd->state.max[1]+ticlen,
			       -rnd->state.plane_d[2]+vscale*(rnd->axis_tics[3][i]-rnd->state.fmin));
		  }
		glEnd();
		for (i=0;i<rnd->axis_ntics[3];i++)
		  {
		    glRasterPos3d(rnd->state.max[0]+2*ticlen,
				  rnd->state.max[1]+2*ticlen,
				  -rnd->state.plane_d[2]+vscale*(rnd->axis_tics[3][i]-rnd->state.fmin)-0.5*ticlen);
		    glwPrintf(rnd->win,"%.3g",rnd->axis_tics[3][i]);
		  }
	      }

	  }
	break;
      case 1:
	eps=1.0e-2*(rnd->state.max[1]-rnd->state.min[1]);
	glBegin(GL_LINE_LOOP);
	glVertex3f(rnd->state.min[0],-rnd->state.plane_d[1]+eps,rnd->state.min[2]);
	glVertex3f(rnd->state.max[0],-rnd->state.plane_d[1]+eps,rnd->state.min[2]);
	glVertex3f(rnd->state.max[0],-rnd->state.plane_d[1]+eps,rnd->state.max[2]);
	glVertex3f(rnd->state.min[0],-rnd->state.plane_d[1]+eps,rnd->state.max[2]);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(rnd->state.min[0],-rnd->state.plane_d[1]+eps,-rnd->state.plane_d[2]);
	glVertex3f(rnd->state.max[0],-rnd->state.plane_d[1]+eps,-rnd->state.plane_d[2]);
	glVertex3f(-rnd->state.plane_d[0],-rnd->state.plane_d[1]+eps,rnd->state.min[2]);
	glVertex3f(-rnd->state.plane_d[0],-rnd->state.plane_d[1]+eps,rnd->state.max[2]);
	glEnd();
	if (rnd->state.vscale!=0.0)
	  {
	    glBegin(GL_LINES);
	    glVertex3f(rnd->state.max[0],-rnd->state.plane_d[1],rnd->state.max[2]);
	    glVertex3f(rnd->state.max[0],-rnd->state.plane_d[1]+rnd->state.vscale,rnd->state.max[2]);
	    glEnd();
	    if (rnd->axis_name[3][0]!='\0')
	      {
		glRasterPos3f(rnd->state.max[0],-rnd->state.plane_d[1]+rnd->state.vscale+ticlen,rnd->state.max[2]);
		glwPrintf(rnd->win,rnd->axis_name[3]);
	      }
	    
	    if (rnd->axis_ntics[3]!=0 && rnd->state.show_frame==2)
	      {
		glBegin(GL_LINES);
		for (i=0;i<rnd->axis_ntics[3];i++)
		  {
		    glVertex3d(rnd->state.max[0],
			       -rnd->state.plane_d[1]+vscale*(rnd->axis_tics[3][i]-rnd->state.fmin),
			       rnd->state.max[2]);
		    glVertex3d(rnd->state.max[0]+ticlen,
			       -rnd->state.plane_d[1]+vscale*(rnd->axis_tics[3][i]-rnd->state.fmin),
			       rnd->state.max[2]+ticlen);
		  }
		glEnd();
		for (i=0;i<rnd->axis_ntics[3];i++)
		  {
		    glRasterPos3d(rnd->state.max[0]+ticlen,
				  -rnd->state.plane_d[1]+vscale*(rnd->axis_tics[3][i]-rnd->state.fmin),
				  rnd->state.max[2]+ticlen);
		    glwPrintf(rnd->win,"%.3g",rnd->axis_tics[3][i]);
		  }
	      }
	  }
	break;
      case 0:
	eps=1.0e-2*(rnd->state.max[0]-rnd->state.min[0]);
	glBegin(GL_LINE_LOOP);
	glVertex3f(-rnd->state.plane_d[0]+eps,rnd->state.min[1],rnd->state.min[2]);
	glVertex3f(-rnd->state.plane_d[0]+eps,rnd->state.max[1],rnd->state.min[2]);
	glVertex3f(-rnd->state.plane_d[0]+eps,rnd->state.max[1],rnd->state.max[2]);
	glVertex3f(-rnd->state.plane_d[0]+eps,rnd->state.min[1],rnd->state.max[2]);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(-rnd->state.plane_d[0]+eps,rnd->state.min[1],-rnd->state.plane_d[2]);
	glVertex3f(-rnd->state.plane_d[0]+eps,rnd->state.max[1],-rnd->state.plane_d[2]);
	glVertex3f(-rnd->state.plane_d[0]+eps,-rnd->state.plane_d[1],rnd->state.min[2]);
	glVertex3f(-rnd->state.plane_d[0]+eps,-rnd->state.plane_d[1],rnd->state.max[2]);
	glEnd();
	if (rnd->state.vscale!=0.0)
	  {
	    glBegin(GL_LINES);
	    glVertex3f(-rnd->state.plane_d[0],rnd->state.max[1],rnd->state.max[2]);
	    glVertex3f(-rnd->state.plane_d[0]+rnd->state.vscale,rnd->state.max[1],rnd->state.max[2]);
	    glEnd();
	    if (rnd->axis_name[3][0]!='\0')
	      {
		glRasterPos3f(-rnd->state.plane_d[0]+rnd->state.vscale+ticlen,rnd->state.max[1],rnd->state.max[2]);
		glwPrintf(rnd->win,rnd->axis_name[3]);
	      }
	  
	    if (rnd->axis_ntics[3]!=0 && rnd->state.show_frame==2)
	      {
		glBegin(GL_LINES);
		for (i=0;i<rnd->axis_ntics[3];i++)
		  {
		    glVertex3d(-rnd->state.plane_d[0]+vscale*(rnd->axis_tics[3][i]-rnd->state.fmin),
			       rnd->state.max[1],
			       rnd->state.max[2]);
		    glVertex3d(-rnd->state.plane_d[0]+vscale*(rnd->axis_tics[3][i]-rnd->state.fmin),
			       rnd->state.max[1]+ticlen,
			       rnd->state.max[2]+ticlen);
		  }
		glEnd();
		for (i=0;i<rnd->axis_ntics[3];i++)
		  {
		    glRasterPos3d(-rnd->state.plane_d[0]-ticlen+vscale*(rnd->axis_tics[3][i]-rnd->state.fmin),
				  rnd->state.max[1]+2*ticlen,
				  rnd->state.max[2]+2*ticlen);
		    glwPrintf(rnd->win,"%.3g",rnd->axis_tics[3][i]);
		  }
	      }
	}
      break;
    }

#ifdef xxx
  glBegin(GL_LINE_LOOP);
  glVertex3f(rnd->state.min[0],rnd->state.min[1],rnd->state.min[2]);
  glVertex3f(rnd->state.max[0],rnd->state.min[1],rnd->state.min[2]);
  glVertex3f(rnd->state.max[0],rnd->state.max[1],rnd->state.min[2]);
  glVertex3f(rnd->state.min[0],rnd->state.max[1],rnd->state.min[2]);
  glEnd();

  
  glBegin(GL_LINE_LOOP);
  glVertex3f(rnd->state.min[0],rnd->state.min[1],rnd->state.max[2]);
  glVertex3f(rnd->state.max[0],rnd->state.min[1],rnd->state.max[2]);
  glVertex3f(rnd->state.max[0],rnd->state.max[1],rnd->state.max[2]);
  glVertex3f(rnd->state.min[0],rnd->state.max[1],rnd->state.max[2]);
  glEnd();


  glBegin(GL_LINES);
  glVertex3f(rnd->state.min[0],rnd->state.min[1],rnd->state.min[2]);
  glVertex3f(rnd->state.min[0],rnd->state.min[1],rnd->state.max[2]);

  glVertex3f(rnd->state.max[0],rnd->state.min[1],rnd->state.min[2]);
  glVertex3f(rnd->state.max[0],rnd->state.min[1],rnd->state.max[2]);


  glVertex3f(rnd->state.max[0],rnd->state.max[1],rnd->state.min[2]);
  glVertex3f(rnd->state.max[0],rnd->state.max[1],rnd->state.max[2]);

  glVertex3f(rnd->state.min[0],rnd->state.max[1],rnd->state.min[2]);
  glVertex3f(rnd->state.min[0],rnd->state.max[1],rnd->state.max[2]);
  glEnd();

#endif 
  /* glwSetFontSize(rnd->win,0.035);*/ 

}



glRenderer glrCreate(char *title, int xpos,int ypos,int wdt,int hgt)
{
    glWindow w;
    glRenderer rnd;
    int i;
    glwInitPosition(xpos,ypos,wdt,hgt);
    glwInitTitle(title);
    w=glwCreate();
    if (w==NULL)
	{
	    fprintf(stderr,"glrnd: unable to open window\n");
	    return NULL;
	}


    rnd=calloc(1,sizeof(struct glRendererStruct));
    rnd->win=w;
    strcpy(rnd->name,title);
    
     /* cool picture !*/
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear((GLbitfield)(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    glwSwapBuffers(rnd->win);
    glFlush();

    glrRegisterKeyAction(rnd,GLW_Return, glrKeyAction_continue,"Return: Quit user control mode");
    glrRegisterKeyAction(rnd,GLW_BackSpace, glrKeyAction_stop,"Backspace: Enter user control mode");
    glrRegisterKeyAction(rnd,GLW_Delete, glrKeyAction_stop,"Backspace: Enter user control mode");

    glrRegisterKeyAction(rnd,GLW_a, glrKeyAction_a,"a: Switch to GUI");
    glrRegisterKeyAction(rnd,GLW_c, glrKeyAction_c,"c: Toggle remembered lists");
    glrRegisterKeyAction(rnd,GLW_space,glrKeyAction_mode_control,"SPACE: Mode control");
    glrRegisterKeyAction(rnd,GLW_q,glrKeyAction_mode_control,"q: Mode control (Quit)");
    
    glrRegisterKeyAction(rnd,GLW_g,glrKeyAction_g,"g: Toggle Gouraud/flat shading.");
    
    /*    glrRegisterKeyAction(rnd,GLW_t,glrKeyAction_t,"t: Toggle transparency");
     */
    glrRegisterKeyAction(rnd,GLW_m,glrKeyAction_m,"m: Toggle model display when moving.");
    glrRegisterKeyAction(rnd,GLW_w,glrKeyAction_wireframe,"w: toggle wireframe mode");
    glrRegisterKeyAction(rnd,GLW_B,glrKeyAction_B,"B: Toggle background color (black/white). ");
    glrRegisterKeyAction(rnd,GLW_D,glrKeyAction_d,"d: Dump actual picture to  ppm file (look for *-*.ppm)");
    glrRegisterKeyAction(rnd,GLW_P,glrKeyAction_D,"D: Print actual picture using ppm dump");
    glrRegisterKeyAction(rnd,GLW_p,glrKeyAction_p,"p: Dump actual picture to eps file (look for *-*.eps)");
    glrRegisterKeyAction(rnd,GLW_V,glrKeyAction_V,"V: Start/Stop video recording");
    glrRegisterKeyAction(rnd,GLW_v,glrKeyAction_v,"v: Toggle vscale for plane sections");
    glrRegisterKeyAction(rnd,GLW_S,glrKeyAction_S,"S: Save actual state (look for .*-rndstate)");
    glrRegisterKeyAction(rnd,GLW_F,glrKeyAction_F,"F: Toggle rendering volume frame (bounding box) drawing.");
    glrRegisterKeyAction(rnd,GLW_plus,glrKeyAction_inc_mouse, "+: Increase mouse sensitivity.");
    glrRegisterKeyAction(rnd,GLW_minus,glrKeyAction_dec_mouse,"-: Decrease mouse sensitivity.");
    glrRegisterKeyAction(rnd,GLW_greater,glrKeyAction_zoom_in,">: Zoom in.");
    glrRegisterKeyAction(rnd,GLW_less,  glrKeyAction_zoom_out,"<: Zoom out.");
    glrRegisterKeyAction(rnd,GLW_comma, glrKeyAction_decrease_ctrl_prm,",: decrease control parameter.");
    glrRegisterKeyAction(rnd,GLW_period, glrKeyAction_increase_ctrl_prm,".: increase control parameter.");
    glrRegisterKeyAction(rnd,GLW_k, glrKeyAction_decrease_ctrl_fac,",: decrease control parameter by a factor.");
    glrRegisterKeyAction(rnd,GLW_K, glrKeyAction_increase_ctrl_fac,".: increase control parameter by a factor.");
    glrRegisterKeyAction(rnd,GLW_l,glrKeyAction_l,"l: Toggle level surface mode.");
    glrRegisterKeyAction(rnd,GLW_i,glrKeyAction_i,"i: Toggle isoline mode.");
    glrRegisterKeyAction(rnd,GLW_I,glrKeyAction_I,"I: Change number of isolines.");
    glrRegisterKeyAction(rnd,GLW_O,glrKeyAction_O,"O: Toggle Ortho");
    glrRegisterKeyAction(rnd,GLW_x,glrKeyAction_x,"x: Show x orthogonal plane section.");
    glrRegisterKeyAction(rnd,GLW_y,glrKeyAction_y,"y: Show y orthogonal plane section.");
    glrRegisterKeyAction(rnd,GLW_z,glrKeyAction_z,"z: Show z orthogonal plane section.");
    glrRegisterKeyAction(rnd,GLW_r,glrKeyAction_r,"r: Restore last saved state.");
    glrRegisterKeyAction(rnd,GLW_R,glrKeyAction_R,"R: Reset to internal default.");
    glrRegisterKeyAction(rnd,GLW_h,glrKeyAction_help,"h: This help.");
    glrRegisterKeyAction(rnd,GLW_question,glrKeyAction_help,"?: This help.");
    glrRegisterKeyAction(rnd,GLW_Left ,glrKeyAction_left,"left:move left");
    glrRegisterKeyAction(rnd,GLW_Right,glrKeyAction_right,"right:move right");
    glrRegisterKeyAction(rnd,GLW_Up   ,glrKeyAction_up   ,"up:move up");
    glrRegisterKeyAction(rnd,GLW_Down ,glrKeyAction_down ,"down:move down");
    /*    glrRegisterKeyAction(rnd,GLW_I,glrKeyAction_info,"I: print state information");
     */
    glrRegisterKeyAction(rnd,GLW_Tab,glrKeyAction_tab,"tab: toggle state change mode");
    glrRegisterKeyAction(rnd,GLW_Page_Down,glrKeyAction_next,"next: toggle state change  mode");
    glrRegisterKeyAction(rnd,GLW_Page_Up,glrKeyAction_prev,"prev: toggle state change  mode");

    rnd->list_base=glGenLists(GLR_N_LISTS);
    for(i=0;i<=GLR_MAX_OBJECTS;i++) rnd->lists[i]=i;
    for(i=1;i<=GLR_MAX_OBJECTS;i++)
      {
	
	glNewList(rnd->list_base+rnd->lists[i],GL_COMPILE);
	glEndList();
      }
    
    glNewList(rnd->list_base+GLR_ARROW_LIST,GL_COMPILE);
    arrow(10,1,0.25);
    glEndList();

    rnd->show_model=1;
    glrGenLightSource(rnd);
    rnd->list_valid=0;
    rnd->data_valid=0;
    
    rnd->switched=0;
    rnd->firstcall=1;
    rnd->next_dialog=0;
    for(i=0;i<=GLR_MAX_OBJECTS;i++) 
      rnd->state.show_object[i]=0;
    
    rnd->state.spacedim=3;

    rnd->dump_w=-1;/* automagically fit to window size */
    rnd->dump_h=-1;
    rnd->dump_i=0;
    

    rnd->draw_info=glrDefaultInfoCallback;
    rnd->user_info[0]='\0';
    rnd->refresh_user_info=0;
    strcpy(rnd->dump_name_stub,title);

    rnd->state.noptions=0;
    rnd->gui_called=0;
    rnd->render_with_gui=0;

    rnd->state.min[0]=0.0;
    rnd->state.max[0]=1.0;

    rnd->state.min[1]=0.0;
    rnd->state.max[1]=1.0;

    rnd->state.min[2]=0.0;
    rnd->state.max[2]=1.0;

    rnd->state.fmin=0.0;
    rnd->state.fmax=1.0;

    rnd->recording=0;

    rnd->axis_name[0][0]='\0';
    rnd->axis_name[1][0]='\0';
    rnd->axis_name[2][0]='\0';
    rnd->axis_name[3][0]='\0';

    rnd->axis_ntics[0]=0;
    rnd->axis_ntics[1]=0;
    rnd->axis_ntics[2]=0;
    rnd->axis_ntics[3]=0;

    glrReset(rnd);

    return rnd;
}


void glrDumpNext(glRenderer rnd)
{
  rnd->dump=1;
}

void glrSetAxisName(glRenderer rnd, char dir, char *name)
{
  switch(dir)
    {
    case 'x': strcpy(rnd->axis_name[0],name);break;
    case 'y': strcpy(rnd->axis_name[1],name);break;
    case 'z': strcpy(rnd->axis_name[2],name);break;
    case 'f': strcpy(rnd->axis_name[3],name);break;
    }

}

void glrSetAxisTics(glRenderer rnd, char dir, int ntics, double *tics)
{
  double *axis_tics;
  int i;
  
  switch(dir)
    {
    case 'x': axis_tics=rnd->axis_tics[0];rnd->axis_ntics[0]=ntics;break;
    case 'y': axis_tics=rnd->axis_tics[1];rnd->axis_ntics[1]=ntics;break;
    case 'z': axis_tics=rnd->axis_tics[2];rnd->axis_ntics[2]=ntics;break;
    case 'f': axis_tics=rnd->axis_tics[3];rnd->axis_ntics[3]=ntics;break;
    }
  for (i=0;i<ntics;i++) axis_tics[i]=tics[i];
}


void glrSetState_FileNameStub(glRenderer rnd, char *name)
{
  strcpy(rnd->dump_name_stub,name);
}

void glrSetDumpPixmapSize(glRenderer rnd, int w, int h)
{
  rnd->dump_w=w;
  rnd->dump_h=h;
}

void glrSetTitle(glRenderer rnd, char *title)
{
  glwSetTitle(rnd->win,title);
}


void glrDestroy(glRenderer rnd)
{
  glDeleteLists(GLR_N_LISTS,rnd->list_base);
  glwDestroy(rnd->win);
  free(rnd);

}

void glrSetUserInfo(glRenderer rnd, char *user_info,...)
{
  va_list ptr;
  va_start(ptr,user_info);
  vsprintf(rnd->user_info,user_info,ptr);
  va_end(ptr);
  rnd->refresh_user_info=1;
}

void glrSetVolume(glRenderer rnd,double xmin, double xmax,
                                  double ymin, double ymax,
                                  double zmin, double zmax)
{

    rnd->state.min[0]=xmin;
    rnd->state.max[0]=xmax;
    rnd->state.min[1]=ymin;
    rnd->state.max[1]=ymax;
    rnd->state.min[2]=zmin;
    rnd->state.max[2]=zmax;
    rnd->state.plane_d[0]= -xmin;
    rnd->state.plane_d[1]= -ymin;
    rnd->state.plane_d[2]= -zmin;
}

void glrGetIsolineMode(glRenderer rnd, int* mode)
{
  *mode=rnd->state.isoline_mode;
}

void glrGetLevelSurface(glRenderer rnd, int* mode)
{
  *mode=rnd->state.level_surf;
}

void glrSetLevelSurface(glRenderer rnd, int mode)
{
  rnd->state.level_surf=mode;
}

void glrGetLevel(glRenderer rnd, double* lev)
{
  *lev=rnd->state.level;
}

void glrSetLevel(glRenderer rnd, double lev)
{
  rnd->state.level=lev;
}

void glrSetDataValid(glRenderer rnd, int valid)
{
  rnd->data_valid=valid;
}

void glrMoveFrame(glRenderer rnd)
{
    rnd->state.move_model=0;
}



void glrMoveWireframe(glRenderer rnd)
{
    rnd->state.wireframe=1;
    rnd->state.move_model=1;
}

void glrMoveModel(glRenderer rnd)
{
  rnd->state.wireframe=0;
  rnd->state.move_model=1;
}


void glrGetWireframe(glRenderer rnd, int *wf)
{
  *wf=rnd->state.wireframe;
}

void glrSetFlatshading(glRenderer rnd, int flat)
{
  rnd->state.gouraud_shading=!flat;
}
void glrGetFlatshading(glRenderer rnd, int *flat)
{
  *flat=!rnd->state.gouraud_shading;
}

void glrSetDialog(glRenderer rnd, int dialog)
{
  rnd->dialog=dialog;
}
void glrGetDialog(glRenderer rnd, int *dialog)
{
  *dialog=rnd->dialog;
}


void glrGetPlane(glRenderer rnd, double *a,double *b,double *c, double *d)
{
  double eps;
  *a=0.0;
  *b=0.0;
  *c=0.0;
  switch(rnd->state.plane_dir)
    {

    case 0: 
      *a=1.0; break;
    case 1: 
      *b=1.0; break;
    case 2:
    default:
      *c=1.0; break;
    }
  
  eps=1.0e-5*(rnd->state.max[rnd->state.plane_dir]-rnd->state.min[rnd->state.plane_dir]);
  *d=-rnd->state.plane_d[rnd->state.plane_dir];
  
  if (*d>rnd->state.max[rnd->state.plane_dir]-eps) *d=rnd->state.max[rnd->state.plane_dir]-eps;
  if (*d<rnd->state.min[rnd->state.plane_dir]+eps) *d=rnd->state.min[rnd->state.plane_dir]+eps;
  *d = - (*d);
}

void glrGetPoint(glRenderer rnd, double *x, double *y, double *z)
{
  *x= -rnd->state.plane_d[0];
  *y= -rnd->state.plane_d[1];
  *z= -rnd->state.plane_d[2];
}

void glrSetVScale(glRenderer rnd, double vscale)
{
  rnd->state.vscale= vscale;
}

void glrGetVScale(glRenderer rnd, double *vscale)
{
 *vscale= rnd->state.vscale*(rnd->state.max[2]-rnd->state.min[2]);
}




static void glrReshape(glWindow w, void *info,
		       int width, int height)
{
  glRenderer rnd=(glRenderer)info;
  rnd->state.asp=(double)width/(double)height;
  rnd->wwidth=width;
  rnd->wheight=height;
  rnd->refresh_user_info=1;
}




static GLenum glrMouseDown(glWindow w, void * info,int x,int y, GLenum mask)
{
  glRenderer rnd=(glRenderer)info;
  glwSetMouseMoveFunc(w,glrMouse);
  if      (mask&GLW_LEFTBUTTON)
    {
      rnd->state.what_done=GLR_ROTATE;
      if (mask & GLW_SHIFT)
	{
	  rnd->state.what_done=GLR_LIGHT;
	}
    }
  else if (mask&GLW_MIDDLEBUTTON)
    {
      if (rnd->state.level_surf) 
	rnd->state.what_done=GLR_ISOLEVEL;
      else
	rnd->state.what_done=GLR_PLANE;

      if (mask & GLW_SHIFT)
	{
	  rnd->state.what_done=GLR_PLANE_ASPECT;
	}
    }
  else if (mask&GLW_RIGHTBUTTON)
    {
      rnd->state.what_done=GLR_INPLANE;
      
      if (mask & GLW_SHIFT)
	{
	  rnd->state.what_done=GLR_INPLANE_ASPECT;
	}

      else if (mask & GLW_MOD1)
	{
	  rnd->state.what_done=GLR_TRANSLATE;
	}
    }

  rnd->lastxpos=x;
  rnd->lastypos=y;
  glrShowModel(rnd,0);
  return GL_TRUE;
}

static GLenum glrMouseUp(glWindow w, void * info,int x,int y, GLenum mask)
{
    glRenderer rnd=(glRenderer)info;
    glwSetMouseMoveFunc(w,NULL);
    glrShowModel(rnd,1);
    rnd->state.what_done=0;
    return GL_TRUE;
}




int glrArrowList(glRenderer rnd)
{
  return rnd->list_base+GLR_ARROW_LIST;
}



static void glrDraw(glWindow w, void *info)
{
  glRenderer rnd=(glRenderer)info;
  int i;
  int draw_secondary=0;

  double xsize,ysize,zsize;
  double scalefac;

  if (rnd->switched) 
    {
      rnd->switched=0;
      return;
    }

  
  if (rnd->dump||rnd->firstcall)

  {
    static float mat_shininess[] = {40.0};
    static float mat_specular[] = {0.5, 0.5, 0.5, 1.0};
    static float mat_ambient_and_diffuse[]={1.0,1.0,1.0,1.0};
    static float lgt_ambient[]={0.1,0.1,0.1,1.0};
    static float lgt_diffuse[]={0.0,0.0,0.0,1.0};
    static float AmbientLight[] = {0.7,0.7,0.7, 1.0};
    static float TwoSideLighting[] = {GL_TRUE};


    glLightfv(GL_LIGHT0,GL_AMBIENT,lgt_ambient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,lgt_diffuse);
    glLightfv(GL_LIGHT1,GL_AMBIENT,lgt_ambient);
    glLightfv(GL_LIGHT1,GL_DIFFUSE,lgt_diffuse);
    glLightfv(GL_LIGHT2,GL_AMBIENT,lgt_ambient);
    glLightfv(GL_LIGHT2,GL_DIFFUSE,lgt_diffuse);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,  AmbientLight);
    glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, TwoSideLighting);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_ambient_and_diffuse);


    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);



    glShadeModel(GL_SMOOTH);



    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    rnd->firstcall=0;
  }
  rnd->list_valid=rnd->data_valid;
  
  glShadeModel(GL_SMOOTH);

  if (rnd->state.bg_black)
    glClearColor(0.0, 0.0, 0.0, 0.0);
  else
    glClearColor(1.0, 1.0, 1.0, 1.0);
  
  glClear((GLbitfield)(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
  
  if (rnd->state.bg_black)
    glColor3f(1.0,1.0,1.0);
  else
    glColor3f(0.0,0.0,0.0);
  
  
  /*** Viewport ***/
  
  if (rnd->state.show_info)
    {
      glViewport(0, 0,0.8*rnd->wwidth, 0.8*rnd->wheight);
    }
  else
    {
      glViewport(0, 0,rnd->wwidth, rnd->wheight);
    }
  /*** Projection Transformation ***/
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  if (rnd->state.ortho)
    glOrtho(-1.0,1.0,-1.0,1.0,-1.0,1.0);
  else
    glFrustum(-1.0,1.0,-1.0,1.0,4.0,6.0);
  
  

  /* Model View matrix */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  if (!rnd->state.ortho)
    glTranslatef(0.0,0.0,-5.0);
  
  
  /*** Light Source ***/
  
  glPushMatrix();
  glRotatef(rnd->state.ltx,1,0,0);
  glRotatef(-rnd->state.lty,0,0,1);
  if(!rnd->state.wireframe)
    {
      glEnable(GL_LIGHT0);
      glLightfv(GL_LIGHT0, GL_POSITION, lt_position);
      if (rnd->state.what_done==GLR_LIGHT)
	{
	  glBegin(GL_LINES);
	  glVertex3d(0,0,0);
	  glVertex3fv(lt_position);
	  glEnd();
	}
    }
  if(rnd->state.show_frame)
    glCallList(rnd->list_base+GLR_LTSRC_LIST);
   glPopMatrix();
  
  /*** Model Transformation ***/
  
  glTranslatef(rnd->state.tranx,rnd->state.trany,0.0);
 
  
  glRotatef(rnd->state.rotx,1.0,0.0,0.0);
  glRotatef(rnd->state.roty,0.0,1.0,0.0);
  glRotatef(rnd->state.rotz,0.0,0.0,1.0);
  glScalef(rnd->state.scale[0],rnd->state.scale[1],rnd->state.scale[2]);

  xsize=rnd->state.scale[0]*(rnd->state.max[0]-rnd->state.min[0]);
  ysize=rnd->state.scale[1]*(rnd->state.max[1]-rnd->state.min[1]);
  zsize=rnd->state.scale[2]*(rnd->state.max[2]-rnd->state.min[2]);


  scalefac=xsize;
  if (ysize>scalefac) scalefac=ysize;
  if (zsize>scalefac) scalefac=zsize;
  
  
  glScalef(rnd->state.zoom/scalefac,rnd->state.zoom/scalefac,rnd->state.zoom/scalefac);
  
  glTranslatef(
	       -0.5*(rnd->state.max[0]+rnd->state.min[0]),
	       -0.5*(rnd->state.max[1]+rnd->state.min[1]),
	       -0.5*(rnd->state.max[2]+rnd->state.min[2])
	       );
  
  /*** Draw stuff ***/

  
  /*** Draw current object ***/
  if (rnd->show_model)
    {
      if (!rnd->list_valid)
	{
	  glNewList(rnd->list_base+rnd->lists[0],GL_COMPILE);
	  if (rnd->draw)
	    rnd->draw(rnd,rnd->user_data);
	  else
	    rnd->draw2(rnd,rnd->user_data,rnd->user_data2);
	  glEndList();
          glCallList(rnd->list_base+rnd->lists[0]);
	  rnd->list_valid=1;
	  draw_secondary=1;
	}
      else
	glCallList(rnd->list_base+rnd->lists[0]);
      
      /*** Draw stored  objects ***/
	
      for(i=1;i<=GLR_MAX_OBJECTS;i++)
	if (rnd->state.show_object[i])
	  glCallList(rnd->list_base+rnd->lists[i]);
	
    }

  /*** Frame  & clipping plane ***/
  if (rnd->state.show_frame)
    {  
      /*     glLineWidth(1);
      glEnable(GL_LINE_SMOOTH);
      glDisable(GL_LINE_SMOOTH);
      */
      glrDrawFrame(rnd);
    }

  if (rnd->state.bg_black)
    glColor3f(1.0,1.0,1.0);
  else
    glColor3f(0.0,0.0,0.0);
 
  
  if (rnd->state.show_info && rnd->draw_info)
  {
    glViewport(0.0,0.0,rnd->wwidth,rnd->wheight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-1.0,-1.0,0.0);
    glScalef(2.0,2.0,0.0);
    rnd->draw_info(rnd,rnd->user_data);
   }

  glFlush();

  if (!rnd->dump)
    {
      glwSwapBuffers(w);

      if (rnd->secondary_callback && draw_secondary) 
	{
	  rnd->secondary_callback(rnd,rnd->user_data);
	  glwAttach(w);
	}
      if (!rnd->dialog) glwQuit(w);
      glDisable(GL_LIGHT0);
    }
}

static void glRender0(glRenderer rnd)
{
  
  if (rnd->dump)
    {

      char fname[128];
      sprintf(fname,"%s-%d.eps",rnd->dump_name_stub,rnd->dump_i);
      glwDump(rnd->win,rnd,fname,rnd->dump_w,rnd->dump_h);
      if (rnd->dialog) fprintf(stderr,"glrnd: picture dumped to %s\n",fname);
      rnd->dump_i++;

    }
  else
    {

      rnd->list_valid=rnd->data_valid;
      glwAttach(rnd->win);


      glwSetKeyDownFunc(rnd->win,glrKey);
      glwSetMouseDownFunc(rnd->win,glrMouseDown);
      glwSetMouseUpFunc(rnd->win,glrMouseUp);
      glwSetReshapeFunc(rnd->win,glrReshape);
      glwSetExposeFunc(rnd->win,glrReshape);
      glwSetRedrawFunc(rnd->win,glrDraw);

      
      if(rnd->render_with_gui)
	{ 
	  rnd->dialog=0;
	  glwSetControlMode(rnd->win,GLW_APPLICATION_DRIVEN);
	  glwProcess(rnd->win,rnd);
	  glwProcess(rnd->win,rnd);/*?!!!*/
	  rnd->dialog=1;
	  rnd->gui_called=1;
	  glr_gui(rnd);
	  rnd->gui_called=0;
	}
      else
	{
	  if (rnd->next_dialog)
	    {
	      rnd->dialog=1;
	      rnd->next_dialog=0;
	    }
		  
	  if (rnd->dialog)
	    {
	      char title[128];
	      glwSetControlMode(rnd->win,GLW_EVENT_DRIVEN);
	      sprintf(title,"%s [wait]",rnd->name);
	      glwSetTitle(rnd->win,title);
	    }
	  else
	    {
	      char title[128];
	      glwSetControlMode(rnd->win,GLW_APPLICATION_DRIVEN);
	      sprintf(title,"%s  [run]",rnd->name);
		      glwSetTitle(rnd->win,title);
	    }
	  
	  
	  glwProcess(rnd->win,rnd);
	}
      
    }
  
}

void glRenderWithGUI(glRenderer rnd, glrDrawCallback f, void *info)
{
  rnd->draw=f;
  rnd->draw2=NULL;
  rnd->user_data=info;
  if (glr_gui)
    rnd->render_with_gui=1;
  glRender0(rnd);
  rnd->render_with_gui=0;
  rnd->dump=0;
}

void glRender(glRenderer rnd, glrDrawCallback f, void *info)
{
  rnd->draw=f;
  rnd->draw2=NULL;
  rnd->user_data=info;
  glRender0(rnd);
  rnd->dump=0;
}

void glRender2(glRenderer rnd, glrDrawCallback2 f, void * structure,void *info)
{
  rnd->draw2=f;
  rnd->draw=NULL;
  rnd->user_data=structure;
  rnd->user_data2=info;
  glRender0(rnd);
  rnd->dump=0;
}

/***************************************************************************
** text output
****************************************************************************
*/

void glrSetFontSize(glRenderer rnd, double font_size)
{
  glwSetFontSize(rnd->win, font_size);
}
void glrPrint(glRenderer rnd, char *text){
  glwPrint(rnd->win, text);
}
static char glr_string_buffer[256];
void glrPrintf(glRenderer rnd, char *format, ...)
{
  va_list ptr;
  va_start(ptr,format);
  vsprintf(glr_string_buffer,format,ptr);
  va_end(ptr);
  glwPrint(rnd->win,glr_string_buffer);
}




/*
 *
 * $Log: glrnd.c,v $
 * Revision 2.72  2001/05/18 10:54:13  fuhrmann
 * strtok bug
 *
 * Revision 2.71  2001/05/15 15:23:45  petzoldt
 * Martins Farbskala: Konflikte beseitigt
 *
 * Revision 2.70  2000/12/13 16:17:43  fuhrmann
 * pfeilchen
 *
 * Revision 2.69  2000/11/22 15:20:49  fuhrmann
 * ICE hack
 * 1 mpeg/eps per variation
 * variation[i]-> variationi
 * bias=... option
 *
 * Revision 2.68  2000/10/25 10:49:59  fuhrmann
 * crop stuff for graphics
 *
 * Revision 2.67  2000/10/18 16:43:32  fuhrmann
 * scl_3d_gl works
 *
 * Revision 2.66  2000/10/18 08:41:50  fuhrmann
 *  scl_3d next step
 *
 * Revision 2.65  2000/01/13 17:36:41  fuhrmann
 * optmized ps output
 *
 * Revision 2.64  2000/01/12 17:50:59  fuhrmann
 * ci stuff repaired
 *
 * Revision 2.63  2000/01/11 16:33:41  fuhrmann
 * name buffer sizes
 *
 * Revision 2.62  1999/08/04 13:03:43  fuhrmann
 * some hacks concerning feedback
 *
 * Revision 2.61  1999/07/21 18:39:40  fuhrmann
 * user info position was wrong...
 *
 * Revision 2.60  1999/07/02 10:38:04  fuhrmann
 * Mesa bug neutralized, gleps reference taken out of glwin
 *
 * Revision 2.59  1999/06/30 13:07:50  fuhrmann
 * font size stuff
 *
 * Revision 2.58  1999/06/29 17:56:24  fuhrmann
 * Feedback buffer is used now...
 *
 * Revision 2.57  1999/04/20 17:32:16  fuhrmann
 * sgi warnings, 3D flux test
 *
 * Revision 2.56  1999/02/09 16:07:26  fuhrmann
 * F key for tics
 * Stupid bug in glmesh.c removed!!!
 *
 * Revision 2.55  1999/02/05 20:24:34  fuhrmann
 * tics introduced
 *
 * Revision 2.54  1999/02/02 14:37:43  fuhrmann
 * mode switch handling in glrnd improved
 * bug in glmesh removed
 *
 * Revision 2.53  1999/02/01 18:36:34  fuhrmann
 * minor changes
 *
 * Revision 2.52  1998/12/18 13:56:26  fuhrmann
 * make keydoc stuff
 *
 * Revision 2.51  1998/09/04 15:25:47  fuhrmann
 * skip frame option in StartRecording
 *
 * Revision 2.50  1998/08/26  18:00:15  fuhrmann
 * *** empty log message ***
 *
 * Revision 2.49  1998/08/11  09:24:47  fuhrmann
 * dy in plane_aspect
 *
 * Revision 2.48  1998/08/10  19:52:34  fuhrmann
 * isoline distance, vscale change in glrnd
 *
 * Revision 2.47  1998/08/03  17:36:27  fuhrmann
 * ildist=0.05
 *
 * Revision 2.46  1998/07/27  19:10:04  fuhrmann
 * material stuff in glmesh
 *
 * Revision 2.45  1998/07/27  18:04:29  fuhrmann
 * Handling 3 planes, function evaluation, glrGetPoint
 *
 * Revision 2.44  1998/07/06  11:54:06  fuhrmann
 * was_dialog error
 *
 * Revision 2.43  1998/07/06  10:45:11  fuhrmann
 * Increased compiler warning level for all PDELIB-1.7 modules,
 * removed all bugs, some warnings remain (rcsids, ld_init, unused parameters)
 *
 * Revision 2.42  1998/04/15  17:48:25  fuhrmann
 * bug with dump filename removed
 *
 * Revision 2.41  1998/03/06  16:45:30  fuhrmann
 * tiff->ppm
 * print command
 * video recording
 *
 * Revision 2.40  1998/03/03  13:40:45  fuhrmann
 * alpha value in diffuse reflexion
 *
 * Revision 2.39  1998/03/03  10:07:30  fuhrmann
 * color stuff changed a bit...
 *
 * Revision 2.38  1998/01/15  13:42:25  fuhrmann
 * prevent segmentation fault in key action registry
 *
 * Revision 2.37  1998/01/14  18:27:33  fuhrmann
 * Merge mit Ilja
 *
 * Revision 2.36  1997/11/27  19:03:08  fuhrmann
 * glwRecord stuff, PAL-Format, tex-file for keys
 *
 * Revision 2.35  1997/11/04  15:53:29  fuhrmann
 * color scales ; crash with glrnd
 *
 * Revision 2.34  1997/10/15  15:17:24  fuhrmann
 * secondary callback stuff
 *
 * Revision 2.33  1997/10/01  15:41:27  fuhrmann
 * GLW_MOD1
 *
 * Revision 2.32  1997/09/22  17:37:37  fuhrmann
 * info callback stuff
 *
 * Revision 2.31  1997/09/09  17:49:25  fuhrmann
 * line_smooth & linewidth 1 for frame
 *
 * Revision 2.30  1997/07/25  09:49:07  fuhrmann
 * bug mit r/R keys removed
 *
 * Revision 2.29  1997/06/18  09:49:40  fuhrmann
 * initialization of *min and *max
 *
 * Revision 2.28  1997/05/15  10:52:57  fuhrmann
 * glrShowModel stuff
 *
 * Revision 2.27  1997/04/21  09:42:01  fuhrmann
 * user_data stuff - this should be the kernel of the database module
 *
 * Revision 2.26  1997/04/15  09:38:18  fuhrmann
 * some minor adjustments in mouse button stuff
 *
 * Revision 2.25  1997/04/15  08:10:59  fuhrmann
 * info stuff, new mous key scheme etc, still not ready
 *
 * Revision 2.24  1997/04/10  13:55:51  fuhrmann
 * selective display of objects
 *
 * Revision 2.23  1997/04/08  15:22:30  fuhrmann
 * show state stuff in window, not in title etc.
 *
 * Revision 2.22  1997/03/20  18:17:37  fuhrmann
 * bounding box + function min/max -> state
 *
 * Revision 2.21  1997/03/19  11:05:39  fuhrmann
 * transparency (beta code)
 *
 * Revision 2.20  1997/03/18  17:57:11  fuhrmann
 * wireframe bugs corrected
 * save/restore state filename
 * roty; get/set perspective deleted
 *
 * Revision 2.19  1997/02/28  17:41:07  fuhrmann
 * glwGrab code
 *
 * Revision 2.18  1997/02/28  14:13:08  fuhrmann
 * show_list_toggle -> state
 *
 * Revision 2.17  1997/02/24  20:38:10  fuhrmann
 * list remembering beta code
 *
 * Revision 2.16  1997/02/18  09:37:35  fuhrmann
 * introduced: glrState struct
 *             glrGetWindow (for gui)
 *             glrSetGUI
 * allows modular extension by any gui procedure (JAVA, motif etc.)
 *
 * Revision 2.15  1997/02/05  16:58:20  fuhrmann
 * removed some SGI warnings
 *
 * Revision 2.14  1996/10/11  18:13:19  fuhrmann
 * GLW_s->GLW_S for save (not so dangerous...)
 *
 * Revision 2.13  1996/10/11  18:06:11  fuhrmann
 * Save/Restore state exported
 * Light source movement changed (close to optimal now)
 * display lists for frame/light source
 *
 * Revision 2.12  1996/10/09  17:18:29  fuhrmann
 * aspect ratio handling
 * joint mouse/keyboard interface
 * save/load/init in one procedure
 * verbosity if dialog (when working with files)
 * new state view in title
 * keys changed: s is now save, tab/prev/next is toggle mode, c is free
 *
 * Revision 2.11  1996/09/23  17:38:56  fuhrmann
 * access of display list via glGenLists
 *
 * Revision 2.10  1996/09/19  16:32:41  fuhrmann
 * test if dump file is writable
 *
 * Revision 2.9  1996/09/17  09:33:22  langmach
 * text output from glwin is now available
 *
 * Revision 2.8  1996/09/06  18:12:07  fuhrmann
 * zoom was in wrong direction
 *
 * Revision 2.7  1996/09/04  19:53:35  fuhrmann
 * - experiments with zoom stuff & ortho
 * - isoline stuff
 * - glrXSetPlane
 *
 * Revision 2.6  1996/09/03  08:48:00  fuhrmann
 * fit dump pixmap size to window size as default
 *
 * Revision 2.5  1996/08/30  18:06:01  fuhrmann
 * dump/restore state ( d &r)
 *
 * Revision 2.4  1996/07/17  18:43:34  fuhrmann
 * -DUSE_DISPLAY_LISTS as option
 *
 * Revision 2.3  1996/07/17  16:57:20  langmach
 * moving with cursor keys
 *
 * Revision 2.2  1996/03/11  19:27:48  fuhrmann
 * repaired glrSetDumpPixmapSize
 *
 * Revision 2.1  1996/02/21  20:21:28  fuhrmann
 * - bug removed when calling glRender with changing data
 *
 * Revision 2.0  1996/02/15  19:57:10  fuhrmann
 * First meta-stable distribution
 *
 * Revision 1.5  1996/02/09  16:47:23  fuhrmann
 * - Changed keyboard interface. Now there is
 *   one State Control Key (Space).
 * - show user/app ctrl in window title
 *
 * Revision 1.4  1996/02/06  09:58:17  fuhrmann
 * * glrDump stuff introduced (key D)
 * * migrated to glwSetControlMode
 *
 * Revision 1.3  1995/11/08  12:20:02  fuhrmann
 * glrSetTitle introduced
 * glRender2 for structure and data pointer usage introduced
 *
 * Revision 1.2  1995/10/24  12:40:22  fuhrmann
 * glwInitDisplaymode 'raus
 *
 * Revision 1.1  1995/10/20  15:42:03  fuhrmann
 * Initial revision
 *
 */
