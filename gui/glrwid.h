/*
 * $Id: glrwid.h,v 1.4 1997/11/06 16:39:49 fuhrmann Exp $
 */
#include <Xm/XmAll.h>

#define Realize(w) XtRealizeWidget(w)
#define MainLoop(con) XtAppMainLoop(con)
#define Manage(w) XtManageChild(w)
#define Unmanage(w) XtUnmanageChild(w)
#define SetResource(w,s,st) XtVaSetValues(w,s,st,NULL)
#define GetResource(w,s,st) XtVaGetValues(w,s,st,NULL)
#define Callback(w, typ, cproc, data) XtAddCallback(w, typ, cproc, data)
#define streq(wid, str) (strcmp(XtName(wid), str) == 0 )


/* PROTOTYPES */

void ScaleCB (Widget w, XtPointer client_data, XtPointer call_data );

void TextCB (Widget w, XtPointer client_data, XtPointer call_data );

XmString CreateMotifString ( char *string );

Widget GetApplShell ( Widget w );

Widget GetTopLevel ( Widget w );

void Show ( Widget shell );

void Hide ( Widget shell );

Widget CreateShell ( Widget parent, char* title );

Widget CreateScrolledContainer ( Widget parent , Widget work );

Widget CreatePanedContainer ( Widget parent );

Widget CreateMainContainer ( Widget parent );

Widget CreateFormContainer ( Widget parent );

Widget CreatePulldownMenu ( Widget parent );

Widget CreateCascadeButton ( Widget parentPulldown,
                              Widget subMenuId,
                              char *string,
                             char mnemonic );

Widget CreateMenuBar ( Widget parent );

void SetHelpId (Widget menubar, Widget casc);

Widget CreateLabel ( Widget parent,
                        char *string );

Widget CreatePushButton ( Widget parent,
                        char *string );


Widget CreateToggleButton ( Widget parent,
                        char *string );

Widget CreateRadioBox ( Widget parent );

Widget CreateRadioButton ( Widget parent,
                        char *string );

Widget CreateVerticalSlider ( Widget parent, 
                                char *title, 
                               int min,
                               int max,
                               int value,
                               int decimalpoints );

Widget CreateHorizontalSlider ( Widget parent, 
                                char *title, 
                               int min, 
                               int max, 
                               int value, 
                               int decimalpoints);

Widget CreateVerticalContainer ( Widget parent );

Widget CreateHorizontalContainer ( Widget parent );

Widget CreateText ( Widget parent, char *value);

Widget CreateSliderText ( Widget parent,
                       char *title,
                       int min,
                       int max,
                       int value,
                      int decimalpoints );

Widget CreateVerticalSeparator ( Widget parent );

Widget CreateHorizontalSeparator ( Widget parent );

Widget CreateEditableText( Widget parent,
                        int rows,
                        int columns,
                        char *value );

Widget CreateNonEditableText( Widget parent,
                        int rows,
                        int columns,
                        char *value );

Widget CreateRenderArea ( Widget parent, Dimension width, Dimension height );

Widget CreateTextField ( Widget parent, char *label, int length,
                         char *value );

/***************************************************************************/
/*
 * $Log: glrwid.h,v $
 * Revision 1.4  1997/11/06 16:39:49  fuhrmann
 * test
 *
 * Revision 1.3  1997/06/05  17:39:50  fuhrmann
 * Ch. Steinborn, 5.6., slider scales
 *
 * Revision 1.1  1997/04/21  09:47:38  fuhrmann
 * Initial revision
 *
 * Revision 1.1  1997/04/21  09:47:38  fuhrmann
 * Initial revision
 *	
 */
