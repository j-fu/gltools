/* This is a local copy of xglinfo, see below.*/


/*
 * HISTORY
 * $Log: xglinfo.c,v $
 * Revision 2.2  1998/07/06 10:45:12  fuhrmann
 * Increased compiler warning level for all PDELIB-1.7 modules,
 * removed all bugs, some warnings remain (rcsids, ld_init, unused parameters)
 *
 * Revision 2.1  1996/10/02  11:11:05  fuhrmann
 * running
 *
 * Revision 2.0  1996/10/02  11:09:24  fuhrmann
 *  Zusammenkopie der Netzquellen
 *
 */


/****************** global.h *********************************/
/*
 * 
 *           Copyright (c) Digital Equipment Corporation, 1993
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute  this software and its
 * documentation for any  purpose   and without fee  is  hereby  granted,
 * provided that the above copyright notice appear in all copies and that
 * both  that  copyright  notice  and  this  permission notice appear  in
 * supporting documentation, and that the name of Digital  not be used in
 * advertising or publicity  pertaining to distribution  of the  software
 * without specific, written prior permission.
 * 
 * DIGITAL DISCLAIMS   ALL  WARRANTIES WITH   REGARD   TO  THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT   SHALL  DIGITAL  BE   LIABLE  FOR  ANY SPECIAL,   INDIRECT   OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION  OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 * 
 */
/*
 * HISTORY
 * $Log: xglinfo.c,v $
 * Revision 2.2  1998/07/06 10:45:12  fuhrmann
 * Increased compiler warning level for all PDELIB-1.7 modules,
 * removed all bugs, some warnings remain (rcsids, ld_init, unused parameters)
 *
 * Revision 2.1  1996/10/02  11:11:05  fuhrmann
 * running
 *
 * Revision 2.0  1996/10/02  11:09:24  fuhrmann
 *  Zusammenkopie der Netzquellen
 *
 * Revision 1.1.2.3  1993/06/18  15:02:48  John_Dennis
 * 	change copyright token to free copyright token
 * 	[1993/06/18  14:59:24  John_Dennis]
 *
 * Revision 1.1.2.2  1993/06/16  20:33:25  John_Dennis
 * 	various lint clean ups
 * 	[1993/06/16  20:13:33  John_Dennis]
 * 
 * 	Initial Version by John Dennis
 * 	[1993/06/15  20:07:16  John_Dennis]
 * 
 * $EndLog$
 */
/*****************************************************************************/
/******************************** Documentation ******************************/
/*****************************************************************************/

/*****************************************************************************/
/*********************************** Defines *********************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Type Definitions ****************************/
/*****************************************************************************/

/*****************************************************************************/
/*************************  External Global Variables  ***********************/
/*****************************************************************************/

/*****************************************************************************/
/****************************  Exported Functions  ***************************/
/*****************************************************************************/

char *VisualClassName(
  long Key
);

char *EventMaskName(
  long Key
);

char *ByteOrderName(
  long Key
);

char *WindowFocusName(
  long Key
);

char *WindowFocusRevertName(
  long Key
);

/****************** stringConversion.c *********************************/
/*
 * 
 *           Copyright (c) Digital Equipment Corporation, 1993
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute  this software and its
 * documentation for any  purpose   and without fee  is  hereby  granted,
 * provided that the above copyright notice appear in all copies and that
 * both  that  copyright  notice  and  this  permission notice appear  in
 * supporting documentation, and that the name of Digital  not be used in
 * advertising or publicity  pertaining to distribution  of the  software
 * without specific, written prior permission.
 * 
 * DIGITAL DISCLAIMS   ALL  WARRANTIES WITH   REGARD   TO  THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT   SHALL  DIGITAL  BE   LIABLE  FOR  ANY SPECIAL,   INDIRECT   OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION  OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 * 
 */
/*
 * HISTORY
 * $Log: xglinfo.c,v $
 * Revision 2.2  1998/07/06 10:45:12  fuhrmann
 * Increased compiler warning level for all PDELIB-1.7 modules,
 * removed all bugs, some warnings remain (rcsids, ld_init, unused parameters)
 *
 * Revision 2.1  1996/10/02  11:11:05  fuhrmann
 * running
 *
 * Revision 2.0  1996/10/02  11:09:24  fuhrmann
 *  Zusammenkopie der Netzquellen
 *
 * Revision 1.1.2.3  1993/06/18  15:02:50  John_Dennis
 * 	change copyright token to free copyright token
 * 	[1993/06/18  14:59:28  John_Dennis]
 *
 * Revision 1.1.2.2  1993/06/16  20:33:32  John_Dennis
 * 	various lint clean ups
 * 	[1993/06/16  20:13:37  John_Dennis]
 * 
 * 	Initial Version by John Dennis
 * 	[1993/06/15  20:07:21  John_Dennis]
 * 
 * $EndLog$
 */
/*****************************************************************************/
/******************************** Documentation ******************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Include Files *******************************/
/*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>

/*****************************************************************************/
/****************************** Internal Defines *****************************/
/*****************************************************************************/

/*****************************************************************************/
/************************** Internal Type Definitions ************************/
/*****************************************************************************/

typedef struct {
  long Key;
  char *String;
} NumberedStrings;


/*****************************************************************************/
/**********************  External Function Declarations  *********************/
/*****************************************************************************/

/*****************************************************************************/
/**********************  Internal Function Declarations  *********************/
/*****************************************************************************/

/*****************************************************************************/
/*************************  External Global Variables  ***********************/
/*****************************************************************************/

