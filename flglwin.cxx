#ifdef HAVE_FLTK
#ifndef NO_FLTK
#include "flglwin.h"

#include <FL/Fl.H>
#include <FL/gl.h>
#include <iostream.h>


#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

// TODO:
// functionality test
// mpeg
// spurious segfaults ??
//
// c++ example for process()
// vernuenftiges dump-API...

//
// process() wird fuer das gesamtwidget
// gebraucht -- sonst macht das alles wenig sinn
// flglwProcess(this)
//


//
// so far so good... What do we do with offscreen functionality ?
// -- keep that old X stuff ?
// maybe, video dump can work w/o offscreen: dump before swap...
// glw->XCTX
// glw->flctx


//
//  PPMDump ctx
//  EPSDump ctx
//  MPGDump ctx
// create, destroy & process are virtual methods...



// questions to Bill S.
// why there is no resize event instead of  the valid() stuff ? 
// 
//

//////////////////////////////////////////////////
// glwin definitions
#define GLW_DEFAULT_FONT_SIZE 0.035
#define GLW_MAX_FONT_SIZE 50
#define GLW_BUFSIZE 256
#define GLW_MAX_FONT_SIZE 50
static char glw_string_buffer[GLW_BUFSIZE];

extern "C"
{
#include "gleps.h"
  typedef void (*glwRedrawFunc)(glWindow w,void * user_data);
  typedef void (*glwExposeFunc)(glWindow w, void *user_data,int x, int y);
  typedef void (*glwReshapeFunc)(glWindow w, void *user_data,int x, int y);
  typedef GLenum (*glwKeyDownFunc)(glWindow w, void *user_data,int, GLenum);
  typedef  GLenum (*glwMouseDownFunc)(glWindow w, void *user_data,int x, int y, GLenum mask);
  typedef GLenum (*glwMouseUpFunc)(glWindow w, void *user_data,int x, int y, GLenum mask);
  typedef GLenum (*glwMouseMoveFunc)(glWindow w, void *user_data,int x, int y, GLenum mask);
  typedef void (*glwReshapeFunc)(
                                 glWindow w,
                                 void *user_data,
                                 int width,
                                 int height
                                 );
                  
void glwStartMPEGRecording(
              glWindow win, 
              char *fileNameStub,
	      int skip
              );
void  glwStopMPEGRecording(
              glWindow win
              );

void glwStartEPSRecording(
              glWindow win, 
              char *fileNameStub,
	      int skip
              );
void  glwStopEPSRecording(
              glWindow win
              );



}

void  glwInitFB(glWindow win);

typedef struct  glWindowStruct
{
  flglWindow *flglwin;
  void *info;
  int mode;
  int quit;
  double font_size;
  glwRedrawFunc RedrawFunc;
  glwMouseDownFunc MouseDownFunc;
  glwMouseMoveFunc MouseMoveFunc;
  glwMouseUpFunc MouseUpFunc;
  glwReshapeFunc ReshapeFunc;
  glwReshapeFunc ExposeFunc;
  glwKeyDownFunc KeyDownFunc;
#include "gldump.h"
} *glWindow;



//////////////////////////////////////////////////////
// flglwin API

flglWindow::flglWindow(int x,int y,int w,int h,const char *l, glWindow win):
  Fl_Gl_Window(x,y,w,h,l)
{
  gl_font(FL_COURIER,10);
  size_range((int)(0.1*w),(int)(0.1*h),Fl::h()*(double)w/(double)h,Fl::h(),0,0,1); 
  private_glwin=win;
}

flglWindow::flglWindow(int x,int y,int w,int h,const char *l):
  Fl_Gl_Window(x,y,w,h,l)
{
  gl_font(FL_COURIER,10);
  private_glwin=(glWindow)calloc(sizeof(struct glWindowStruct),1);
  private_glwin->mode=GLW_EVENT_DRIVEN;
  glwSetFontSize(private_glwin,GLW_DEFAULT_FONT_SIZE);
  glwInitFB(private_glwin);
}

