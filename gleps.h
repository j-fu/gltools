/*
% $Id: gleps.h,v 2.6 2000/11/22 15:20:49 fuhrmann Exp $

\section{gleps  - Encapsulated Postscript Dump}
$Revision: 2.6 $\\
$Date: 2000/11/22 15:20:49 $\\
Author: J\"urgen Fuhrmann\\
\bigskip
%--------------------------------------------------------------------

This module provides the possibility to dump rendered graphics into
vector postscript files using the feedback buffer rendering 
mechanism of OpenGL. It is in a beta state and possibly will remain
there because it is not that easy to map all the OpenGL functionality
to Postscript. It should reasonably well render graphics which remains
in the limits of the OpenGL features used by the other gltools parts.


This code would not exist if Mark Kilgard wouldn't have placed his
rendereps sample code onto the net, and if there would not exist 
Frederic Delhoume's free gouraudtriangle postscript code.
From Mark's code, handling of lines and polygons has been taken.
String handling is new.
*/




/*\func{glepsDumpUnSorted}*/
void glepsDumpUnSorted(glWindow w, FILE *file,int crop);

/*

  Create vector postscript dump using the feedback buffer
   mechanism, but without hidden surface removal.
  The crop flag is used to decide whether to crop ps output to
  the actually drawn area or not.
   */


/*\func{glepsDumpSorted}*/
void glepsDumpSorted(glWindow w, FILE *file, int crop);
/*
  Create vector postscript dump using the feedback buffer
  mechanism, with hidden surface removal based on sorting
  the feedback buffer before the dump.
  The crop flag is used to decide whether to crop ps output to
  the actually drawn area or not.

  */ 


/*\func{glepsSetOutputFormat}*/
void glepsSetOutputFormat(char * coord_fmt, char *color_fmt);
/*
  Set the output format for floating point numbers  (different for coordinates and colors)
  in the postscript file.
  (The more accurate the format, the longer the file...). There have to
  be a trailing spaces both formats.
*/
void glepsEmitHeader(glWindow w, FILE *file);
void glepsEmitTrailer(glWindow w, FILE *file, int npages);
void glepsGrabEPSFrame(glWindow w, FILE *file, int npage);

/*
\begin{comment}
 *
 *$Log: gleps.h,v $
 *Revision 2.6  2000/11/22 15:20:49  fuhrmann
 *ICE hack
 *1 mpeg/eps per variation
 *variation[i]-> variationi
 *bias=... option
 *
 *Revision 2.5  2000/10/25 10:49:59  fuhrmann
 *crop stuff for graphics
 *
 *Revision 2.4  2000/07/28 13:40:47  schmelze
 **** empty log message ***
 *
 *Revision 2.3  2000/01/13 17:36:41  fuhrmann
 *optmized ps output
 *
 *Revision 2.2  1999/12/21 17:18:55  fuhrmann
 *doc update for gltools-2-3
 *
 *Revision 2.1  1999/06/29 17:56:24  fuhrmann
 *Feedback buffer is used now...
 *
\end{comment}
 */