/*****************************************************************************/
/*************************  Internal Global Variables  ***********************/
/*****************************************************************************/

/*****************************************************************************/
/***************************  Internal Functions  ****************************/
/*****************************************************************************/

static void
HeapSort(
long     n,
NumberedStrings x[]
)
{
  long i;			/* array index */
  long j;			/* array index */
  long k;			/* array index */
  NumberedStrings y;		/* temp element for swapping */

  /*
   * heap sort only works if n is greater than or equal to 3, so if n < 3
   * sort trivally
   */
  if (n == 1)
    return;
  if (n == 2) {
    if (x[0].Key > x[1].Key) {
      y = x[0];
      x[0] = x[1];
      x[1] = y;
    }
    return;
  }
  /* create initial heap */
  for (k = 1; k < n; k++) {
    /* insert x[k] into existing heap of size k-1 */
    i = k;
    y = x[k];		/* y is node to insert */
    j = (i - 1) / 2;		/* j is the father of i */
    while (i >= 1) {
      if (y.Key <= x[j].Key)
	break;
      x[i] = x[j];
      i = j;
      j = (i - 1) / 2;
    }
    x[i] = y;
  }

  /*
   * We remove x[0] and place it in its proper position in the array. We then
   * adjust the heap
   */
  for (k = n - 1; k > 0; k--) {
    y = x[k];
    x[k] = x[0];
    /*
     * Readjust the heap of order k-1. Move y down the heap for proper
     * position
     */
    i = 0;
    j = 1;
    if ((k - 1 >= 2) && (x[2].Key > x[1].Key))
      j = 2;
    /* j is the larger son of i in the heap of size k-1 */
    while (j <= k - 1) {
      if (x[j].Key <= y.Key)
	break;
      x[i] = x[j];
      i = j;
      j = (2 * i) + 1;
      if (j + 1 <= k - 1)
	if (x[j + 1].Key > x[j].Key)
	  j++;
    }
    x[i] = y;
  }
}


static char *BinarySearch (
  long Key,
  NumberedStrings *Table,
  long TableSize
)
{
  long Low = 0;
  long High = TableSize - 1;
  long Mid;
  static char ErrString[80];

  while (Low <= High) {
    Mid = (Low + High) / 2;
    if (Key == Table[Mid].Key) return (Table[Mid].String);
    if (Key < Table[Mid].Key)
      High = Mid - 1;
    else
      Low = Mid + 1;
  }
  sprintf(ErrString, "Key Unknown %ld", Key);
  return (ErrString);
}

/*****************************************************************************/
/****************************  Exported Functions  ***************************/
/*****************************************************************************/

char *VisualClassName(
  long Key
)
{
  static NumberedStrings Names[] = {
    {StaticGray,	"StaticGray"},
    {GrayScale,		"GrayScale"},
    {StaticColor,	"StaticColor"},
    {PseudoColor,	"PseudoColor"},
    {TrueColor,		"TrueColor"},
    {DirectColor,	"DirectColor"},
  };
  static int Sorted = 0;	/* not sorted yet */
  static const long N = sizeof(Names) / sizeof(NumberedStrings);

  if (!Sorted) {
    Sorted = 1;
    HeapSort(N, Names);
  }
  return(BinarySearch(Key, Names, N));
}

char *EventMaskName(
  long Key
)
{
  static NumberedStrings Names[] = {
    {NoEventMask,		"NoEvent"},
    {KeyPressMask,		"KeyPress"},
    {KeyReleaseMask,		"KeyRelease"},
    {ButtonPressMask,		"ButtonPress"},
    {ButtonReleaseMask,		"ButtonRelease"},
    {EnterWindowMask,		"EnterWindow"},
    {LeaveWindowMask,		"LeaveWindow"},
    {PointerMotionMask,		"PointerMotion"},
    {PointerMotionHintMask,	"PointerMotionHint"},
    {Button1MotionMask,		"Button1Motion"},
    {Button2MotionMask,		"Button2Motion"},
    {Button3MotionMask,		"Button3Motion"},
    {Button4MotionMask,		"Button4Motion"},
    {Button5MotionMask,		"Button5Motion"},
    {ButtonMotionMask,		"ButtonMotion"},
    {KeymapStateMask,		"KeymapState"},
    {ExposureMask,		"Exposure"},
    {VisibilityChangeMask,	"VisibilityChange"},
    {StructureNotifyMask,	"StructureNotify"},
    {ResizeRedirectMask,	"ResizeRedirect"},
    {SubstructureNotifyMask,	"SubstructureNotify"},
    {SubstructureRedirectMask,	"SubstructureRedirect"},
    {FocusChangeMask,		"FocusChange"},
    {PropertyChangeMask,	"PropertyChange"},
    {ColormapChangeMask,	"ColormapChange"},
    {OwnerGrabButtonMask,	"OwnerGrabButton"},
  };
  static int Sorted = 0;	/* not sorted yet */
  static const long N = sizeof(Names) / sizeof(NumberedStrings);

  if (!Sorted) {
    Sorted = 1;
    HeapSort(N, Names);
  }
  return(BinarySearch(Key, Names, N));
}

char *ByteOrderName(
  long Key
)
{
  static NumberedStrings Names[] = {
    {LSBFirst,	"LSBFirst"},
    {MSBFirst,	"MSBFirst"},
  };
  static int Sorted = 0;	/* not sorted yet */
  static const long N = sizeof(Names) / sizeof(NumberedStrings);

  if (!Sorted) {
    Sorted = 1;
    HeapSort(N, Names);
  }
  return(BinarySearch(Key, Names, N));
}

