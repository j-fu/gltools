static char *rcsid= "$Id: glwin.c,v 2.57 2001/05/04 15:19:17 fuhrmann Exp $";
#ifdef NO_FLTK
#undef HAVE_FLTK
#endif
#ifndef HAVE_FLTK
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>
#include <GL/glx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdarg.h>
#include <assert.h>
#include "glwin.h"
#include "gleps.h" /* TODO: avoid this depedency somehow */

#define GLW_BUFSIZE 256
#define GLW_MAX_FONT_SIZE 50
#define GLW_FONT_NAME  "-*-lucidatypewriter-*-r-normal-*-%d-*-*-*-*-*-iso8859-1"
#define GLW_DEFAULT_FONT_SIZE 0.035
static int debug=0;
static char glw_string_buffer[GLW_BUFSIZE];

#define glwTrace(x) {if (debug) fprintf(stderr,"glwin: %5d: %s\n",__LINE__,x);}
void glwDebug(void)
{
  debug=!debug;
  if (debug) 
    fprintf(stderr,"glwin: debugging enabled.\n");
  else
    fprintf(stderr,"glwin: debugging disabled.\n");
}

#define glwError(msg) glw_error(__LINE__,msg) 
static void glw_error(int line, char * msg)
{
 fprintf(stderr,"\nflglwin.c: error on line %d: %s\n",line,msg);
}

struct glWindowStruct 
{
  int x, y, w, h;
  GLenum type;
  GLenum dmPolicy;
  Window wMain, wOverlay, wToplevel;
  Pixmap pixmap;
  

  XVisualInfo *vInfoMain, *vInfoOverlay;
  int vInfoMainClass;
  int vInfoOverlayClass;
  Colormap cMapMain, cMapOverlay;
  GLXPixmap glx_pixmap;
  GLXContext cMain, cOverlay;
  
  int isMesa;

  void (*RedrawFunc)(glWindow, void *);
  int control_mode;
  
  void (*ExposeFunc)(glWindow, void *,int, int);
  void (*ReshapeFunc)(glWindow, void *,int, int);
  GLenum (*KeyDownFunc)(glWindow, void *,int, GLenum);
  GLenum (*MouseDownFunc)(glWindow, void *,int, int, GLenum);
  GLenum (*MouseUpFunc)(glWindow, void *,int, int, GLenum);
  GLenum (*MouseMoveFunc)(glWindow, void *,int, int, GLenum);
  
  int lastEventType;
  GLenum drawAllowFlag;
  
  Display *display;
  int screen; 
  Window root;
  Atom deleteWindowAtom;
  char *title,*xtitle,*state;
  void * info;
  int process;
  GLuint font_base[GLW_MAX_FONT_SIZE];
  GLuint font_range[GLW_MAX_FONT_SIZE];
  double font_size;
  int font_to_use;
  int keep_aspect;

  Cursor wait_cursor;
  Cursor user_cursor;

  int offscreen;
  int swap;

#include "gldump.h"
};
#include "gldump.c"

static glWindow CurrentWindow=NULL;

/******************************************************************************/

float colorMaps[] = {
    0.000000, 1.000000, 0.000000, 1.000000, 0.000000, 1.000000, 
    0.000000, 1.000000, 0.333333, 0.776471, 0.443137, 0.556863, 
    0.443137, 0.556863, 0.219608, 0.666667, 0.666667, 0.333333, 
    0.666667, 0.333333, 0.666667, 0.333333, 0.666667, 0.333333, 
    0.666667, 0.333333, 0.666667, 0.333333, 0.666667, 0.333333, 
    0.666667, 0.333333, 0.039216, 0.078431, 0.117647, 0.156863, 
    0.200000, 0.239216, 0.278431, 0.317647, 0.356863, 0.400000, 
    0.439216, 0.478431, 0.517647, 0.556863, 0.600000, 0.639216, 
    0.678431, 0.717647, 0.756863, 0.800000, 0.839216, 0.878431, 
    0.917647, 0.956863, 0.000000, 0.000000, 0.000000, 0.000000, 
    0.000000, 0.000000, 0.000000, 0.000000, 0.247059, 0.247059, 
    0.247059, 0.247059, 0.247059, 0.247059, 0.247059, 0.247059, 
    0.498039, 0.498039, 0.498039, 0.498039, 0.498039, 0.498039, 
    0.498039, 0.498039, 0.749020, 0.749020, 0.749020, 0.749020, 
    0.749020, 0.749020, 0.749020, 0.749020, 1.000000, 1.000000, 
    1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
    0.000000, 0.000000, 0.247059, 0.247059, 0.247059, 0.247059, 
    0.247059, 0.247059, 0.247059, 0.247059, 0.498039, 0.498039, 
    0.498039, 0.498039, 0.498039, 0.498039, 0.498039, 0.498039, 
    0.749020, 0.749020, 0.749020, 0.749020, 0.749020, 0.749020, 
    0.749020, 0.749020, 1.000000, 1.000000, 1.000000, 1.000000, 
    1.000000, 1.000000, 1.000000, 1.000000, 0.000000, 0.000000, 
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
    0.247059, 0.247059, 0.247059, 0.247059, 0.247059, 0.247059, 
    0.247059, 0.247059, 0.498039, 0.498039, 0.498039, 0.498039, 
    0.498039, 0.498039, 0.498039, 0.498039, 0.749020, 0.749020, 
    0.749020, 0.749020, 0.749020, 0.749020, 0.749020, 0.749020, 
    1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 
    1.000000, 1.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
    0.000000, 0.000000, 0.000000, 0.000000, 0.247059, 0.247059, 
    0.247059, 0.247059, 0.247059, 0.247059, 0.247059, 0.247059, 
    0.498039, 0.498039, 0.498039, 0.498039, 0.498039, 0.498039, 
    0.498039, 0.498039, 0.749020, 0.749020, 0.749020, 0.749020, 
    0.749020, 0.749020, 0.749020, 0.749020, 1.000000, 1.000000, 
    1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
    0.000000, 0.000000, 0.247059, 0.247059, 0.247059, 0.247059, 
    0.247059, 0.247059, 0.247059, 0.247059, 0.498039, 0.498039, 
    0.498039, 0.498039, 0.498039, 0.498039, 0.498039, 0.498039, 
    0.749020, 0.749020, 0.749020, 0.749020, 0.749020, 0.749020, 
    0.749020, 0.749020, 1.000000, 1.000000, 1.000000, 1.000000, 
    1.000000, 1.000000, 1.000000, 1.000000, 0.000000, 0.000000, 
    1.000000, 1.000000, 0.000000, 0.000000, 1.000000, 1.000000, 
    0.333333, 0.443137, 0.776471, 0.556863, 0.443137, 0.219608, 
    0.556863, 0.666667, 0.666667, 0.333333, 0.666667, 0.333333, 
    0.666667, 0.333333, 0.666667, 0.333333, 0.666667, 0.333333, 
    0.666667, 0.333333, 0.666667, 0.333333, 0.666667, 0.333333, 
    0.039216, 0.078431, 0.117647, 0.156863, 0.200000, 0.239216, 
    0.278431, 0.317647, 0.356863, 0.400000, 0.439216, 0.478431, 
    0.517647, 0.556863, 0.600000, 0.639216, 0.678431, 0.717647, 
    0.756863, 0.800000, 0.839216, 0.878431, 0.917647, 0.956863, 
    0.000000, 0.141176, 0.282353, 0.427451, 0.568627, 0.713726, 
    0.854902, 1.000000, 0.000000, 0.141176, 0.282353, 0.427451, 
    0.568627, 0.713726, 0.854902, 1.000000, 0.000000, 0.141176, 
    0.282353, 0.427451, 0.568627, 0.713726, 0.854902, 1.000000, 
    0.000000, 0.141176, 0.282353, 0.427451, 0.568627, 0.713726, 
    0.854902, 1.000000, 0.000000, 0.141176, 0.282353, 0.427451, 
    0.568627, 0.713726, 0.854902, 1.000000, 0.000000, 0.141176, 
    0.282353, 0.427451, 0.568627, 0.713726, 0.854902, 1.000000, 
    0.000000, 0.141176, 0.282353, 0.427451, 0.568627, 0.713726, 
    0.854902, 1.000000, 0.000000, 0.141176, 0.282353, 0.427451, 
    0.568627, 0.713726, 0.854902, 1.000000, 0.000000, 0.141176, 
    0.282353, 0.427451, 0.568627, 0.713726, 0.854902, 1.000000, 
    0.000000, 0.141176, 0.282353, 0.427451, 0.568627, 0.713726, 
    0.854902, 1.000000, 0.000000, 0.141176, 0.282353, 0.427451, 
    0.568627, 0.713726, 0.854902, 1.000000, 0.000000, 0.141176, 
    0.282353, 0.427451, 0.568627, 0.713726, 0.854902, 1.000000, 
    0.000000, 0.141176, 0.282353, 0.427451, 0.568627, 0.713726, 
    0.854902, 1.000000, 0.000000, 0.141176, 0.282353, 0.427451, 
    0.568627, 0.713726, 0.854902, 1.000000, 0.000000, 0.141176, 
    0.282353, 0.427451, 0.568627, 0.713726, 0.854902, 1.000000, 
    0.000000, 0.141176, 0.282353, 0.427451, 0.568627, 0.713726, 
    0.854902, 1.000000, 0.000000, 0.141176, 0.282353, 0.427451, 
    0.568627, 0.713726, 0.854902, 1.000000, 0.000000, 0.141176, 
    0.282353, 0.427451, 0.568627, 0.713726, 0.854902, 1.000000, 
    0.000000, 0.141176, 0.282353, 0.427451, 0.568627, 0.713726, 
    0.854902, 1.000000, 0.000000, 0.141176, 0.282353, 0.427451, 
    0.568627, 0.713726, 0.854902, 1.000000, 0.000000, 0.141176, 
    0.282353, 0.427451, 0.568627, 0.713726, 0.854902, 1.000000, 
    0.000000, 0.141176, 0.282353, 0.427451, 0.568627, 0.713726, 
    0.854902, 1.000000, 0.000000, 0.141176, 0.282353, 0.427451, 
    0.568627, 0.713726, 0.854902, 1.000000, 0.000000, 0.141176, 
    0.282353, 0.427451, 0.568627, 0.713726, 0.854902, 1.000000, 
    0.000000, 0.141176, 0.282353, 0.427451, 0.568627, 0.713726, 
    0.854902, 1.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
    1.000000, 1.000000, 1.000000, 1.000000, 0.333333, 0.443137, 
    0.443137, 0.219608, 0.776471, 0.556863, 0.556863, 0.666667, 
    0.666667, 0.333333, 0.666667, 0.333333, 0.666667, 0.333333, 
    0.666667, 0.333333, 0.666667, 0.333333, 0.666667, 0.333333, 
    0.666667, 0.333333, 0.666667, 0.333333, 0.039216, 0.078431, 
    0.117647, 0.156863, 0.200000, 0.239216, 0.278431, 0.317647, 
    0.356863, 0.400000, 0.439216, 0.478431, 0.517647, 0.556863, 
    0.600000, 0.639216, 0.678431, 0.717647, 0.756863, 0.800000, 
    0.839216, 0.878431, 0.917647, 0.956863, 0.000000, 0.000000, 
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
    0.000000, 0.000000, 0.247059, 0.247059, 0.247059, 0.247059, 
    0.247059, 0.247059, 0.247059, 0.247059, 0.247059, 0.247059, 
    0.247059, 0.247059, 0.247059, 0.247059, 0.247059, 0.247059, 
    0.247059, 0.247059, 0.247059, 0.247059, 0.247059, 0.247059, 
    0.247059, 0.247059, 0.247059, 0.247059, 0.247059, 0.247059, 
    0.247059, 0.247059, 0.247059, 0.247059, 0.247059, 0.247059, 
    0.247059, 0.247059, 0.247059, 0.247059, 0.247059, 0.247059, 
    0.498039, 0.498039, 0.498039, 0.498039, 0.498039, 0.498039, 
    0.498039, 0.498039, 0.498039, 0.498039, 0.498039, 0.498039, 
    0.498039, 0.498039, 0.498039, 0.498039, 0.498039, 0.498039, 
    0.498039, 0.498039, 0.498039, 0.498039, 0.498039, 0.498039, 
    0.498039, 0.498039, 0.498039, 0.498039, 0.498039, 0.498039, 
    0.498039, 0.498039, 0.498039, 0.498039, 0.498039, 0.498039, 
    0.498039, 0.498039, 0.498039, 0.498039, 0.749020, 0.749020, 
    0.749020, 0.749020, 0.749020, 0.749020, 0.749020, 0.749020, 
    0.749020, 0.749020, 0.749020, 0.749020, 0.749020, 0.749020, 
    0.749020, 0.749020, 0.749020, 0.749020, 0.749020, 0.749020, 
    0.749020, 0.749020, 0.749020, 0.749020, 0.749020, 0.749020, 
    0.749020, 0.749020, 0.749020, 0.749020, 0.749020, 0.749020, 
    0.749020, 0.749020, 0.749020, 0.749020, 0.749020, 0.749020, 
    0.749020, 0.749020, 1.000000, 1.000000, 1.000000, 1.000000, 
    1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 
    1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 
    1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 
    1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 
    1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 
    1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 
};
float glwRGBMap[8][3] = {
    {
	0, 0, 0
    },
    {
	1, 0, 0
    },
    {
	0, 1, 0
    },
    {
	1, 1, 0
    },
    {
	0, 0, 1
    },
    {
	1, 0, 1
    },
    {
	0, 1, 1
    },
    {
	1, 1, 1
    }
};