flglWindow::~flglWindow()
{

  free(private_glwin);
  // destroy fb
}
      

void flglWindow::process() 
{
  glwProcess(glwin(),0);
};

void flglWindow::draw() 
{
  if (glwin()->RedrawFunc) glwin()->RedrawFunc(glwin(),glwin()->info);
};

int flglWindow::handle(int event) 
{ 
  make_current();
  switch (event)
    {
    case FL_PUSH:
      if (glwin()->MouseDownFunc)
	{
	  if (glwin()->MouseDownFunc(glwin(),
				    glwin()->info,
				    Fl::event_x(),
				    Fl::event_y(), 
				    mask()))  
	    valid(0);
	  return 1;
	}
    case FL_DRAG:
      if (glwin()->MouseMoveFunc)
	{
	  if (glwin()->MouseMoveFunc(glwin(),glwin()->info,
				      Fl::event_x(),Fl::event_y(), mask()))  
	    valid(0);
	  return 1;
	}
    case FL_RELEASE:    
      if (glwin()->MouseUpFunc)
	{
	  if (glwin()->MouseUpFunc(glwin(),glwin()->info,
				  Fl::event_x(),Fl::event_y(), mask()))
	    valid(0);
	    return 1;
	}
      
    case FL_FOCUS :
    case FL_UNFOCUS :
      //... Return 1 if you want keyboard events, 0 otherwise
      return 1;
    case FL_KEYBOARD:
      //... keypress, key is in Fl::event_key(), ascii in Fl::event_text()
	  //... Return 1 if you understand/use the keyboard event, 0 otherwise...
      if (glwin()->KeyDownFunc(glwin(),glwin()->info,
			      translated_key(), mask()))  valid(0);
	  return 1;
    case FL_SHORTCUT:
      //... shortcut, key is in Fl::event_key(), ascii in Fl::event_text()
      //... Return 1 if you understand/use the shortcut event, 0 otherwise...
      return 0;
    case FL_SHOW:
      if (glwin()->ExposeFunc) 
	{
	  glwin()->ExposeFunc(glwin(),glwin()->info,w(),h());
	  glwSetFontSize(glwin(),glwin()->font_size);
	      valid(0);
	}
	  return 1;
    default:
      // pass other events to the base class...
	  return Fl_Gl_Window::handle(event);
	  
    }
};

GLenum flglWindow::mask(void)
{
  int msk = 0;
  int state=Fl::event_state();
  
  if (state&FL_BUTTON1)  msk |= GLW_LEFTBUTTON;
  if (state&FL_BUTTON2)  msk |= GLW_MIDDLEBUTTON;
  if (state&FL_BUTTON3)  msk |= GLW_RIGHTBUTTON;
  if (state&FL_SHIFT)    msk |= GLW_SHIFT;
  if (state&FL_CTRL)     msk |= GLW_CONTROL;
  if (state&FL_ALT)      msk |= GLW_MOD1;
  return (GLenum) msk;
};

char flglWindow::translated_key(void)
{
  int key=Fl::event_key();
  int state=Fl::event_state();
  
  if (isalpha(key))
    {
      if (state&FL_SHIFT) 
	return toupper(key); // convert to caps
      else
	return key; // small letters as plain
	}
  else if (isprint(key)) return key; // all printable  as plain
  else if (key>= FL_KP && key<=FL_KP_Last)
    {
      key=key-FL_KP;
      if (key<=9) return GLW_KP_0+key;
      else return 0;
    }
  else if (key>= FL_F && key<=FL_F_Last)
    {
      key=key-FL_F;
      if (key<=12) return GLW_F1+key;
      else return 0;
    }
#define caseKS(sym) case FL_##sym: return GLW_##sym; break
  switch(key)
    {
      caseKS(Escape);
      caseKS(BackSpace);
      caseKS(Tab); 
      caseKS(Print); 
      caseKS(Scroll_Lock); 
      caseKS(Pause);
      caseKS(Insert);
      caseKS(Home);
      caseKS(Page_Up);
      caseKS(Delete);
      caseKS(End);
      caseKS(Page_Down);
      caseKS(Left);
      caseKS(Up);
      caseKS(Right);
      caseKS(Down);
      caseKS(KP_Enter); 
    case FL_Enter: return GLW_Return; break;
    }
  return 0;
};