char *WindowFocusName(
  long Key
)
{
  static NumberedStrings Names[] = {
    {None,		"None"},
    {PointerRoot,	"PointerRoot"},
  };
  static int Sorted = 0;	/* not sorted yet */
  static const long N = sizeof(Names) / sizeof(NumberedStrings);

  if (!Sorted) {
    Sorted = 1;
    HeapSort(N, Names);
  }
  return(BinarySearch(Key, Names, N));
}

char *WindowFocusRevertName(
  long Key
)
{
  static NumberedStrings Names[] = {
    {RevertToNone,		"RevertToNone"},
    {RevertToPointerRoot,	"RevertToPointerRoot"},
    {RevertToParent,		"RevertToParent"},
  };
  static int Sorted = 0;	/* not sorted yet */
  static const long N = sizeof(Names) / sizeof(NumberedStrings);

  if (!Sorted) {
    Sorted = 1;
    HeapSort(N, Names);
  }
  return(BinarySearch(Key, Names, N));
}
/****************** original xglinfo.c *********************************/

/*
 * 
 *           Copyright (c) Digital Equipment Corporation, 1993
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute  this software and its
 * documentation for any  purpose   and without fee  is  hereby  granted,
 * provided that the above copyright notice appear in all copies and that
 * both  that  copyright  notice  and  this  permission notice appear  in
 * supporting documentation, and that the name of Digital  not be used in
 * advertising or publicity  pertaining to distribution  of the  software
 * without specific, written prior permission.
 * 
 * DIGITAL DISCLAIMS   ALL  WARRANTIES WITH   REGARD   TO  THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT   SHALL  DIGITAL  BE   LIABLE  FOR  ANY SPECIAL,   INDIRECT   OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION  OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 * 
 */
/*
 * HISTORY
 * $Log: xglinfo.c,v $
 * Revision 2.2  1998/07/06 10:45:12  fuhrmann
 * Increased compiler warning level for all PDELIB-1.7 modules,
 * removed all bugs, some warnings remain (rcsids, ld_init, unused parameters)
 *
 * Revision 2.1  1996/10/02  11:11:05  fuhrmann
 * running
 *
 * Revision 2.0  1996/10/02  11:09:24  fuhrmann
 *  Zusammenkopie der Netzquellen
 *
 * Revision 1.1.2.3  1993/06/18  15:02:52  John_Dennis
 * 	change copyright token to free copyright token
 * 	[1993/06/18  14:59:33  John_Dennis]
 *
 * Revision 1.1.2.2  1993/06/16  20:33:39  John_Dennis
 * 	various lint clean ups, nicer formatting, etc.
 * 	[1993/06/16  20:13:42  John_Dennis]
 * 
 * 	Initial Version by John Dennis
 * 	[1993/06/15  20:07:25  John_Dennis]
 * 
 * $EndLog$
 */
/*****************************************************************************/
/******************************** Documentation ******************************/
/*****************************************************************************/

/*
 * See man page for full documentation.
 *
 * Written by John Dennis, Digital Equipment Corporation.
 * jdennis@mlo.dec.com
 *
 * With thanks and credit to Jim Fulton who wrote the original version of
 * xdpyinfo while at MIT which served as a starting point for this code.
 */

/*****************************************************************************/
/******************************* Include Files *******************************/
/*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>

/*****************************************************************************/
/****************************** Internal Defines *****************************/
/*****************************************************************************/

#define GL_GET_CONFIG(_config, _pValue)                                      \
{                                                                            \
  switch (result = glXGetConfig(dpy, visual, _config, (_pValue))) {          \
  case Success:                                                              \
    break;                                                                   \
  case GLX_NO_EXTENSION:                                                     \
    printf("ERROR glXGetConfig %s return GLX_NO_EXTENSION, visuaID 0x%lx\n", \
          "_config", visual->visualid);                                      \
    break;                                                                   \
  case GLX_BAD_SCREEN:                                                       \
    printf("ERROR glXGetConfig %s return GLX_BAD_SCREEN, visuaID 0x%lx\n",   \
          "_config", visual->visualid);                                      \
    break;                                                                   \
  case GLX_BAD_ATTRIBUTE:                                                    \
    printf("ERROR glXGetConfig %s return GLX_BAD_ATTRIBUTE, visuaID 0x%lx\n",\
          "_config", visual->visualid);                                      \
    break;                                                                   \
  case GLX_BAD_VISUAL:                                                       \
    printf("ERROR glXGetConfig %s return GLX_BAD_VISUAL, visuaID 0x%lx\n",   \
          "_config", visual->visualid);                                      \
    break;                                                                   \
  default:                                                                   \
    printf("ERROR glXGetConfig %s return unknown error %d, visuaID 0x%lx\n", \
          "_config", result, visual->visualid);                              \
    break;                                                                   \
  }                                                                          \
}

/*****************************************************************************/
/************************** Internal Type Definitions ************************/
/*****************************************************************************/

typedef struct GLXVisualInfo {
  int useGL;			       /* support GLX rendering */
  int bufferSize;		       /* depth of the color buffer */
  int level;			       /* level in plane stacking */
  int rgba;			       /* true if RGBA mode */
  int doubleBuffer;		       /* double buffering supported */
  int stereo;			       /* stereo buffering supported */
  int auxBuffers;		       /* number of aux buffers */
  int redSize;			       /* number of red component bits */
  int greenSize;		       /* number of green component bits */
  int blueSize;			       /* number of blue component bits */
  int alphaSize;		       /* number of alpha component bits */
  int depthSize;		       /* number of depth bits */
  int stencilSize;		       /* number of stencil bits */
  int accumRedSize;		       /* number of red accum bits */
  int accumGreenSize;		       /* number of green accum bits */
  int accumBlueSize;		       /* number of blue accum bits */
  int accumAlphaSize;		       /* number of alpha accum bits */
} GLXVisualInfo;