/******************************************************************************/


#if defined(DEC) && defined(vms)

static int ffs (unsigned int mask)
{
    int			num;
    unsigned int	bit;

    if (mask == 0) return 0;
    
    for (num = 1, bit = 1; (mask & bit) != 0; num++, bit <<= 1) {}

    return num;

}

#endif /* DEC */
/******************************************************************************/

int glwGetColorMapSize(glWindow w)
{

  return w->vInfoMain->colormap_size;
    
}
/******************************************************************************/

static int glwLoadFont(glWindow w,int isize)
{
  Font id;
  int first, last;
  XFontStruct *font_info;
  char fontname[128];
  if (debug) 
    {
      printf("Load font size %d\n",isize);

    }
  if ((isize<0)&& (isize>=GLW_MAX_FONT_SIZE))
    {
      glwError("font size out of range\n");
      return 0;
    }
  sprintf(fontname,GLW_FONT_NAME,isize);
  font_info = XLoadQueryFont(w->display, fontname);
  if (font_info == NULL) 
    {
      glwError("font not found\n");
      return 0;
    }

  id = font_info->fid;
  first = font_info->min_char_or_byte2;
  last = font_info->max_char_or_byte2;
  
  w->font_base[isize] = glGenLists((GLuint) last+1);
  w->font_range[isize]=last+1;
  if (w->font_base[isize] == 0) 
    {
      printf ("out of display lists\n");
      return 0;
    }
  
  glXUseXFont(id, first, last-first+1, w->font_base[isize]+first);
  /*    *height = font_info->ascent + font_info->descent;
   *width = font_info->max_bounds.width;  */
  return 1;
}



void glwPrint(glWindow w,char *s)
{

  if (!glwPrintFB(w,s))
    {
      if (w->font_base[w->font_to_use]==0) return;
      glPushAttrib (GL_LIST_BIT);
      glListBase(w->font_base[w->font_to_use]);
      glCallLists((int)strlen(s), GL_UNSIGNED_BYTE, (GLubyte *) s);
      glPopAttrib ();
    }
}

void glwPrintf(glWindow w,char *format,...)
{ 
  va_list ptr;
  va_start(ptr,format);
  vsprintf(glw_string_buffer,format,ptr);
  va_end(ptr);
  glwPrint(w,glw_string_buffer);
}


void glwSetFontSize(glWindow w, double font_size)
{
  int isize;
  isize=(int)(font_size*(double)w->h);
  if (isize<2) isize=2;
  if (isize>=GLW_MAX_FONT_SIZE) isize=GLW_MAX_FONT_SIZE-1;
  w->font_size=font_size;
  if (w->font_base[isize]==0) glwLoadFont(w,isize);
  w->font_to_use=isize;
}
/******************************************************************************/

void glwGetMouseLoc(glWindow w,int *x, int *y)
{
    int junk;

    *x = 0;
    *y = 0;
    XQueryPointer(w->display, w->wMain, (Window *)&junk, (Window *)&junk,
		  &junk, &junk, x, y, (unsigned int *)&junk);
}

/******************************************************************************/

void glwSetFogRamp(glWindow w,int density, int startIndex)
{
    XColor c[256];
    int rShift, gShift, bShift, intensity, fogValues, colorValues;
    int i, j, k;

    switch (w->vInfoMainClass) {
      case DirectColor:
	fogValues = 1 << density;
	colorValues = 1 << startIndex;
	for (i = 0; i < colorValues; i++) {
	    for (j = 0; j < fogValues; j++) {
		k = i * fogValues + j;
		intensity = i * fogValues + j * colorValues;
		if (intensity > w->vInfoMain->colormap_size) {
		    intensity = w->vInfoMain->colormap_size;
		}
		intensity = (intensity << 8) | intensity;
		rShift = ffs((unsigned int)w->vInfoMain->red_mask) - 1;
		gShift = ffs((unsigned int)w->vInfoMain->green_mask) - 1;
		bShift = ffs((unsigned int)w->vInfoMain->blue_mask) - 1;
		c[k].pixel = ((k << rShift) & w->vInfoMain->red_mask) |
			     ((k << gShift) & w->vInfoMain->green_mask) |
			     ((k << bShift) & w->vInfoMain->blue_mask);
		c[k].red = (unsigned short)intensity;
		c[k].green = (unsigned short)intensity;
		c[k].blue = (unsigned short)intensity;
		c[k].flags = DoRed | DoGreen | DoBlue;
	    }
	}
	XStoreColors(w->display, w->cMapMain, c, w->vInfoMain->colormap_size);
	break;
      case GrayScale:
      case PseudoColor:
	fogValues = 1 << density;
	colorValues = 1 << startIndex;
	for (i = 0; i < colorValues; i++) {
	    for (j = 0; j < fogValues; j++) {
		k = i * fogValues + j;
		intensity = i * fogValues + j * colorValues;
		if (intensity > w->vInfoMain->colormap_size) {
		    intensity = w->vInfoMain->colormap_size;
		}
		intensity = (intensity << 8) | intensity;
		c[k].pixel = k;
		c[k].red = (unsigned short)intensity;
		c[k].green = (unsigned short)intensity;
		c[k].blue = (unsigned short)intensity;
		c[k].flags = DoRed | DoGreen | DoBlue;
	    }
	}
	XStoreColors(w->display, w->cMapMain, c, w->vInfoMain->colormap_size);
	break;
    }

    XSync(w->display, 0);
}

/******************************************************************************/