/*
<h3><A name=event_key>int Fl::event_key()
<br>int Fl::event_key(int)</A>
<br><A name=get_key>int Fl::get_key(int)</A></h3>

<tt>Fl::event_key()</tt> returns which key on the keyboard was last
pushed.  It returns zero if the last event was not a key press or release.

<P><tt>Fl::event_key(int)</tt> returns true if the given key was held
down (or pressed) <I>during</I> the last event.  This is constant until
the next event is read from the server.

<P><tt>Fl::get_key(int)</tt> returns true if the given key is held down <I>
now</I>.  Under X this requires a round-trip to the server and is <I>
much</I> slower than <tt>Fl::event_key(int)</tt>.

<P>Keys are identified by the <I>unshifted</I> values. FLTK defines a
set of symbols that should work on most modern machines for every key
on the keyboard:

<UL>
<LI>All keys on the main keyboard producing a printable ASCII
character use the value of that ASCII character (as though shift);
ctrl, and caps lock were not on).  The space bar is 32. </LI>
<LI>All keys on the numeric keypad producing a printable ASCII
character use the value of that ASCII character plus <tt>FL_KP</tt>.
The highest possible value is <tt>FL_KP_Last</tt> so you can
range-check to see if something is  on the keypad. </LI>
<LI>All numbered function keys use the number on the function key plus <tt>
FL_F</tt>.  The highest possible number is <tt>FL_F_Last</tt>, so you
can range-check a value. </LI>
<LI>Buttons on the mouse are considered keys, and use the button
number (where the left button is 1) plus <tt>FL_Button</tt>. </LI>
<LI>All other keys on the keypad have a symbol: <tt>FL_Escape,
FL_BackSpace, FL_Tab, FL_Enter, FL_Print, FL_Scroll_Lock, FL_Pause,
FL_Insert, FL_Home, FL_Page_Up, FL_Delete, FL_End, FL_Page_Down,
FL_Left, FL_Up, FL_Right, FL_Down, FL_Shift_L, FL_Shift_R,
FL_Control_L, FL_Control_R, FL_Caps_Lock, FL_Alt_L, FL_Alt_R,
FL_Meta_L, FL_Meta_R, FL_Menu, FL_Num_Lock, FL_KP_Enter</tt>.  Be
careful not to confuse these with the very similar, but all-caps,
symbols used by <A href=events.html#event_state><tt>Fl::event_state()</tt>
</A>. </LI>
</UL>
*/  


///////////////////////////////////////////////////
// glwin API


static char init_title[GLW_NAMELEN]={'g','l','t','o','o','l','s'};
static int init_x=0,
  init_y=0,
  init_w=352,
  init_h=288;

#define glwError(msg) glw_error(__LINE__,msg) 
static void glw_error(int line, char * msg)
{
 fprintf(stderr,"\nflglwin.c: error on line %d: %s\n",line,msg);
}

char *glwGetDefault(glWindow w,char *resource, char *dflt)
{

  return dflt; // xdefaults are too muchh...
}

#include "gldump.c"

glWindow glwCreate(void) 
{  
  glWindow win=(glWindow)calloc(1,sizeof(struct  glWindowStruct));
  win->mode=GLW_EVENT_DRIVEN;
  win->flglwin=new flglWindow(init_x,init_y,init_w,init_h,init_title,win);
  win->flglwin->resizable(win->flglwin);
  win->flglwin->show();
  win->flglwin->flush();
  win->flglwin->valid(0);
  glwInitTitle("gltools");
  init_x=0;
  init_y=0;
  init_w=352;
  init_h=288;
  glwSetFontSize(win,GLW_DEFAULT_FONT_SIZE);
  glwInitFB(win);
  return win;
}