/*****************************************************************************/
/**********************  External Function Declarations  *********************/
/*****************************************************************************/

/*****************************************************************************/
/**********************  Internal Function Declarations  *********************/
/*****************************************************************************/

static void
Usage(
  void
);

static void
ProcessArgs(
	     int argc   ,
	     char       *argv[]
);

void
PrintEventMask (
    int indent,				/* amount by which to indent */
    unsigned long mask				/* event mask */
);

void
PrintExtensionInfo(
  Display    *dpy,
  int         fieldNameLen,
  int	      indent
);

void
PrintPixmapFormats(
  Display    *dpy,
  int         fieldNameLen,
  int	      indent
);

void
PrintWindowFocus(
  Display    *dpy,
  int         fieldNameLen
);

void
PrintDisplayInfo(
  Display    *dpy,
  int         fieldNameLen
);

int
GetGLVisualInfo (
    Display    *dpy,
    XVisualInfo *visual,
    GLXVisualInfo *glVisual
);

int
PrintCookedVisualInfo (
    Display    *dpy,
    XVisualInfo *visual,
    int indent
);

int
PrintRawVisualInfo (
    Display    *dpy,
    XVisualInfo *visual,
    int indent
);

void
PrintScreenInfo(
  Display    *dpy,
  int         scr,
  int         fieldNameLen,
  int	      indent
);

void
PrintScreenVisualInfo(
  Display    *dpy,
  int         scr,
  int         fieldNameLen,
  int	      indent
);

/*****************************************************************************/
/*************************  External Global Variables  ***********************/
/*****************************************************************************/

char       *ProgramName;

/*****************************************************************************/
/*************************  Internal Global Variables  ***********************/
/*****************************************************************************/

static char       *displayname = NULL;	/* server to contact */
static int         printDisplayInfo = GL_TRUE;
static int         printScreenInfo = GL_TRUE;
static int         printVisualInfo = GL_TRUE;
static int         printCookedVisualInfo = GL_TRUE;
static int         printRawVisualInfo = GL_FALSE;

static int	    pageWidth = 80;
static GLint       glErrorBase, glEventBase;
static GLint       glMajorVersion, glMinorVersion;
static Bool	    dpyHasGL = GL_FALSE;

/*****************************************************************************/
/***************************  Internal Functions  ****************************/
/*****************************************************************************/

static void
Usage(
  void
)
{
  fprintf(stderr, "usage:  %s [-display displayname]\n\n",
	  ProgramName);
  fprintf(stderr, "  the following options take a '+' or '-' switch\n");
  fprintf(stderr, "  a '+' turns the option on, a '-' turns the option off\n\n");
  fprintf(stderr, "  [+-printDisplayInfo] [+-printScreenInfo] [+-printVisualInfo]\n");
  fprintf(stderr, "  [+-printCookedVisualInfo] [+-printRawVisualInfo]\n");
  exit(1);
}

static void
ProcessArgs(
	     int argc   ,
	     char       *argv[]
)
{
  int         i;
    char       *arg;
  char        switchChar;

  ProgramName = argv[0];

  for (i = 1; i < argc; i++) {
    arg = argv[i];
    switchChar = arg[0];

    if (switchChar == '-' || switchChar == '+') {
      arg++;
      if (strcmp(arg, "display") == 0) {
	if (switchChar == '-') {
	  if (++i >= argc) goto missing;
	  displayname = argv[i];
	}
	else goto unknown;
      }
      else if (strcmp(arg, "printDisplayInfo") == 0) {
	if (switchChar == '+')
	  printDisplayInfo = GL_TRUE;
	else
	  printDisplayInfo = GL_FALSE;
      }
      else if (strcmp(arg, "printScreenInfo") == 0) {
	if (switchChar == '+')
	  printScreenInfo = GL_TRUE;
	else
	  printScreenInfo = GL_FALSE;
      }
      else if (strcmp(arg, "printVisualInfo") == 0) {
	if (switchChar == '+')
	  printVisualInfo = GL_TRUE;
	else
	  printVisualInfo = GL_FALSE;
      }
      else if (strcmp(arg, "printRawVisualInfo") == 0) {
	if (switchChar == '+')
	  printRawVisualInfo = GL_TRUE;
	else
	  printRawVisualInfo = GL_FALSE;
      }
      else if (strcmp(arg, "printCookedVisualInfo") == 0) {
	if (switchChar == '+')
	  printCookedVisualInfo = GL_TRUE;
	else
	  printCookedVisualInfo = GL_FALSE;
      }
      else goto unknown;
    }
    else {
      goto unknown;
    }
  }
  return;

  unknown:
      fprintf(stderr, "unknown arg: %s\n", argv[i]);
      Usage();
    
  missing:
      fprintf(stderr, "missing arg for: %s\n", argv[i]);
      Usage();
}