void glwSetGreyRamp(glWindow w)
{
    XColor c[256];
    float intensity;
    int rShift, gShift, bShift, i;

    switch (w->vInfoMainClass) {
      case DirectColor:
	for (i = 0; i < w->vInfoMain->colormap_size; i++) {
	    intensity = (float)i / (float)w->vInfoMain->colormap_size *
			65535.0 + 0.5;
	    rShift = ffs((unsigned int)w->vInfoMain->red_mask) - 1;
	    gShift = ffs((unsigned int)w->vInfoMain->green_mask) - 1;
	    bShift = ffs((unsigned int)w->vInfoMain->blue_mask) - 1;
	    c[i].pixel = ((i << rShift) & w->vInfoMain->red_mask) |
			 ((i << gShift) & w->vInfoMain->green_mask) |
			 ((i << bShift) & w->vInfoMain->blue_mask);
	    c[i].red = (unsigned short)intensity;
	    c[i].green = (unsigned short)intensity;
	    c[i].blue = (unsigned short)intensity;
	    c[i].flags = DoRed | DoGreen | DoBlue;
	}
	XStoreColors(w->display, w->cMapMain, c, w->vInfoMain->colormap_size);
	break;
      case GrayScale:
      case PseudoColor:
	for (i = 0; i < w->vInfoMain->colormap_size; i++) {
	    intensity = (float)i / (float)w->vInfoMain->colormap_size *
			65535.0 + 0.5;
	    c[i].pixel = i;
	    c[i].red = (unsigned short)intensity;
	    c[i].green = (unsigned short)intensity;
	    c[i].blue = (unsigned short)intensity;
	    c[i].flags = DoRed | DoGreen | DoBlue;
	}
	XStoreColors(w->display, w->cMapMain, c, w->vInfoMain->colormap_size);
	break;
    }

    XSync(w->display, 0);
}

/******************************************************************************/

void glwSetOneColor(glWindow w,int index, float r, float g, float b)
{
    XColor c;
    int rShift, gShift, bShift;

    switch (w->vInfoMainClass) {
      case DirectColor:
	rShift = ffs((unsigned int)w->vInfoMain->red_mask) - 1;
	gShift = ffs((unsigned int)w->vInfoMain->green_mask) - 1;
	bShift = ffs((unsigned int)w->vInfoMain->blue_mask) - 1;
	c.pixel = ((index << rShift) & w->vInfoMain->red_mask) |
		  ((index << gShift) & w->vInfoMain->green_mask) |
		  ((index << bShift) & w->vInfoMain->blue_mask);
	c.red = (unsigned short)(r * 65535.0 + 0.5);
	c.green = (unsigned short)(g * 65535.0 + 0.5);
	c.blue = (unsigned short)(b * 65535.0 + 0.5);
	c.flags = DoRed | DoGreen | DoBlue;
	XStoreColor(w->display, w->cMapMain, &c);
	break;
      case GrayScale:
      case PseudoColor:
	if (index < w->vInfoMain->colormap_size) {
	    c.pixel = index;
	    c.red = (unsigned short)(r * 65535.0 + 0.5);
	    c.green = (unsigned short)(g * 65535.0 + 0.5);
	    c.blue = (unsigned short)(b * 65535.0 + 0.5);
	    c.flags = DoRed | DoGreen | DoBlue;
	    XStoreColor(w->display, w->cMapMain, &c);
	}
	break;
    }

    XSync(w->display, 0);
}

/******************************************************************************/

void glwSetOverlayMap(glWindow w, int size, float *rgb)
{
    XColor c;
    unsigned long *buf;
    int max, i;

    if (w->vInfoMainClass == PseudoColor) {
	max = (size > w->vInfoOverlay->colormap_size) ?
	      w->vInfoOverlay->colormap_size : size;
	buf = (unsigned long *)calloc(max, sizeof(unsigned long));
	XAllocColorCells(w->display, w->cMapOverlay, True, NULL, 0, buf, max-1);
	for (i = 1; i < max; i++) {
	    c.pixel = i;
	    c.red = (unsigned short)(rgb[i] * 65535.0 + 0.5);
	    c.green = (unsigned short)(rgb[size+i] * 65535.0 + 0.5);
	    c.blue = (unsigned short)(rgb[size*2+i] * 65535.0 + 0.5);
	    c.flags = DoRed | DoGreen | DoBlue;
	    XStoreColor(w->display, w->cMapOverlay, &c);
	}
	free(buf);
    }

    XSync(w->display, 0);
}


/******************************************************************************/

void glwSetRGBMap(glWindow w,int size, float *rgb)
{
    XColor c;
    int rShift, gShift, bShift, max, i;

    switch (w->vInfoMainClass) {
      case DirectColor:
	max = (size > w->vInfoMain->colormap_size) ? w->vInfoMain->colormap_size
						  : size;
	for (i = 0; i < max; i++) {
	    rShift = ffs((unsigned int)w->vInfoMain->red_mask) - 1;
	    gShift = ffs((unsigned int)w->vInfoMain->green_mask) - 1;
	    bShift = ffs((unsigned int)w->vInfoMain->blue_mask) - 1;
	    c.pixel = ((i << rShift) & w->vInfoMain->red_mask) |
		      ((i << gShift) & w->vInfoMain->green_mask) |
		      ((i << bShift) & w->vInfoMain->blue_mask);
	    c.red = (unsigned short)(rgb[i] * 65535.0 + 0.5);
	    c.green = (unsigned short)(rgb[size+i] * 65535.0 + 0.5);
	    c.blue = (unsigned short)(rgb[size*2+i] * 65535.0 + 0.5);
	    c.flags = DoRed | DoGreen | DoBlue;
	    XStoreColor(w->display, w->cMapMain, &c);
	}
	break;
      case GrayScale:
      case PseudoColor:
	max = (size > w->vInfoMain->colormap_size) ? w->vInfoMain->colormap_size
						  : size;
	for (i = 0; i < max; i++) {
	    c.pixel = i;
	    c.red = (unsigned short)(rgb[i] * 65535.0 + 0.5);
	    c.green = (unsigned short)(rgb[size+i] * 65535.0 + 0.5);
	    c.blue = (unsigned short)(rgb[size*2+i] * 65535.0 + 0.5);
	    c.flags = DoRed | DoGreen | DoBlue;
	    XStoreColor(w->display, w->cMapMain, &c);
	}
	break;
    }

    XSync(w->display, 0);
}



/******************************************************************************/

GLenum glwSetWindowLevel(glWindow w,GLenum level)
{

    switch (level) {
      case GLW_OVERLAY:
	if (GLW_HAS_OVERLAY(w->type)) {
	    if (!glXMakeCurrent(w->display, w->wOverlay, w->cOverlay)) {
		return GL_FALSE;
	    }
	} else {
	    return GL_FALSE;
	}
	break;
      case GLW_RGB:
      case GLW_INDEX:
	if (!glXMakeCurrent(w->display, w->wMain, w->cMain)) {
	    return GL_FALSE;
	}
	break;
    default:
	break;
    }
    return GL_TRUE;
}

void glwQuit(glWindow w)
{

 w->process=0;
}
/******************************************************************************/


