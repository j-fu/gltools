static char *rcsid= "$Id: xglquery.c,v 1.1 1996/10/16 15:51:07 darcy Exp $";
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>


static char       *displayname = NULL;	/* server to contact */
static char       *ProgramName = NULL;	
static GLint       glErrorBase, glEventBase;
static Bool	    dpyHasGL = GL_FALSE;

/*****************************************************************************/
/***************************  Internal Functions  ****************************/
/*****************************************************************************/

static void
Usage(
  void
)
{
  fprintf(stderr, "usage:  xglquery [-display displayname]\n\n");
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
  static GLint       glErrorBase, glEventBase;

  ProcessArgs(argc, argv);

  dpy = XOpenDisplay(displayname);
  if (!dpy) {
    fprintf(stderr, "%s:  unable to open display \"%s\".\n",
	    ProgramName, XDisplayName(displayname));
    exit(1);
  }

  dpyHasGL = glXQueryExtension(dpy, &glErrorBase, &glEventBase);

  XCloseDisplay(dpy);
  if (dpyHasGL) return 0; else return 1;
}
/*
 * $Log: xglquery.c,v $
 * Revision 1.1  1996/10/16 15:51:07  darcy
 * forget revision numbers less than 1.1
 *
 * Revision 0.1  1996/10/09  17:22:22  fuhrmann
 * initial revision
 *	
 */