void
PrintEventMask (
    int indent,				/* amount by which to indent */
    unsigned long mask				/* event mask */
)
{
  char buf[1024];
    int i, pad;
    int nameWidth = 21;
    unsigned long bit;
    int len = indent;
    int nameLen;
    int bitsfound = 0;
    char *name;

    for (i = 0; i < indent; i++) buf[i] = ' ';

    for (bit = 1; mask; bit <<= 1) {
      if (bit & mask) {
	mask ^= bit;		       /* clear bit from mask */
	name = EventMaskName((long)bit);
	nameLen = strlen(name);
	if (nameLen >= nameWidth) nameLen = nameWidth - 1; /* -1 for pad */
	pad = nameWidth - nameLen;
	if (len + nameLen > pageWidth) {
	  puts (buf);
	  len = indent;
	}
	strncpy(buf+len, name, nameLen);
	len += nameLen;
	if (len + pad > pageWidth) {
	  buf[len] = 0;
	  puts (buf);
	  len = indent;
	}
	else {
	  for (i = 0; i < pad; i++) buf[len++] = ' ';
	}
	buf[len] = 0;
	bitsfound++;
      }
    }

    if (bitsfound) puts (buf);
}

void
PrintExtensionInfo(
  Display    *dpy,
  int         fieldNameLen,
  int	      indent
)
{
  int         n = 0;
  int         len, lineLen;
  char      **extlist = XListExtensions(dpy, &n);


  printf("%*s%d total\n",
	 fieldNameLen, "Server Extensions:",
	 n);
  if (extlist) {
    register int i;

    printf("%*s", indent, "");
    lineLen = indent;
    for (i = 0; i < n; i++) {
      len = strlen(extlist[i]);
      len += 2;			       /* for ", " */
      if ((lineLen + len) >= pageWidth) {
	printf("\n%*s", indent, "");
	lineLen = indent;
      }
      printf("%s, ", extlist[i]);
      lineLen += len;
    }
    if (lineLen > indent) printf("\n");
    XFreeExtensionList(extlist);
  }
}

void
PrintPixmapFormats(
  Display    *dpy,
  int         fieldNameLen,
  int	      indent
)
{
  int i, n;
  XPixmapFormatValues *pmf;

  pmf = XListPixmapFormats(dpy, &n);
  printf("%*s%d total\n",
	 fieldNameLen, "pixmap formats:",
	 n);
  if (pmf) {
    for (i = 0; i < n; i++) {
      printf("%*sdepth %2d, bits_per_pixel %2d, scanline_pad %2d\n",
	     indent, "",
	     pmf[i].depth, pmf[i].bits_per_pixel, pmf[i].scanline_pad);
    }
    XFree((char *) pmf);
  }
}

void
PrintWindowFocus(
  Display    *dpy,
  int         fieldNameLen
)
{
  Window      focusWin;
  int         focusRevert;

  XGetInputFocus(dpy, &focusWin, &focusRevert);
  switch (focusWin) {
  case PointerRoot:
  case None:
    printf("%*s%s\n",
	   fieldNameLen, "focus:",
	   WindowFocusName(focusWin));
    break;
  default:
    printf("%*s0x%lx, revert to %s\n",
	   fieldNameLen, "focus window:",
	   focusWin, WindowFocusRevertName(focusRevert));
    break;
  }
}

void
PrintDisplayInfo(
  Display    *dpy,
  int         fieldNameLen
)
{
  char       *cp;
  int         minkeycode, maxkeycode;
  char        title[256];
  int         i;
  int         titleLen, fillLen;

  sprintf(title, " Display %s ", DisplayString(dpy));
  titleLen = strlen(title);
  fillLen = (pageWidth - titleLen) / 2;
  for (i = 0; i < fillLen; i++) printf("%c", '=');
  printf("%s", title);
  for (i = 0; i < fillLen; i++) printf("%c", '=');
  printf("\n");  

  printf("%*s%s\n",
	 fieldNameLen, "name of display:",
	 DisplayString(dpy));
  printf("%*s%d.%d\n",
	 fieldNameLen, "version number:",
	 ProtocolVersion(dpy), ProtocolRevision(dpy));
  printf("%*s%s\n",
	 fieldNameLen, "vendor string:",
	 ServerVendor(dpy));
  printf("%*s%d\n",
	 fieldNameLen, "vendor release:",
	 VendorRelease(dpy));
  printf("%*s%ld bytes\n",
	 fieldNameLen, "max request size:",
	 XMaxRequestSize(dpy) * 4);
  printf("%*s%d\n",
	 fieldNameLen, "motion buffer size:",
	 XDisplayMotionBufferSize(dpy));

  cp = ByteOrderName(BitmapBitOrder(dpy));
  printf("%*sunit = %d, bit order = %s, padding = %d\n",
	 fieldNameLen, "bitmap:",
	 BitmapUnit(dpy), cp, BitmapPad(dpy));

  cp = ByteOrderName(ImageByteOrder(dpy));
  printf("%*s%s\n",
	 fieldNameLen, "image byte order:",
	 cp);

  XDisplayKeycodes(dpy, &minkeycode, &maxkeycode);
  printf("%*sminimum %d, maximum %d\n",
	 fieldNameLen, "keycode range:",
	 minkeycode, maxkeycode);

  PrintWindowFocus(dpy, fieldNameLen);

  printf("%*s%d\n",
	 fieldNameLen, "default screen num:",
	 DefaultScreen(dpy));
  printf("%*s%d\n",
	 fieldNameLen, "number of screens:",
	 ScreenCount(dpy));


  PrintPixmapFormats(dpy, fieldNameLen,
		     fieldNameLen > 0 ? fieldNameLen : -fieldNameLen);
  PrintExtensionInfo(dpy, fieldNameLen,
		     fieldNameLen > 0 ? fieldNameLen : -fieldNameLen);

  printf("%*s",
	 fieldNameLen, "GLX Extension:");
  if (dpyHasGL) {
    printf("error base = %d, event base = %d, ",
	   glErrorBase, glEventBase);
    if (glXQueryVersion(dpy, &glMajorVersion, &glMinorVersion)) {
      printf("Version %d.%d\n",
	     glMajorVersion, glMinorVersion);
    }
    else {
      printf("glXQueryVersion returned error\n");
      exit(1);
    }
  }
  else {
    printf("NONE\n");
  }
}