static GLenum DoNextEvent(glWindow w)
{
    XEvent current, ahead;
    char buf[1000];
    KeySym ks;
    int key;

    XNextEvent(w->display, &current);
    switch (current.type) {
      case MappingNotify:
	XRefreshKeyboardMapping((XMappingEvent *)&current);
	w->lastEventType = MappingNotify;
	return GL_FALSE;

      case MapNotify:
	w->lastEventType = MapNotify;
	w->drawAllowFlag = GL_TRUE;
	return GL_FALSE;

      case UnmapNotify:
	w->lastEventType = UnmapNotify;
	w->drawAllowFlag = GL_FALSE;
	return GL_FALSE;
      
      case ClientMessage:
	w->lastEventType = ClientMessage;
	if (current.xclient.data.l[0] == w->deleteWindowAtom) {
	    exit(0);
	}
	return GL_FALSE;

      case Expose:
	while (XEventsQueued(current.xexpose.display, QueuedAfterReading) > 0) {
	    XPeekEvent(current.xexpose.display, &ahead);
	    if (ahead.xexpose.window != current.xexpose.window ||
		ahead.type != Expose) {
		break;
	    }
	    XNextEvent(w->display, &current);
	}
	if (current.xexpose.count == 0) {
	    if (w->ExposeFunc) {
#ifdef DEC /* bug fixes */
		(*ExposeFunc)(w.w, w.h);
		if (lastEventType == ConfigureNotify 
			&& drawAllowFlag == GL_TRUE) {
		    /* the last event must have generated a draw */
		    lastEventType = Expose;
		    return GL_FALSE;
		} else {
		    drawAllowFlag = GL_TRUE;
		    lastEventType = Expose;
		    return GL_TRUE;
		}
#else /* not DEC */
		w->drawAllowFlag = GL_TRUE;
		w->lastEventType = Expose;
		(*w->ExposeFunc)(w,w->info,w->w, w->h);
		if (w->lastEventType == ConfigureNotify) {
		    return GL_FALSE;
		} else {
		    return GL_TRUE;
		}
#endif /* not DEC */
	    }
	}
	return GL_FALSE;

      case ConfigureNotify:
	w->lastEventType = ConfigureNotify;
	w->w = current.xconfigure.width;
	w->h = current.xconfigure.height;
	glwSetFontSize(w, w->font_size);
	if (w->ReshapeFunc) {
	    (*w->ReshapeFunc)(w,w->info,w->w, w->h);
	    return GL_TRUE;
	} else {
	    return GL_FALSE;
	}

      case MotionNotify:
	w->lastEventType = MotionNotify;
	if (w->MouseMoveFunc) {
	    GLenum mask;

	    mask = (GLenum)0;
	    if (current.xmotion.state & Button1Mask)  mask |= GLW_LEFTBUTTON;
	    if (current.xmotion.state & Button2Mask)  mask |= GLW_MIDDLEBUTTON;
	    if (current.xmotion.state & Button3Mask)  mask |= GLW_RIGHTBUTTON;
	    if (current.xmotion.state & ShiftMask)    mask |= GLW_SHIFT;
	    if (current.xmotion.state & ControlMask)  mask |= GLW_CONTROL;
	    if (current.xmotion.state & Mod1Mask)     mask |= GLW_MOD1;
	    if (current.xkey.state & LockMask)        mask |= GLW_LOCK;
	    	    
	    return (*w->MouseMoveFunc)(w,w->info,current.xmotion.x, current.xmotion.y, mask);
	} else {
	    return GL_FALSE;
	}

      case ButtonPress:
	w->lastEventType = ButtonPress;
	if (w->MouseDownFunc) {
	    GLenum mask;

	    mask = (GLenum)0;
	    if (current.xbutton.button==1)  mask |= GLW_LEFTBUTTON;
	    if (current.xbutton.button==2)  mask |= GLW_MIDDLEBUTTON;
	    if (current.xbutton.button==3)  mask |= GLW_RIGHTBUTTON;
	    if (current.xbutton.state & ShiftMask)    mask |= GLW_SHIFT;
	    if (current.xbutton.state & ControlMask)  mask |= GLW_CONTROL;
	    if (current.xmotion.state & Mod1Mask)     mask |= GLW_MOD1;

	    return (*w->MouseDownFunc)(w,w->info,current.xbutton.x, current.xbutton.y, mask);
	} else {
	    return GL_FALSE;
	}
      case ButtonRelease:
        w->lastEventType = ButtonRelease;
        if (w->MouseUpFunc) {
            GLenum mask;
            mask = (GLenum) 0;
            if (current.xbutton.button==1)  mask |= GLW_LEFTBUTTON;
            if (current.xbutton.button==2)  mask |= GLW_MIDDLEBUTTON;
            if (current.xbutton.button==3)  mask |= GLW_RIGHTBUTTON;
            if (current.xbutton.state & ShiftMask)    mask |= GLW_SHIFT;
            if (current.xbutton.state & ControlMask)  mask |= GLW_CONTROL;
	    if (current.xmotion.state & Mod1Mask)     mask |= GLW_MOD1;

            return (*w->MouseUpFunc)(w,w->info,current.xbutton.x, current.xbutton.y, mask);
        } else {
            return GL_FALSE;
        }

      case KeyPress:
        w->lastEventType = KeyPress;
        XLookupString(&current.xkey, buf, sizeof(buf), &ks, 0);
        switch (ks) {
#define CaseKS(sym) case XK_##sym: key=GLW_##sym; break
	  CaseKS(0);
          CaseKS(1);
          CaseKS(2);
          CaseKS(3);
          CaseKS(4);
          CaseKS(5);
          CaseKS(6);
          CaseKS(7);
          CaseKS(8);
          CaseKS(9);
          CaseKS(A);
          CaseKS(B);
          CaseKS(C);
          CaseKS(D);
          CaseKS(E);
          CaseKS(F);
          CaseKS(G);
          CaseKS(H);
          CaseKS(I);
          CaseKS(J);
          CaseKS(K);
          CaseKS(L);
          CaseKS(M);
          CaseKS(N);
          CaseKS(O);
          CaseKS(P);
          CaseKS(Q);
          CaseKS(R);
          CaseKS(S);
          CaseKS(T);
          CaseKS(U);
          CaseKS(V);
          CaseKS(W);
          CaseKS(X);
          CaseKS(Y);
          CaseKS(Z);
          CaseKS(a);
          CaseKS(b);
          CaseKS(c);
          CaseKS(d);
          CaseKS(e);
          CaseKS(f);
          CaseKS(g);
          CaseKS(h);
          CaseKS(i);
          CaseKS(j);
          CaseKS(k);
          CaseKS(l);
          CaseKS(m);
          CaseKS(n);
          CaseKS(o);
          CaseKS(p);
          CaseKS(q);
          CaseKS(r);
          CaseKS(s);
          CaseKS(t);
          CaseKS(u);
          CaseKS(v);
          CaseKS(w);
          CaseKS(x);
          CaseKS(y);
          CaseKS(z);
	  CaseKS(space);
	  CaseKS(exclam);
	  CaseKS(quotedbl);
	  CaseKS(numbersign);
	  CaseKS(dollar);
	  CaseKS(percent);
	  CaseKS(ampersand);
	  CaseKS(apostrophe);
	  CaseKS(parenleft);
	  CaseKS(parenright);
	  CaseKS(asterisk);
	  CaseKS(plus);
	  CaseKS(comma);
	  CaseKS(minus);
	  CaseKS(period);
	  CaseKS(slash);
	  CaseKS(colon);
	  CaseKS(semicolon);
	  CaseKS(less);
	  CaseKS(equal);
	  CaseKS(greater);
	  CaseKS(question);
	  CaseKS(at);
	  CaseKS(bracketleft);
	  CaseKS(bracketright);
	  CaseKS(asciicircum);
	  CaseKS(underscore);
	  CaseKS(grave);
	  CaseKS(braceleft);
	  CaseKS(bar);
	  CaseKS(braceright);
	  CaseKS(asciitilde);
	  CaseKS(Return);
	  CaseKS(BackSpace);
	  CaseKS(Escape);
	  CaseKS(Left);
	  CaseKS(Up);
	  CaseKS(Right);
	  CaseKS(Down);
	  CaseKS(KP_Enter);
	  CaseKS(KP_Home);
	  CaseKS(KP_Left);
	  CaseKS(KP_Up);
	  CaseKS(KP_Right);
	  CaseKS(KP_Down);
	  CaseKS(KP_Page_Up);
	  CaseKS(KP_Page_Down);
	  CaseKS(KP_End);
	  CaseKS(KP_Begin);
	  CaseKS(KP_Insert);
	  CaseKS(KP_Delete);
	  CaseKS(KP_Divide);
	  CaseKS(KP_Multiply);
	  CaseKS(KP_Add);
	  CaseKS(KP_Subtract);
	  CaseKS(KP_Decimal);
	  CaseKS(KP_0);
	  CaseKS(KP_1);
	  CaseKS(KP_2);
	  CaseKS(KP_3);
	  CaseKS(KP_4);
	  CaseKS(KP_5);
	  CaseKS(KP_6);
	  CaseKS(KP_7);
	  CaseKS(KP_8);
	  CaseKS(KP_9);
	  CaseKS(F1);
	  CaseKS(F2);
	  CaseKS(F3);
	  CaseKS(F4);
	  CaseKS(F5);
	  CaseKS(F6);
	  CaseKS(F7);
	  CaseKS(F8);
	  CaseKS(F9);
	  CaseKS(F10);
	  CaseKS(F11);
	  CaseKS(F12);
	  CaseKS(Page_Up);
	  CaseKS(Page_Down);
	  CaseKS(Tab);
	  CaseKS(Home);
	  CaseKS(End);
	  CaseKS(Insert);
	  CaseKS(Delete);
	  CaseKS(Print);
	  CaseKS(Pause);
	  CaseKS(Scroll_Lock);
          default:              key = GL_FALSE;         break;
        }
	if (key && w->KeyDownFunc) {
	    GLenum mask;

	    mask = (GLenum)0;
	    if (current.xkey.state & ShiftMask)    mask |= GLW_SHIFT;
	    if (current.xkey.state & ControlMask)  mask |= GLW_CONTROL;
	    if (current.xkey.state & Mod1Mask)     mask |= GLW_MOD1;

	    return (*w->KeyDownFunc)(w,w->info,key, mask);
	} else {
	    return GL_FALSE;
	}
    }
    return GL_FALSE;
}


void glwSwapBuffers(glWindow w)
{
  if (w->swap) return;
  w->swap=1;
  if (!w->offscreen)
    glXSwapBuffers(w->display, w->wMain);
    
  glwDumpOnSwap(w);
  w->swap=0;

}


int glwAttach(glWindow w)
{
  if (CurrentWindow==w) return 1; 
  glwTrace("attach");
  if (w==NULL) { glwError("null window"); return GL_FALSE;}
  if (w->offscreen)
    {
	if (!glXMakeCurrent(w->display, w->glx_pixmap, w->cMain)) 
	  {
	    glwError("Unable to attach window!");
	    return 0;
	  }
    }
  else
    {
      if (!glXMakeCurrent(w->display, w->wMain, w->cMain)) 
	{
	  glwError("Unable to attach window!");
	  return 0;
	}
    }

  glwTrace("attach");
  CurrentWindow=w;
  return 1;
}

void glwFlush(glWindow w)
{
  glwTrace("flush");
  glXWaitGL();
  XFlush(w->display);
}
void glwProcess(glWindow w,void *info)
{

    GLenum need_redraw;
    int first=1;
    glwTrace("process");
    if (!glwAttach(w)) return;
    w->info=info;
    w->process=1;  
    if (w->control_mode==GLW_EVENT_DRIVEN ) 
	w->drawAllowFlag=(GLenum)1;  
		
    do	
	{
	    if (w->control_mode==GLW_APPLICATION_DRIVEN ) 
		{
		  glwTrace("redraw");
		  (*w->RedrawFunc)(w,w->info);
		    need_redraw = GL_FALSE;
		    if (!w->offscreen)
		    {
		      while (XPending(w->display)) 
			{
			  need_redraw |= DoNextEvent(w);
			}
		    }
		} 
	    else 
		{
		  if (first)
		    {
		      need_redraw=(GLenum)1;
		      first=0;
		    }
		  else
		    need_redraw = DoNextEvent(w);
		}
	    
	
	    if ( 
		w->drawAllowFlag && 
		(w->control_mode==GLW_EVENT_DRIVEN ) && 
		need_redraw && 
		w->process
		) 
		{
		  XDefineCursor(w->display, w->wMain,w->wait_cursor);
		  glwTrace("redraw");
		  (*w->RedrawFunc)(w,w->info);
		  XDefineCursor(w->display, w->wMain,w->user_cursor);
		}
	} 
    while (w->process);
    glXWaitGL();
    XFlush(w->display);
    w->drawAllowFlag = GL_FALSE;
    w->lastEventType= -1;
    glwTrace("process");
}