void glwProcess(glWindow w, void *info)
{
  w->info=info;
 
  Fl::wait(0.0);
  w->flglwin->handle(FL_SHOW);// necessary to init glrnd viewport
  w->flglwin->flush();
  glwDumpOnSwap(w);

  
  if (w->mode==GLW_EVENT_DRIVEN)
    {  
      w->quit=0;
      while(!w->quit)
	{
	  w->flglwin->cursor(FL_CURSOR_HAND);
	  Fl::wait(1.0e20);
	  if (!w->flglwin->valid())
	    {
	      w->flglwin->handle(FL_SHOW);
	      w->flglwin->cursor(FL_CURSOR_WAIT);
	      w->flglwin->flush();
	      glwDumpOnSwap(w);
	      w->flglwin->cursor(FL_CURSOR_HAND);
	    }
	}
      w->flglwin->cursor(FL_CURSOR_DEFAULT);
      
    }
}

void  glwSetControlMode(glWindow w,int mode)
{
  w->mode=mode;
};
void   glwQuit(glWindow w)
{
  w->quit=1;
};


void glwPrint(
              glWindow w,
              char *s
              )
{
  if (!glwPrintFB(w,s))
    gl_draw(s);
};

void glwPrintf(glWindow w,char *format,...)
{ 
  char buffer[128];
  va_list ptr;
  va_start(ptr,format);
  vsprintf(buffer,format,ptr);
  va_end(ptr);
  glwPrint(w,buffer);
}

void glwSetFontSize(glWindow w,double size)
{
  w->font_size=size;
  gl_font(FL_COURIER,(int)(size*(double)w->flglwin->h()));
};


void  glwSwapBuffers(glWindow w)
{
};




char*      glwGetTitle(glWindow w){return (char*)w->flglwin->label();};
void      glwSetTitle(glWindow w,char *title){w->flglwin->label(title);};
void      glwInitTitle(char *title){ strncpy(init_title,title,GLW_NAMELEN);};


void glwGetWindowSize(
                      glWindow w, 
                      int *width, 
                      int *height
                      )
{
  *width=w->flglwin->w();
  *height=w->flglwin->h();
};
void glwInitPosition(int x,int y,int w, int h )
{
  init_x=x;
  init_y=y;
  init_w=w;
  init_h=h;

};

int       glwAttach(
                    glWindow win
                    ){
  win->flglwin->make_current();
};






void glwInitOffscreen(void){};
void glwInitToplevel(void* x){};

void glwDestroy(glWindow w)
{
  delete w->flglwin; // free(w) in this destructor
}

void glwSetRedrawFunc(glWindow win, glwRedrawFunc f)
{
  win->RedrawFunc=f;
}

void glwSetMouseDownFunc(glWindow win, glwMouseDownFunc f)
{
  win->MouseDownFunc=f;
}
void glwSetMouseUpFunc(glWindow win, glwMouseUpFunc f)
{
  win->MouseUpFunc=f;
}
void glwSetKeyDownFunc(glWindow win, glwKeyDownFunc f)
{
  win->KeyDownFunc=f;
}
void glwSetMouseMoveFunc(glWindow win, glwMouseDownFunc f)
{
  win->MouseMoveFunc=f;
}

void glwSetReshapeFunc( glWindow win,glwReshapeFunc f)
{
  win->ReshapeFunc=f;
}

void glwSetExposeFunc(glWindow win, glwReshapeFunc f)
{
  win->ExposeFunc=f;
}

#endif
#endif

//
// $Log: flglwin.cxx,v $
// Revision 2.4  2000/12/13 15:30:26  fuhrmann
// further fltk hacking
// glprimitives
// arrows for fluxvisualization
//
// Revision 2.3  2000/12/10 22:58:09  fuhrmann
// gleps bug
// flglwin proto works
//
// Revision 2.2  2000/12/08 16:46:04  fuhrmann
//  little bug in flglwin
//
// Revision 2.1  2000/12/08 16:39:43  fuhrmann
// first steps to fltk
//
//