int
GetGLVisualInfo (
    Display    *dpy,
    XVisualInfo *visual,
    GLXVisualInfo *glVisual
)
{
  int result;

  if (!dpyHasGL) {
    glVisual->useGL = GL_FALSE;
    return(0);
  }

  GL_GET_CONFIG(GLX_USE_GL, &glVisual->useGL);

  if (!glVisual->useGL) return(0);

  GL_GET_CONFIG(GLX_BUFFER_SIZE, &glVisual->bufferSize);
  GL_GET_CONFIG(GLX_LEVEL, &glVisual->level);
  GL_GET_CONFIG(GLX_RGBA, &glVisual->rgba);
  GL_GET_CONFIG(GLX_DOUBLEBUFFER, &glVisual->doubleBuffer);
  GL_GET_CONFIG(GLX_STEREO, &glVisual->stereo);
  GL_GET_CONFIG(GLX_AUX_BUFFERS, &glVisual->auxBuffers);
  GL_GET_CONFIG(GLX_RED_SIZE, &glVisual->redSize);
  GL_GET_CONFIG(GLX_GREEN_SIZE, &glVisual->greenSize);
  GL_GET_CONFIG(GLX_BLUE_SIZE, &glVisual->blueSize);
  GL_GET_CONFIG(GLX_ALPHA_SIZE, &glVisual->alphaSize);
  GL_GET_CONFIG(GLX_DEPTH_SIZE, &glVisual->depthSize);
  GL_GET_CONFIG(GLX_STENCIL_SIZE, &glVisual->stencilSize);
  GL_GET_CONFIG(GLX_ACCUM_RED_SIZE, &glVisual->accumRedSize);
  GL_GET_CONFIG(GLX_ACCUM_GREEN_SIZE, &glVisual->accumGreenSize);
  GL_GET_CONFIG(GLX_ACCUM_BLUE_SIZE, &glVisual->accumBlueSize);
  GL_GET_CONFIG(GLX_ACCUM_ALPHA_SIZE, &glVisual->accumAlphaSize);

  return(1);
}

int
PrintCookedVisualInfo (
    Display    *dpy,
    XVisualInfo *visual,
    int indent
)
{
  int result;
  int hasRGB, hasAlpha, hasDepth, hasStencil, hasAccum, hasAuxBuffers;
  GLXVisualInfo glVisualInfo;
  GLXVisualInfo *gl = &glVisualInfo;
  int listEmpty;

  printf ("%s visual: ID = 0x%lx (hex) %ld (decimal) screen = %d\n",
	  VisualClassName(visual->class) , visual->visualid, visual->visualid,
	  visual->screen);

  if ((result = GetGLVisualInfo(dpy, visual, gl))) {

    hasRGB = ((gl->redSize != 0)   ||
	      (gl->greenSize != 0) ||
	      (gl->blueSize != 0));
    hasAlpha = (gl->alphaSize != 0);
    hasDepth = (gl->depthSize != 0);
    hasStencil = (gl->stencilSize != 0);
    hasAccum = ((gl->accumRedSize != 0)   ||
		(gl->accumGreenSize != 0) ||
		(gl->accumBlueSize != 0)  ||
		(gl->accumAlphaSize != 0));
    hasAuxBuffers = (gl->auxBuffers != 0);


    printf("%*s", indent, "");
    if (gl->level < 0) {
      printf("UNDERLAY(%d) ", -gl->level);
    }
    if (gl->level > 0) {
      printf("OVERLAY(%d) ", gl->level);
    }
    if (gl->doubleBuffer)
      printf("DOUBLE buffered ");
    else
      printf("SINGLE buffered ");

    if (gl->stereo)
      printf("STEREO ");
    else
      printf("MONO ");

    /*
     * RGBA visual
     */
    if (gl->rgba) {		       /* rgba visual */
      printf("RGB visual ");

      listEmpty = GL_TRUE;
      if (hasAlpha || hasDepth || hasStencil || hasAccum || hasAuxBuffers) {
	printf("with (");
	if (hasAlpha) {
	  if (listEmpty) listEmpty = GL_FALSE; else printf(" ");
	  printf("Alpha");
	}
	if (hasDepth) {
	  if (listEmpty) listEmpty = GL_FALSE; else printf(" ");
	  printf("Z");
	}
	if (hasStencil) {
	  if (listEmpty) listEmpty = GL_FALSE; else printf(" ");
	  printf("Stencil");
	}
	if (hasAccum) {
	  if (listEmpty) listEmpty = GL_FALSE; else printf(" ");
	  printf("Accum");
	}
	if (hasAuxBuffers) {
	  if (listEmpty) listEmpty = GL_FALSE; else printf(" ");
	  printf("%d Aux Buffers",gl->auxBuffers);
	}
	printf(")\n");
      }

      printf("%*s", indent, "");
      printf("GL Sizes: RGBA=(%d,%d,%d,%d), ",
	     gl->redSize, gl->greenSize, gl->blueSize, gl->alphaSize);
      if (hasDepth)
	printf("Z=%d, ", gl->depthSize);
      if (hasStencil)
	printf("Stencil=%d, ", gl->stencilSize);
      if (hasAccum)
	printf("Accum=(%d,%d,%d,%d)",
	       gl->accumRedSize, gl->accumGreenSize, gl->accumBlueSize,
	       gl->accumAlphaSize);
      printf("\n");

      /*
       * Do some simple sanity checks on rgba visual.
       */
      if (!hasRGB)
	printf("%*sERROR: RGBA visual, but RGB have zero size\n",
	       indent, "");
      if (hasAlpha && hasAccum && gl->accumAlphaSize == 0)
	printf("%*sERROR: RGBA visual with Alpha and Accum, but Accum Alpha size is zero\n",
	       indent, "");
    }				       /* end rgba visual */
    /*
     * COLOR INDEX visual
     */
    else {			       /* color index visual */
      printf("COLOR INDEX visual ");

      listEmpty = GL_TRUE;
      if (hasDepth || hasStencil || hasAuxBuffers) {
	printf("with (");
	if (hasDepth) {
	  if (listEmpty) listEmpty = GL_FALSE; else printf(" ");
	  printf("Z");
	}
	if (hasStencil) {
	  if (listEmpty) listEmpty = GL_FALSE; else printf(" ");
	  printf("Stencil");
	}
	if (hasAuxBuffers) {
	  if (listEmpty) listEmpty = GL_FALSE; else printf(" ");
	  printf("%d Aux Buffers",gl->auxBuffers);
	}
	printf(")\n");
      }

      printf("%*s", indent, "");
      printf("GL Sizes: ColorIndex=%d, ", gl->bufferSize);
      if (hasDepth)
	printf("Z=%d, ", gl->depthSize);
      if (hasStencil)
	printf("Stencil=%d", gl->stencilSize);
      printf("\n");

      /*
       * Do some simple sanity checks on color index visual.
       */
      if (hasRGB)
	printf("%*sERROR: CI visual, but RGB has non-zero sizes (%d,%d,%d)\n",
	       indent, "", gl->redSize, gl->greenSize, gl->blueSize);
      if (hasAlpha)
	printf("%*sERROR: CI visual, but Alpha has non-zero size = %d\n",
	       indent, "", gl->alphaSize);
      if (hasAccum)
	printf("%*sERROR: CI visual, but Accum has non-zero sizes (%d,%d,%d,%d)\n",
	       indent, "", gl->accumRedSize, gl->accumGreenSize,
	       gl->accumBlueSize, gl->accumAlphaSize);
    }				       /* end color index visual */
  }
  else {
    printf("%*s", indent, "");
    printf("GL NOT SUPPORTED\n");
  }

  printf("%*sCore X: ", indent, "");
  printf ("depth=%d, ", visual->depth);
  printf ("colormapSize=%d ", visual->colormap_size);

  if ((visual->class == TrueColor) || (visual->class == DirectColor)) {
    printf ("RGB: masks=(0x%lx,0x%lx,0x%lx) ",
	    visual->red_mask, visual->green_mask, visual->blue_mask);
    printf ("bits=%d",
	    visual->bits_per_rgb);
  }
  printf("\n");
  return 0;
}