/******************************************************************************/

void glwSetRedrawFunc(glWindow w, void (*Func)(glWindow, void *))
{
    if (w==NULL) { glwError("null window"); return;}
    w->RedrawFunc = Func;
}


void glwSetControlMode(glWindow w, int mode)
{
    if (w==NULL) { glwError("null window"); return;}
    if (!w->offscreen)
      {
	w->control_mode=mode;
	if (w->control_mode==GLW_APPLICATION_DRIVEN)
	  XDefineCursor(w->display,w->wMain, None);
	else
	  XDefineCursor(w->display, w->wMain,w->user_cursor);
      }
}
/******************************************************************************/

void glwSetExposeFunc(glWindow w, void (*Func)(glWindow, void *,int, int))
{    
    if (w==NULL) { glwError("null window"); return;}
    w->ExposeFunc = Func;
}

/******************************************************************************/

void glwSetReshapeFunc(glWindow w,void (*Func)(glWindow, void *,int, int))
{
    if (w==NULL) { glwError("null window"); return;}

    w->ReshapeFunc = Func;
}

/******************************************************************************/

void glwSetDisplayFunc(glWindow w, void (*Func)(glWindow, void *))
{
  if (w==NULL) { glwError("null window"); return;}
  if (Func!=NULL)
    {
      w->RedrawFunc = Func;
      w->control_mode=GLW_EVENT_DRIVEN;
    }
  else
    w->control_mode=GLW_APPLICATION_DRIVEN;
  
}
/******************************************************************************/

void glwSetKeyDownFunc(glWindow w,GLenum (*Func)(glWindow, void *,int, GLenum))
{
    if (w==NULL) { glwError("null window"); return;}

    w->KeyDownFunc = Func;
}

/******************************************************************************/

void glwSetMouseDownFunc(glWindow w,GLenum (*Func)(glWindow, void *,int, int, GLenum))
{

   if (w==NULL) { glwError("null window"); return;}
   w->MouseDownFunc = Func;
}

/******************************************************************************/

void glwSetMouseUpFunc(glWindow w,GLenum (*Func)(glWindow, void *,int, int, GLenum))
{
    if (w==NULL) { glwError("null window"); return;}

    w->MouseUpFunc = Func;
}

/******************************************************************************/

void glwSetMouseMoveFunc(glWindow w,GLenum (*Func)(glWindow, void *,int, int, GLenum))
{
    if (w==NULL) { glwError("null window"); return;}

    w->MouseMoveFunc = Func;
}

/******************************************************************************/

void glwSetIdleFunc(glWindow w,void (*Func)(glWindow, void *))
{
  if (w==NULL) { glwError("null window"); return;}
  if (Func!=NULL)
    {
      w->RedrawFunc = Func;
      w->control_mode=GLW_APPLICATION_DRIVEN;
    }
  else
    w->control_mode=GLW_EVENT_DRIVEN;
}

/******************************************************************************/

void glwDestroy(glWindow w)
{
   int i;
   glwTrace("destroy:");
   if (glwAttach(w)) 
     {   
       glFlush();
       glFinish();
     }
   for (i=0;i<GLW_MAX_FONT_SIZE;i++)
     {
       if (w->font_base[i]!=0) glDeleteLists(w->font_base[i],w->font_range[i]);
     }
   
  glXMakeCurrent(w->display, w->wMain,NULL);
   
   if (GLW_HAS_OVERLAY(w->type)) 
     {
       glXDestroyContext(w->display, w->cOverlay);
       XDestroyWindow(w->display, w->wOverlay);
       XFreeColormap(w->display, w->cMapOverlay);
       XFree((char *)w->vInfoOverlay);
       glwTrace("destroy overlay");
       
     }
   if (w->offscreen)
     {
       glXDestroyGLXPixmap(w->display, w->glx_pixmap);
       XFreePixmap(w->display, w->pixmap);
       glwTrace("destroy pixmap");
     }
   else
     {
       glXDestroyContext(w->display, w->cMain);
       glwTrace("destroy xwindow");
     }
   XFreeColormap(w->display, w->cMapMain);
   XFree((char *)w->vInfoMain);
   XDestroyWindow(w->display, w->wMain);
   XCloseDisplay(w->display);
   if (w->title!=NULL) free(w->title);
   if (w->xtitle!=NULL) free(w->xtitle);
   if (w->state!=NULL) free(w->state);
   /* ????MEMORY LEAK   free(w);*/
   w=NULL;
   glwTrace("destroy window");
}


/******************************************************************************/
char *glwGetDefault(glWindow w,char *resource, char *dflt)
{
  char *data;
  data=XGetDefault(w->display,"gltools",resource);
  if (data==NULL) 
    return dflt;
  
    return data;
}

/******************************************************************************/
void glwGetWindowSize(glWindow w, int *width, int *height)
{
  *width=w->w;
  *height=w->h;
}
/******************************************************************************/
int glwIsMesa(glWindow w)
{
  return w->isMesa;
}

/******************************************************************************/
static glWindow wCreate=NULL;
static glWindow wEvent=NULL;

static void InitWindow(void)
{
  glWindow w;
  if (wCreate==NULL)
    {
      wCreate=(struct glWindowStruct *)calloc(sizeof(struct glWindowStruct),1);
      if (wCreate==NULL) 
	{
	  glwError("No space for window struct!");
	  return;
	}
    }
  else
    return;
  
  w=wCreate;
  w->display = XOpenDisplay(NULL);
  
  if (w->display==NULL) 
    {
      glwError("Unable to connect to display!");
      return;
    }
  
  w->x=atoi(glwGetDefault(w,"xOrigin","0"));
  w->y=atoi(glwGetDefault(w,"yOrigin","0"));;
  w->w=(atoi(glwGetDefault(w,"width","352"))/16)*16; /* GLW_MPEG1_PAL */
  w->h=(atoi(glwGetDefault(w,"height","288"))/16)*16; /* GLW_MPEG1_PAL */
  w->type=(GLenum)(GLW_RGB|GLW_DOUBLE|GLW_DIRECT|GLW_DEPTH);
  w->dmPolicy=(GLenum)GLW_MINIMUM_CRITERIA;
  w->title=(char*)malloc(GLW_NAMELEN);
  w->xtitle=(char*)malloc(GLW_NAMELEN);
  w->state=NULL;
  strcpy(w->title,glwGetDefault(w,"title","gltools"));
  w->control_mode=GLW_EVENT_DRIVEN;
  w->font_base[0]=0;
  w->keep_aspect=1;
  w->offscreen=0;

  glwInitFB(w);

  w->wToplevel=0;
  w->swap=0;
}

void glwInitDisplayMode(GLenum type)
{   
  InitWindow();
  wCreate->type = type;
}

void glwInitOffscreen(void)
{   
  InitWindow();
  wCreate->offscreen=1;
  wCreate->control_mode=GLW_APPLICATION_DRIVEN;
}

void glwInitDisplayModePolicy(GLenum type)
{
  InitWindow();
  wCreate->dmPolicy = type;
}

void glwInitToplevel(void *toplevel)
{
  InitWindow();
  wCreate->wToplevel=(Window)toplevel;
}

/******************************************************************************/

void glwInitPosition(int x, int y, int width, int height)
{
  
  InitWindow();
  wCreate->x = x;
  wCreate->y = y;
  wCreate->w = (width/16)*16;
  wCreate->h = (height/16)*16;
}

void glwInitPos(int x, int y)
{
  
  InitWindow();
  wCreate->x = x;
  wCreate->y = y;
}


void glwInitTitle(char *title)
{
  
  InitWindow();
  strcpy(wCreate->title,title);
}


void glwInitAspectKeeping(int yesno)
{
  wCreate->keep_aspect=yesno;
}

 
static int ErrorHandler(Display *display, XErrorEvent *event)
{
#ifdef glDebug
  char buf[80];
  glwError("Received X error!");
  printf(" Error code   : %d\n", event->error_code);
  printf(" Request code : %d\n", event->request_code);
  printf(" Minor code   : %d\n", event->minor_code);
  XGetErrorText(display, event->error_code, buf, 80);
  printf(" Error text   : %s\n", buf);
#endif  
  return 0;
}

static XVisualInfo *FindBestMainVisual(glWindow w,GLenum type)
{
    int list[32], i;

    i = 0;

    list[i++] = GLX_LEVEL;
    list[i++] = 0;

    if (GLW_IS_DOUBLE(type)) {
	list[i++] = GLX_DOUBLEBUFFER;
    }

    if (GLW_IS_RGB(type)) {
	list[i++] = GLX_RGBA;
	list[i++] = GLX_RED_SIZE;
	list[i++] = 1;
	list[i++] = GLX_GREEN_SIZE;
	list[i++] = 1;
	list[i++] = GLX_BLUE_SIZE;
	list[i++] = 1;
	if (GLW_HAS_ALPHA(type)) {
	    list[i++] = GLX_ALPHA_SIZE;
	    list[i++] = 1;
	}
	if (GLW_HAS_ACCUM(type)) {
	    list[i++] = GLX_ACCUM_RED_SIZE;
	    list[i++] = 1;
	    list[i++] = GLX_ACCUM_GREEN_SIZE;
	    list[i++] = 1;
	    list[i++] = GLX_ACCUM_BLUE_SIZE;
	    list[i++] = 1;
	    if (GLW_HAS_ALPHA(type)) {
		list[i++] = GLX_ACCUM_ALPHA_SIZE;
		list[i++] = 1;
	    }
	}
    } else if (GLW_IS_INDEX(type)) {
	list[i++] = GLX_BUFFER_SIZE;
	list[i++] = 1;
    }

    if (GLW_HAS_DEPTH(type)) {
	list[i++] = GLX_DEPTH_SIZE;
	list[i++] = 1;
    }

    if (GLW_HAS_STENCIL(type)) {
	list[i++] = GLX_STENCIL_SIZE;
	list[i++] = 1;
    }

    list[i] = (int)None;

    return glXChooseVisual(w->display, w->screen, list);
}