int
PrintRawVisualInfo (
    Display    *dpy,
    XVisualInfo *visual,
    int indent
)
{
  int result;
  GLXVisualInfo glVisualInfo;
  GLXVisualInfo *gl = &glVisualInfo;

  printf ("%s visual: ID = 0x%lx (hex) %ld (decimal) screen = %d\n",
	  VisualClassName(visual->class) , visual->visualid, visual->visualid,
	  visual->screen);
  
  if ((result = GetGLVisualInfo(dpy, visual, gl))) {

    printf("%*s", indent, "");
    printf("useGL=%d, level=%d, doubleBuffer=%d, rgba=%d, stereo=%d\n",
	   gl->useGL, gl->level, gl->doubleBuffer, gl->rgba, gl->stereo);

    printf("%*s", indent, "");
    printf("bufferSize=%d, number aux buffers=%d\n",
	   gl->bufferSize, gl->auxBuffers);

    printf("%*s", indent, "");
    printf("sizes: RGBA=(%d,%d,%d,%d), Z=%d, STENCIL=%d, ACCUM=(%d,%d,%d,%d)\n",
	   gl->redSize, gl->greenSize, gl->blueSize, gl->alphaSize,
	   gl->depthSize, gl->stencilSize, 
	   gl->accumRedSize, gl->accumGreenSize,
	   gl->accumBlueSize, gl->accumAlphaSize);

  }
  else {
    printf("%*s", indent, "");
    printf("GL NOT SUPPORTED\n");
  }

  printf("%*sCore X: ", indent, "");
  printf ("depth=%d, ", visual->depth);
  printf ("colormapSize=%d ", visual->colormap_size);
  printf ("RGB: masks=(0x%lx,0x%lx,0x%lx) ",
	  visual->red_mask, visual->green_mask, visual->blue_mask);
  printf ("bits=%d",
	  visual->bits_per_rgb);
  printf("\n");
  return 0;
}