static XVisualInfo *FindExactMainVisual(glWindow w, GLenum type)
{
  int i, nvis, val, rval, gval, bval, aval;
  XVisualInfo *vis_list, *this_vis, *best_vis, sampleVis;
  int this_score, best_score;
  
  /* Get list of visuals for this screen */
  sampleVis.screen = w->screen;
  vis_list = XGetVisualInfo(w->display, VisualScreenMask, &sampleVis, &nvis);
  
  /* 
   * Loop through the visuals; find first one that matches the attr 
   * specified in type
   */
  best_score = -1; best_vis = NULL;
  for ( i = 0; i < nvis; i++ ) 
    {
      this_vis = &vis_list[i];
      if (debug) 
	fprintf(stderr,"glwin: testing visual id 0x%x: ", (unsigned)this_vis->visualid);
      /* Visual must be supported by GLX */
      if ( glXGetConfig(w->display, this_vis, GLX_USE_GL, &val) ) continue;
      if ( !val ) 
	{  
	  if (debug) fprintf(stderr,"no GLX support.\n");
	  continue;
	}
      /* Visual must be in main planes which is level 0 */
      glXGetConfig(w->display, this_vis, GLX_LEVEL, &val);
      if ( val != 0 ) 
	{  
	  if (debug) fprintf(stderr,"no level 0 visual.\n");
	  continue;
	}
      
      /* Color Index or RGBA? It must match the requested value */
      glXGetConfig(w->display, this_vis, GLX_RGBA, &val);
      if ( GLW_IS_RGB(type) && !val ) 
	{  
	  if (debug) fprintf(stderr,"no RGB support.\n");
	  continue;
	}
      
      if ( GLW_IS_INDEX(type) && val )
	{  
	  if (debug) fprintf(stderr,"no color index support.\n");
	  continue;
	}
      
      
      /* Double buffered or Single buffered? */
      glXGetConfig(w->display, this_vis, GLX_DOUBLEBUFFER, &val);
      if ( GLW_IS_DOUBLE(type) && !val )
	{  
	  if (debug) fprintf(stderr,"no double buffer support.\n");
	  continue;
	}
      if ( GLW_IS_SINGLE(type) && val ) 
	{  
	  if (debug) fprintf(stderr,"no single buffer support.\n");
	  continue;
	}

      
      /* If accum requested then accum rgb size must be > 0 */
      /* If alpha requested then alpha size must be > 0 */
      /* if accum & alpha requested then accum alpha size must be > 0 */
      if ( GLW_IS_RGB(type) ) 
	{
	  glXGetConfig(w->display, this_vis, GLX_ACCUM_RED_SIZE, &rval);
	  glXGetConfig(w->display, this_vis, GLX_ACCUM_GREEN_SIZE, &gval);
	  glXGetConfig(w->display, this_vis, GLX_ACCUM_BLUE_SIZE, &bval);
	 
	  glXGetConfig(w->display, this_vis, GLX_ACCUM_ALPHA_SIZE, &aval);
	  if ( GLW_HAS_ACCUM(type) ) 
	    {
	      if ( rval <= 0 || gval <= 0 || bval <= 0 ) 
		{  
		  if (debug) fprintf(stderr,"accum RGB size <0.\n");
		  continue;
		}
	    } 
	  else 
	    {
	      if ( rval > 0 || gval > 0 || bval > 0 || aval > 0 )
		{  
		  if (debug) fprintf(stderr,"accum support not demanded.\n");
		  continue;
		}
	    }
	
	  glXGetConfig(w->display, this_vis, GLX_ALPHA_SIZE, &val);
	  if ( GLW_HAS_ALPHA(type) ) 
	    {
	      if ( val <= 0 )
		{  
		  if (debug) fprintf(stderr,"alpha size < 0.\n");
		  continue;
		}

	    if ( GLW_HAS_ACCUM(type) && aval <= 0 ) 
	      {
		if (debug) fprintf(stderr,"alpha accum size < 0.\n");
		continue;
	      }
	    }
	  else
	    {
	      if ( val > 0 ) 
	      {
		if (debug) fprintf(stderr,"alpha support not demanded.\n");
		continue;
	      }
	    }
	
	}
      
      /* Check depth buffer */
      glXGetConfig(w->display, this_vis, GLX_DEPTH_SIZE, &val);
      if ( GLW_HAS_DEPTH(type) ) 
	{
	  if ( val <= 0 )
	    {
	      if (debug) fprintf(stderr,"no depth buffer support.\n");
	      continue;
	    }
	} 
      else
	{
	  if ( val > 0 )
	    {
	      if (debug) fprintf(stderr,"depth buffer support not demanded.\n");
	      continue;
	    }
	}
      
      /* Check stencil buffer */
      glXGetConfig(w->display, this_vis, GLX_STENCIL_SIZE, &val);
      if ( GLW_HAS_STENCIL(type) )
	{
	  if ( val <= 0 )
	    {
	      if (debug) fprintf(stderr,"no stencil buffer support.\n");
	      continue;
	    }
	} 
      else
	{
	  if ( val > 0 )
	    {
	      if (debug) fprintf(stderr,"stencil buffer support not demanded.\n");
	      continue;
	    }
	}
      
      glXGetConfig(w->display, this_vis, GLX_BUFFER_SIZE, &this_score);
      
      if (debug) fprintf(stderr,"ok, buffer size=%d.\n",this_score);
      if (this_score > best_score ) 
	{
	  best_score = this_score;
	  best_vis = this_vis;
	}
      
    }
  
  if ( best_vis ) 
    {
      sampleVis.visualid = best_vis->visualid;
      sampleVis.screen = w->screen;
      if ( nvis > 0 ) XFree(vis_list);
      return XGetVisualInfo(w->display, VisualIDMask|VisualScreenMask, 
			    &sampleVis, &nvis);
    } 
  else 
    {
      if ( nvis > 0 ) XFree(vis_list);
      return None;
    }
}

/*
old code 

static XVisualInfo *FindMainVisual(glWindow w,GLenum type)
{
  int list[32], i;
  i = 0;
  
  list[i++] = GLX_LEVEL;
  list[i++] = 0;
  
  if (GLW_IS_DOUBLE(type)) 
    {
      list[i++] = GLX_DOUBLEBUFFER;
    }

  if (GLW_IS_RGB(type)) 
    {
      list[i++] = GLX_RGBA;
      list[i++] = GLX_RED_SIZE;
      list[i++] = 1;
      list[i++] = GLX_GREEN_SIZE;
      list[i++] = 1;
      list[i++] = GLX_BLUE_SIZE;
      list[i++] = 1;

      if (GLW_HAS_ALPHA(type)) 
	{
	  list[i++] = GLX_ALPHA_SIZE;
	  list[i++] = 1;
	}
      
      if (GLW_HAS_ACCUM(type)) 
	{
	  list[i++] = GLX_ACCUM_RED_SIZE;
	  list[i++] = 1;
	  list[i++] = GLX_ACCUM_GREEN_SIZE;
	  list[i++] = 1;
	  list[i++] = GLX_ACCUM_BLUE_SIZE;
	  list[i++] = 1;
	  if (GLW_HAS_ALPHA(type)) {
	    list[i++] = GLX_ACCUM_ALPHA_SIZE;
	    list[i++] = 1;
	  }
	}
    }
  else if (GLW_IS_INDEX(type)) 
    {
      list[i++] = GLX_BUFFER_SIZE;
      list[i++] = 1;
    }
  
  if (GLW_HAS_DEPTH(type)) 
    {
      list[i++] = GLX_DEPTH_SIZE;
      list[i++] = 1;
    }
  
  if (GLW_HAS_STENCIL(type)) 
    {
      list[i++] = GLX_STENCIL_SIZE;
      list[i++] = 1;
    }
  
  list[i] = (int)None;
  return glXChooseVisual(w->display, w->screen, list);
}
*/

static XVisualInfo *FindOverlayVisual(glWindow w)
{
    int list[3];

    list[0] = GLX_LEVEL;
    list[1] = 1;
    list[2] = (int)None;
    return glXChooseVisual(w->display, w->screen, list);
}

static GLenum GetMainWindowType(glWindow w,XVisualInfo *vi)
{
    GLenum mask;
    int x, y, z;

    mask = (GLenum)0;

    glXGetConfig(w->display, vi, GLX_DOUBLEBUFFER, &x);
    if (x) {
	mask |= GLW_DOUBLE;
    } else {
	mask |= GLW_SINGLE;
    }

    glXGetConfig(w->display, vi, GLX_RGBA, &x);
    if (x) {
	mask |= GLW_RGB;
	glXGetConfig(w->display, vi, GLX_ALPHA_SIZE, &x);
	if (x > 0) {
	    mask |= GLW_ALPHA;
	}
	glXGetConfig(w->display, vi, GLX_ACCUM_RED_SIZE, &x);
	glXGetConfig(w->display, vi, GLX_ACCUM_GREEN_SIZE, &y);
	glXGetConfig(w->display, vi, GLX_ACCUM_BLUE_SIZE, &z);
	if (x > 0 && y > 0 && z > 0) {
	    mask |= GLW_ACCUM;
	}
    } else {
	mask |= GLW_INDEX;
    }

    glXGetConfig(w->display, vi, GLX_DEPTH_SIZE, &x);
    if (x > 0) {
	mask |= GLW_DEPTH;
    }

    glXGetConfig(w->display, vi, GLX_STENCIL_SIZE, &x);
    if (x > 0) {
	mask |= GLW_STENCIL;
    }

    if (glXIsDirect(w->display, w->cMain)) {
	mask |= GLW_DIRECT;
    } else {
	mask |= GLW_INDIRECT;
    }

    return mask;
}