void
PrintScreenInfo(
  Display    *dpy,
  int         scr,
  int         fieldNameLen,
  int	      indent
)
{
  Screen     *s = ScreenOfDisplay(dpy, scr);	/* opaque structure */
  int         i;		/* temp variable: iterator */
  static char *yes = "YES", *no = "NO", *when = "WHEN MAPPED";
  double      xres, yres;
  int         ndepths = 0, *depths = NULL;
  char        title[256];
  int         titleLen, fillLen;

  sprintf(title, " Screen %d ", scr);
  titleLen = strlen(title);
  fillLen = (pageWidth - titleLen) / 2;
  for (i = 0; i < fillLen; i++) printf("%c", '=');
  printf("%s", title);
  for (i = 0; i < fillLen; i++) printf("%c", '=');
  printf("\n");  

  /*
   * there are 2.54 centimeters to an inch; so there are 25.4 millimeters.
   * 
   * dpi = N pixels / (M millimeters / (25.4 millimeters / 1 inch)) = N pixels /
   * (M inch / 25.4) = N * 25.4 pixels / M inch
   */

  xres = ((((double) DisplayWidth(dpy, scr)) * 25.4) /
	  ((double) DisplayWidthMM(dpy, scr)));
  yres = ((((double) DisplayHeight(dpy, scr)) * 25.4) /
	  ((double) DisplayHeightMM(dpy, scr)));

  printf("%*s%d\n",
	 fieldNameLen, "screen:",
	 scr);
  printf("%*s%dx%d pixels (%dx%d millimeters)\n",
         fieldNameLen, "dimensions:",
	 DisplayWidth(dpy, scr), DisplayHeight(dpy, scr),
	 DisplayWidthMM(dpy, scr), DisplayHeightMM(dpy, scr));

  printf("%*s%dx%d dots per inch\n",
	 fieldNameLen, "resolution:",
	 (int) (xres + 0.5), (int) (yres + 0.5));
  depths = XListDepths(dpy, scr, &ndepths);
  if (!depths)
    ndepths = 0;
  printf("%*s(%d total):       ",
	 fieldNameLen, "depths:", ndepths);
  for (i = 0; i < ndepths; i++) {
    printf("%d", depths[i]);
    if (i < ndepths - 1) {
      putchar(',');
      putchar(' ');
    }
  }
  putchar('\n');
  if (depths)
    XFree((char *) depths);
  printf("%*s0x%lx\n",
	 fieldNameLen, "root window id:", RootWindow(dpy, scr));
  printf("%*s%d plane%s\n",
	 fieldNameLen, "depth of root window:",
	 DisplayPlanes(dpy, scr),
	 DisplayPlanes(dpy, scr) == 1 ? "" : "s");
  printf("%*sminimum %d, maximum %d\n",
	 fieldNameLen, "number colormaps:",
	 MinCmapsOfScreen(s), MaxCmapsOfScreen(s));
  printf("%*s0x%lx, number colormap cells %d\n",
	 fieldNameLen, "default colormap:",
	 DefaultColormap(dpy, scr), DisplayCells(dpy, scr));
  printf("%*sblack 0x%lx, white 0x%lx\n",
	 fieldNameLen, "preallocated pixels:",  
	 BlackPixel(dpy, scr), WhitePixel(dpy, scr));
  printf("%*sbacking-store %s, save-unders %s\n",
	 fieldNameLen, "options:",
	 (DoesBackingStore(s) == NotUseful) ? no :
	 ((DoesBackingStore(s) == Always) ? yes : when),
	 DoesSaveUnders(s) ? yes : no);
  printf("%*s0x%lx\n",
	 fieldNameLen, "input event mask:", EventMaskOfScreen(s));
  PrintEventMask(fieldNameLen > 0 ? fieldNameLen : -fieldNameLen,
		 EventMaskOfScreen(s));


  return;
}

void
PrintScreenVisualInfo(
  Display    *dpy,
  int         scr,
  int         fieldNameLen,
  int	      indent
)
{
  XVisualInfo visualTemplate;		/* fill in for getting info */
  XVisualInfo *visuals;		/* retured info */
  int         nVisuals;		/* number of elements returned */
  int         i;
  char        title[256];
  int         titleLen, fillLen;

  nVisuals = 0;
  visualTemplate.screen = scr;
  visuals = XGetVisualInfo(dpy, VisualScreenMask, &visualTemplate, &nVisuals);

  sprintf(title, " %d Visuals for Screen %d (default = 0x%lx) ",
	  nVisuals, scr, XVisualIDFromVisual(DefaultVisual(dpy, scr)));
  titleLen = strlen(title);
  fillLen = (pageWidth - titleLen) / 2;
  for (i = 0; i < fillLen; i++) printf("%c", '=');
  printf("%s", title);
  for (i = 0; i < fillLen; i++) printf("%c", '=');
  printf("\n");  

  for (i = 0; i < nVisuals; i++) {
    if (printCookedVisualInfo)
      PrintCookedVisualInfo (dpy, &visuals[i], indent);
    if (printRawVisualInfo)
      PrintRawVisualInfo (dpy, &visuals[i], indent);
    printf("\n");
  }
  if (visuals)
    XFree((char *) visuals);

}

/*****************************************************************************/
/****************************  Exported Functions  ***************************/
/*****************************************************************************/

int
main(
      int argc,
      char *argv[]
)
{
  Display    *dpy;		/* X connection */
  int         screenNum;
  int         fieldNameLen = -24;
  int         indent = 2;

  ProcessArgs(argc, argv);

  dpy = XOpenDisplay(displayname);
  if (!dpy) {
    fprintf(stderr, "%s:  unable to open display \"%s\".\n",
	    ProgramName, XDisplayName(displayname));
    exit(1);
  }

  dpyHasGL = glXQueryExtension(dpy, &glErrorBase, &glEventBase);

  if (printDisplayInfo) PrintDisplayInfo(dpy, fieldNameLen);

  for (screenNum = 0; screenNum < ScreenCount(dpy); screenNum++) {
    if (printScreenInfo)
      PrintScreenInfo(dpy, screenNum, fieldNameLen, indent);
    if (printRawVisualInfo || printCookedVisualInfo)
      PrintScreenVisualInfo(dpy, screenNum, fieldNameLen, indent);
  }

  return(XCloseDisplay(dpy));
}