static XVisualInfo *get_visual_from_id( Display *dpy, int scr,int id)
{
  XVisualInfo temp,*xtemp;
  long mask;
  int n;
  int default_depth;
  int default_class;
  
   mask = VisualIDMask;
 
    temp.visualid = id;
  
  xtemp= XGetVisualInfo( dpy, mask, &temp, &n );
  return xtemp;

}


static int WaitForMainWindow(Display *d, XEvent *e, char *arg)
{

    if (e->type == MapNotify && e->xmap.window == wEvent->wMain) {
	return GL_TRUE;
    } else {
	return GL_FALSE;
    }
}

/*
static int WaitForOverlayWindow(Display *d, XEvent *e, char *arg)
{

    if (e->type == MapNotify && e->xmap.window == wEvent->wOverlay) {
	return GL_TRUE;
    } else {
	return GL_FALSE;
    }
    return GL_FALSE;
}
*/
static void default_expose_func(glWindow w, 
			 void *user_data,
			 int width, 
			 int height)
{
  
  /* force redraw if no user drivers set */

}

static void create_window(glWindow w, int overlayFlag)
{
  XSetWindowAttributes wa;
  XTextProperty tp;
  XSizeHints sh;
  XEvent e;

  wa.colormap = w->cMapMain;
  wa.background_pixmap = None;
  wa.border_pixel = 0;
  wa.backing_store=Always;
  wa.save_under=True;
  wa.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask |
    ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
  w->wMain = XCreateWindow(w->display, w->root, w->x, w->y, w->w, w->h, 0,
			   w->vInfoMain->depth, InputOutput,
			   w->vInfoMain->visual,
			   CWBackPixmap|CWBorderPixel|CWEventMask|CWColormap,
			   &wa);
  
  XStringListToTextProperty(&w->title, 1, &tp);
  sh.flags = USPosition | USSize | PMaxSize |PResizeInc;
  sh.width_inc=16;
  sh.height_inc=16;
  if (w->keep_aspect)
    {
      
      sh.flags|=PAspect;
      sh.min_aspect.x=w->w;
      sh.min_aspect.y=w->h;
      
      sh.max_aspect.x=w->w;
      sh.max_aspect.y=w->h;
    }
  
  {
    Screen *scr = DefaultScreenOfDisplay(w->display);
    sh.max_width=WidthOfScreen(scr);
    sh.max_height=HeightOfScreen(scr);
  }
  
  if (w->wToplevel!=0) 
    XSetTransientForHint(w->display, w->wMain, w->wToplevel);
  XSetWMProperties(w->display, w->wMain, &tp, &tp, 0, 0, &sh, 0, 0);
  w->deleteWindowAtom = XInternAtom(w->display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(w->display, w->wMain, &w->deleteWindowAtom, 1);
  XMapWindow(w->display, w->wMain);
  w->drawAllowFlag = GL_FALSE;
  w->lastEventType= -1;
  wEvent=w;
  XIfEvent(w->display, &e, WaitForMainWindow, 0);
  
  if (overlayFlag == GL_TRUE) {
    w->vInfoOverlay = FindOverlayVisual(w);
    if (w->vInfoOverlay) {
      w->cOverlay = glXCreateContext(w->display, w->vInfoOverlay, None,
				     GL_TRUE);
      w->cMapOverlay = XCreateColormap(w->display, w->root,
				       w->vInfoOverlay->visual, AllocNone);
      glwSetOverlayMap(w,256, colorMaps);
      wa.colormap = w->cMapOverlay;
      wa.background_pixmap = None;
      wa.border_pixel = 0;
      w->wOverlay = XCreateWindow(w->display, w->wMain, 0, 0, w->w, w->h, 0,
				  w->vInfoOverlay->depth, InputOutput,
				       w->vInfoOverlay->visual,
				  CWBackPixmap|CWBorderPixel|CWColormap,
				  &wa);
      XMapWindow(w->display, w->wOverlay);
      XSetWMColormapWindows(w->display, w->wMain, &w->wOverlay, 1);
      w->type |= GLW_OVERLAY;
    } else {
      glwError("Unable to create overlay plane!");
    }
  }

  w->wait_cursor=XCreateFontCursor(w->display,XC_watch);
  w->user_cursor=XCreateFontCursor(w->display,XC_cross);
  XDefineCursor(w->display,w->wMain, w->user_cursor);

}

static void create_pixmap(glWindow w, int overlayFlag)
{
  w->pixmap = XCreatePixmap(w->display, 
			    w->root,
			    w->w, 
			    w->h, 
			    w->vInfoMain->depth);
  if (w->pixmap==0)
    glwError("Unable to create pixmap!");
  else
    glwTrace("pixmap ok");

  w->glx_pixmap =glXCreateGLXPixmap(w->display,
				   w->vInfoMain,
				   w->pixmap);
  if (w->glx_pixmap==0)
    glwError("Unable to create glx_pixmap!");
  else
    glwTrace("glxpixmap ok");
    
}

glWindow glwCreate(void)
{
  glWindow w;
  int erb, evb;
  GLenum overlayFlag;
    
  InitWindow();
  assert(wCreate!=NULL);
  w=wCreate;
  wCreate=0;
  if (!glXQueryExtension(w->display, &erb, &evb))
    {
      glwError("No glx extension found!");
      return NULL;
    }
  w->screen = DefaultScreen(w->display);
  w->root = RootWindow(w->display, w->screen);
  XSetErrorHandler(ErrorHandler);

  if (w->offscreen) w->type |= GLW_INDIRECT;
  
  if (GLW_HAS_OVERLAY(w->type))
      {
	overlayFlag = GL_TRUE;
      } 
  else 
    {
      overlayFlag = GL_FALSE;
    }
  w->type &= ~GLW_OVERLAY;
  
  
  if (w->dmPolicy == GLW_MINIMUM_CRITERIA)
    w->vInfoMain = FindBestMainVisual(w,w->type);
  else if (w->dmPolicy == GLW_EXACT_MATCH)
    w->vInfoMain = FindExactMainVisual(w,w->type);
   if (!w->vInfoMain)
    {
      char*vid;
      if (vid=getenv("GLTOOLS_VISUAL"))
	  w->vInfoMain=get_visual_from_id(w->display, w->screen,atoi(vid));
   if (!w->vInfoMain)
     {
      glwError("Main visual not found!");
      return NULL;
     }
    }
  
  if (debug) fprintf(stderr,"glwin: using  visual id 0x%x\n", (unsigned)w->vInfoMain->visualid);
  
#if defined(__cplusplus) || defined(c_plusplus)
  w->vInfoMainClass = w->vInfoMain->c_class;
#else
  w->vInfoMainClass = w->vInfoMain->class;
#endif  
  
  w->cMain = glXCreateContext(w->display, w->vInfoMain, None,
			      (GLW_IS_DIRECT(w->type))?GL_TRUE:GL_FALSE);
  if (!w->cMain) 
    {
      glwError("Unable to create glx context!");
      return NULL;
    }

  w->type = GetMainWindowType(w,w->vInfoMain);
  
  if (GLW_IS_INDEX(w->type)) {
    if (w->vInfoMainClass != StaticColor &&
	w->vInfoMainClass != StaticGray) {
      w->cMapMain = XCreateColormap(w->display, w->root, w->vInfoMain->visual,
				    AllocAll);
    } else {
      w->cMapMain = XCreateColormap(w->display, w->root, w->vInfoMain->visual,
				    AllocNone);
    }
  } else {
    
    /* alt ging schief fuer MESA auf Xterm 
       w->cMapMain = XCreateColormap(w->display, w->root, w->vInfoMain->visual,
       AllocNone);
       */	
    /* RGB colormap is AllocNone, share the root colormap if possible */
    Screen *scr = DefaultScreenOfDisplay(w->display);
    if (MaxCmapsOfScreen(scr)==1
	&& w->vInfoMain->visual==DefaultVisual(w->display,w->screen)) {
      w->cMapMain = DefaultColormap(w->display,w->screen);
    }
    else {
      w->cMapMain = XCreateColormap(w->display, w->root, w->vInfoMain->visual,
				    AllocNone);
    }
  }
/* PRE-MESA
   glwSetRGBMap(w,256, colorMaps);
   */   
  
  if (GLW_IS_INDEX(w->type) || w->vInfoMainClass==DirectColor) {
    glwSetRGBMap(w,256, colorMaps);
  }
  
  if (w->offscreen)
    create_pixmap(w,overlayFlag);
  else
    create_window(w,overlayFlag);

  

  if (!glwAttach(w))
    {
      glwDestroy(w);
      return NULL;	  
    }
 
  w->isMesa=
    (strstr(glXGetClientString(w->display,GLX_VENDOR),"Mesa")!=NULL)||
    (strstr(glXGetClientString(w->display,GLX_VERSION),"Mesa")!=NULL)||
    (strstr(glXQueryServerString(w->display,w->screen,GLX_VENDOR),"Mesa")!=NULL)||
    (strstr(glXQueryServerString(w->display,w->screen,GLX_VERSION),"Mesa")!=NULL);

  glwSetFontSize(w,GLW_DEFAULT_FONT_SIZE);
  glwSetExposeFunc(w,default_expose_func);
  glwTrace("window ok");
  return w;
}

static void update_title(glWindow w)
{
  XTextProperty tp;
  if (w->offscreen) return;
  if (w->state!=NULL) 
    {
      sprintf(w->xtitle,"%s [ %s ]",w->title,w->state);
      XStringListToTextProperty(&w->xtitle, 1, &tp);
    }
    else
      XStringListToTextProperty(&w->title, 1, &tp);
  
  XSetWMProperties(w->display, w->wMain, &tp, 0, 0, 0, 0, 0, 0);
  XFlush(w->display);
}

void glwShowState(glWindow w, char *state)
{
 if (state!=NULL)
   { 
     if (w->state==NULL)
       w->state=(char*)malloc(GLW_NAMELEN);
     strcpy(w->state,state);
   }
 else
  {
    free(w->state);
    w->state=NULL;
  }
 update_title(w);
}

void glwSetTitle(glWindow w, char *title)
{
  strcpy(w->title,title);
  update_title(w);
}

char * glwGetTitle(glWindow w)
{
  return w->title;
}

#endif
/*
 * $Log: glwin.c,v $
 * Revision 2.57  2001/05/04 15:19:17  fuhrmann
 * pardiso preconditioner
 *
 * Revision 2.56  2000/12/10 23:04:44  fuhrmann
 * macro order corrected
 *
 * Revision 2.55  2000/12/10 22:58:09  fuhrmann
 * gleps bug
 * flglwin proto works
 *
 * Revision 2.54  2000/12/08 16:39:43  fuhrmann
 * first steps to fltk
 *
 * Revision 2.53  2000/11/22 15:20:49  fuhrmann
 * ICE hack
 * 1 mpeg/eps per variation
 * variation[i]-> variationi
 * bias=... option
 *
 * Revision 2.52  2000/11/20 11:10:23  fuhrmann
 * eps graphics stuff
 *
 * Revision 2.51  2000/10/25 10:49:59  fuhrmann
 * crop stuff for graphics
 *
 * Revision 2.50  2000/10/19 09:54:59  fuhrmann
 * scl graphics stuff
 *
 * Revision 2.49  2000/07/27 12:43:36  schmelze
 * *** empty log message ***
 *
 * Revision 2.48  2000/03/06 14:54:13  fuhrmann
 * removed toplevel info output
 *
 * Revision 2.47  2000/02/22 17:27:06  fuhrmann
 * * transient_for in glwin
 * * 0.1a Version of WIASTwoPhase
 *
 * Revision 2.46  2000/01/13 17:36:41  fuhrmann
 * optmized ps output
 *
 * Revision 2.45  2000/01/11 16:33:41  fuhrmann
 * name buffer sizes
 *
 * Revision 2.44  1999/11/12 19:50:11  fuhrmann
 * no abort for X errors
 *
 * Revision 2.43  1999/11/10 12:17:21  fuhrmann
 * sysconlaw.lua, working solve/replay stuff, Append protocol etc.
 *
 * Revision 2.42  1999/11/04 17:16:09  philip
 * string memory bug in glwin
 * roundoff error in colors in gleps
 *
 * Revision 2.41  1999/11/04 16:43:40  philip
 * removed calloc bug
 *
 * Revision 2.40  1999/11/01 18:31:24  fuhrmann
 * some problem with glXmakecurrent
 *
 * Revision 2.39  1999/09/17 16:35:01  fuhrmann
 * feedback stuff war kaputtrepariert...
 *
 * Revision 2.38  1999/09/15 16:16:24  fuhrmann
 * size_t for feedback buffer
 * file !=NULL check for dump files
 *
 * Revision 2.37  1999/08/19 15:31:46  fuhrmann
 * strange memory leak in glwDestroy !!! STILL THERE !!!
 *
 * Revision 2.36  1999/07/02 10:38:05  fuhrmann
 * Mesa bug neutralized, gleps reference taken out of glwin
 *
 * Revision 2.35  1999/06/30 13:07:50  fuhrmann
 * font size stuff
 *
 * Revision 2.34  1999/06/30 10:55:45  fuhrmann
 * no exit at token errors
 *
 * Revision 2.33  1999/06/30 10:27:25  fuhrmann
 * some bells &whistles in feedback stuff
 *
 * Revision 2.32  1999/06/29 17:56:24  fuhrmann
 * Feedback buffer is used now...
 *
 * Revision 2.31  1999/04/20 17:32:16  fuhrmann
 * sgi warnings, 3D flux test
 *
 * Revision 2.30  1999/04/13 11:43:31  fuhrmann
 * more thorough checks, k&r stuff deleted
 *
 * Revision 2.29  1999/04/08 15:03:15  fuhrmann
 * glmesh split up
 *
 * Revision 2.28  1998/09/04  15:25:48  fuhrmann
 * skip frame option in StartRecording
 *
 * Revision 2.27  1998/07/09  14:39:52  fuhrmann
 * initialization bug in mpeg stuff removed
 *
 * Revision 2.26  1998/07/06  10:45:12  fuhrmann
 * Increased compiler warning level for all PDELIB-1.7 modules,
 * removed all bugs, some warnings remain (rcsids, ld_init, unused parameters)
 *
 * Revision 2.25  1998/05/18  17:19:27  fuhrmann
 * if file given in mpeg param ressource is not existent then invoke
 * default mechanism.
 *
 * Revision 2.24  1998/03/11  19:21:04  fuhrmann
 * Hey we can create batch videos! (still beta, but...)
 *
 * Revision 2.23  1998/03/06  16:45:31  fuhrmann
 * tiff->ppm
 * print command
 * video recording
 *
 * Revision 2.22  1998/03/05  20:01:24  fuhrmann
 * interim test version with SGI movie lib : it was a mess...
 *
 * Revision 2.21  1998/01/26  11:07:28  schmelze
 * tests from external working directory, box*test.html
 *
 * Revision 2.20  1998/01/14  18:27:34  fuhrmann
 * Merge mit Ilja
 *
 * Revision 2.19  1997/11/27  19:03:09  fuhrmann
 * glwRecord stuff, PAL-Format, tex-file for keys
 *
 * Revision 2.18  1997/11/04  15:53:30  fuhrmann
 * color scales ; crash with glrnd
 *
 * Revision 2.17  1997/10/17  16:54:58  fuhrmann
 * glwFlush
 *
 * Revision 2.16  1997/10/01  15:41:27  fuhrmann
 * GLW_MOD1
 *
 * Revision 2.15  1997/09/09  17:42:28  fuhrmann
 * cursor stuff
 *
 * Revision 2.14  1997/04/15  08:10:22  fuhrmann
 * still not clear which font to load...
 *
 * Revision 2.13  1997/03/20  18:19:18  fuhrmann
 * glwGrab in no-tiff case
 *
 * Revision 2.12  1997/02/28  17:41:13  fuhrmann
 * glwGrab code
 *
 * Revision 2.11  1997/01/27  14:30:45  fuhrmann
 * other default font
 *
 * Revision 2.10  1996/10/09  16:33:57  fuhrmann
 * keys next, prev, tab
 *
 * Revision 2.9  1996/10/02  11:12:05  fuhrmann
 * some debugging in visual stuff, no change of algorithm
 *
 * Revision 2.8  1996/09/23  17:47:27  fuhrmann
 * -DUSE_OS_MESA for mesa offscreen contexts
 *
 * Revision 2.7  1996/09/19  16:57:34  fuhrmann
 * typo in glwProcess removed
 *
 * Revision 2.6  1996/09/19  16:33:39  fuhrmann
 * GLW_DEPTH in default mode
 * redraw first when enterinh glw_process in event mode
 *
 * Revision 2.5  1996/09/03  09:22:41  fuhrmann
 * tiffio.h location changed
 *
 * Revision 2.4  1996/09/03  09:09:04  fuhrmann
 * - fonts & printing characters
 * - keep aspect ratio
 * - dump pixmap size can default to window size
 *
 * Revision 2.3  1996/08/30  10:47:34  fuhrmann
 * Default expose function set (glw should redraw in event driven
 * mode if user has no callbacks set)
 *
 * Revision 2.2  1996/04/17  16:47:56  fuhrmann
 * error in off-screen rendering: PixelAlignment in GL buf was wrong
 * error in off-screen rendering: wrong count of lines (black upper line in GL dump) fixed
 *
 * Revision 2.1  1996/03/12  09:49:27  fuhrmann
 * Off-screen rendering into memory with large pixmaps using osmesa
 *
 * Revision 2.0  1996/02/15  20:26:14  fuhrmann
 * First meta-stable distribution
 *
 * Revision 1.9  1996/02/15  20:25:00  fuhrmann
 * - Error in return value for glwDump removed
 *
 * Revision 1.8  1996/02/15  14:18:20  fuhrmann
 * - improved behaviour when tiff file not opened
 *
 * Revision 1.7  1996/02/09  16:45:02  fuhrmann
 * - shift keys now part of button mask
 * - more keysyms available
 *
 * Revision 1.6  1996/02/06  09:59:46  fuhrmann
 * - glwDump ( depends on libtiff.a !)
 * - glwSetIdleFunc and glwSetDisplayFunc obsolete,
 *   use glwSetRedrawFunc & glwSetControlMode (better to understand)
 *
 * Revision 1.5  1996/01/30  18:36:15  fuhrmann
 * Changed default title to gltools-1.0
 *
 * Revision 1.4  1995/11/08  12:05:39  fuhrmann
 * glwSetTitle introduced
 *
 * Revision 1.3  1995/10/24  18:34:15  fuhrmann
 * Namelen von 64 auf 128  ???
 * Merge mit libtk 1.1 (dmPolicy stuff) vielleicht unnoetig...
 *
 * Revision 1.2  1995/10/24  12:40:51  fuhrmann
 * Zeilennr in glwError
 *
 * Revision 1.1  1995/10/20  15:42:03  fuhrmann
 * Initial revision
 *
 */
