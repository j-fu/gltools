static char *rcsid= "$Id: glrgui.c,v 1.8 1998/05/18 18:52:59 fuhrmann Exp $";
#include "glrnd.h"
#include "glwin.h"
#include "glrwid.h"
#include "glrgui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>


glRenderer Renderer;



/***************** WIDGET - LIBRARY ********************/

XmString CreateMotifString ( char *string )
{
return XmStringCreateLocalized ( string );
}


/******************************************************/


Widget GetApplShell ( Widget w )
{ 
Widget top;

top = XtParent(w);
while (XtClass(top) != applicationShellWidgetClass) 
	top = XtParent(top);

return top;
}

/******************************************************/

Widget GetTopLevel ( Widget w )
{
Widget top;

top = XtParent(w);
while (XtClass(top) != topLevelShellWidgetClass)
	top=XtParent(top);

return top;
}

/******************************************************/


void Show ( Widget shell )
{
if (XtClass ( shell) == topLevelShellWidgetClass)
	XtPopup ( shell, XtGrabNone );
	else
	printf ("\nShow: Show function is only for topLevelShellWidgetClass\n");
}

/******************************************************/

void Hide ( Widget shell )
{
if (XtClass ( shell )  == topLevelShellWidgetClass )
	XtPopdown ( shell );
	else
	printf ("\nHide: Hide function is only for topLevelShellWidgetClass\n!\n");
}


/******************************************************/


/* Shell */
Widget CreateShell ( Widget parent, char* title )
{
return XtVaCreatePopupShell ( title,
				topLevelShellWidgetClass,
				parent,
				XmNtitle, title,
				XmNdeleteResponse, XmDO_NOTHING,
				NULL );
}

/******************************************************/

/*paned*/
Widget CreatePanedContainer ( Widget parent )
{ 
return XtVaCreateManagedWidget ( "PanedCont",
				xmPanedWindowWidgetClass,
				parent,
				NULL );
}

/******************************************************/

/* main*/
Widget CreateMainContainer ( Widget parent )
{
return XtVaCreateManagedWidget ( "MainCont",
				xmMainWindowWidgetClass,
				parent,
				NULL );
}


/******************************************************/


/* form */
Widget CreateFormContainer ( Widget parent )
{
return XtVaCreateManagedWidget ( "FormCont",
				xmFormWidgetClass,
				parent,
				NULL );
}

/******************************************************/

/*scrolled*/
Widget CreateScrolledContainer ( Widget parent, Widget work )
{
Arg arg[3];
int i=0;
Widget scrolledContainer;

XtSetArg( arg[i], XmNscrollBarDisplayPolicy, XmAS_NEEDED );
i++;
XtSetArg( arg[i], XmNscrollingPolicy, XmAUTOMATIC );
i++;
XtSetArg( arg[i], XmNworkWindow, work );

return XtCreateManagedWidget ( "scolledContainer",
				xmScrolledWindowWidgetClass,
				parent,
				arg,
				i );
}

/******************************************************/


/*pulldown*/
Widget CreatePulldownMenu ( Widget parent )
{
return XmCreatePulldownMenu ( parent, "pulldown", NULL, 0 );
}


/******************************************************/

/*option*/
Widget CreateOptionMenu ( Widget parent )
{
return XmCreateOptionMenu ( parent, "option", NULL, 0);
}


/******************************************************/

/*cascade*/
Widget CreateCascadeButton ( Widget parentPulldown,
				Widget subMenuId,
				char *string,
				char mnemonic )
{
return XtVaCreateManagedWidget ( string,
				xmCascadeButtonWidgetClass,
				parentPulldown,
				XmNlabelString, 
				 XmStringCreateLocalized(string),
				XmNmnemonic, mnemonic,
				XmNsubMenuId, subMenuId,
				NULL );
}

/******************************************************/

/*menubar*/
Widget CreateMenuBar ( Widget parent )
{
Widget menubar = XmCreateMenuBar ( parent, "menubar", NULL, 0 );
XtManageChild ( menubar );
return menubar;
}

/******************************************************/

/*helpId*/
void SetHelpId (Widget menubar, Widget casc)
{
XtVaSetValues( menubar, XmNmenuHelpWidget, casc, NULL);
}

/******************************************************/

/* label */
Widget CreateLabel ( Widget parent,
			char *string )
{
return XtVaCreateManagedWidget ( string,
				xmLabelWidgetClass,
				parent,
				XmNlabelString,
				XmStringCreateLocalized (string),
				NULL );
}

/******************************************************/

/* button */
Widget CreatePushButton ( Widget parent,
			char *string )

{
return XtVaCreateManagedWidget ( string,
				xmPushButtonWidgetClass,
				parent,
				XmNlabelString,
				XmStringCreateLocalized (string),
				NULL);
}

/******************************************************/

/* togglebutton */
Widget CreateToggleButton ( Widget parent,
			char *string )

{
return XtVaCreateManagedWidget ( string,
				xmToggleButtonWidgetClass,
				parent,
				XmNlabelString,
				XmStringCreateLocalized (string),
				XmNindicatorType, XmN_OF_MANY,
				NULL );
}

/******************************************************/

/* radiobox */
Widget CreateRadioBox ( Widget parent )
{
Widget box = XmCreateRadioBox ( parent, 
                        "box",
                        NULL,
                        0 );

XtManageChild ( box );
return box;
}

/******************************************************/


/* radiobutton */
Widget CreateRadioButton ( Widget parent,
			char *string )
{
return XtVaCreateManagedWidget ( string,
				xmToggleButtonWidgetClass,
				parent,
				XmNlabelString, 
				XmStringCreateLocalized (string), 
				XmNindicatorType, XmONE_OF_MANY,
				NULL );
}

/******************************************************/

/* vertical slider */
Widget CreateVerticalSlider ( Widget parent, 
				char *title, 
				int min, 
				int max, 
				int value, 
				int decimalpoints )
{
return XtVaCreateManagedWidget ( title,
				xmScaleWidgetClass,
				parent,
				XmNorientation, XmVERTICAL,
				XmNtitleString,
				XmStringCreateLocalized (title),
				XmNminimum, min,
				XmNmaximum, max,
				XmNvalue, value,
				XmNshowValue, True,
				XmNdecimalPoints, decimalpoints,
				NULL);
}


/******************************************************/


/* horizontal slider */
Widget CreateHorizontalSlider ( Widget parent, 
				char *title, 
				int min, 
				int max, 
				int value, 
				int decimalpoints)
{
return XtVaCreateManagedWidget ( title,
				xmScaleWidgetClass,
				parent,
				XmNorientation, XmHORIZONTAL,
				XmNtitleString, 
				XmStringCreateLocalized (title),
				XmNminimum, min,
				XmNmaximum, max,
				XmNvalue, value,
				XmNshowValue, True,
				XmNdecimalPoints, decimalpoints,
				NULL);
}

/******************************************************/

/* vertical container */
Widget CreateVerticalContainer ( Widget parent )
{
return XtVaCreateManagedWidget ( "VertCont",
				xmRowColumnWidgetClass,
				parent,
				XmNorientation,
				XmVERTICAL,
				NULL );
}

/******************************************************/

/* horizontal container */
Widget CreateHorizontalContainer ( Widget parent )
{
return XtVaCreateManagedWidget ( "HorCont",
                                xmRowColumnWidgetClass,
                                parent,
                                XmNorientation,
                                XmHORIZONTAL,
                                NULL ); 
}

/******************************************************/

Widget CreateText ( Widget parent, char *value)
{
return XtVaCreateManagedWidget ( "text",
				xmTextWidgetClass,
				parent,
				XmNrows, 1,
				XmNcolumns, strlen (value),
				XmNvalue, value,
				NULL );
}


/******************************************************/


/* vertical separator */
Widget CreateVerticalSeparator ( Widget parent )
{
return XtVaCreateManagedWidget ( "VertSeparator",
				xmSeparatorWidgetClass,
				parent,
				XmNorientation, XmVERTICAL,
				NULL );
}

/******************************************************/


/* horizontal separator */
Widget CreateHorizontalSeparator ( Widget parent )
{
return XtVaCreateManagedWidget ( "HorSeparator",
                                xmSeparatorWidgetClass,
                                parent,
                                XmNorientation, XmHORIZONTAL,
                                NULL );
}

/******************************************************/


/* editable text */
Widget CreateEditableText( Widget parent,
			int rows,
			int columns,
			char *value )
{
Widget text= XmCreateScrolledText ( parent, 
				value,
				NULL,
				0 );

XtVaSetValues ( text, XmNrows, rows,
		XmNcolumns, columns,
		XmNeditable, True,
		XmNeditMode, XmMULTI_LINE_EDIT,
		XmNvalue, value ,
		XmNeditable, True,
		NULL);

XtManageChild ( text );
return text;
}

/******************************************************/


/* noneditable text */
Widget CreateNonEditableText( Widget parent,
                        int rows,
                        int columns,
                        char *value )
{
Widget text= CreateEditableText ( parent, rows, columns, value );
XtVaSetValues ( text, XmNeditable, False, NULL );
return text;
}


/******************************************************/

/* render area */
Widget CreateRenderArea ( Widget parent, Dimension width, Dimension height )
{
Pixel black;
Widget draw;
Display *display;
int screen;

display = XtDisplay (parent);
screen = DefaultScreen (display);
black = BlackPixel ( display, screen);

draw = XtVaCreateManagedWidget ( "draw",
				xmDrawingAreaWidgetClass,
				parent,
				XmNwidth, width,
				XmNheight, height,
				XmNbackground, black,
				NULL);
return draw;
}

/******************************************************/


Widget CreateScrolledRenderArea ( Widget parent )
{
Widget renderArea;
Widget scrolled;

scrolled = CreateScrolledContainer ( parent , renderArea );
SetResource(scrolled, XmNheight, 300);
SetResource(scrolled, XmNwidth, 300);
renderArea = CreateRenderArea ( scrolled, 512, 512 );

return renderArea;
}

/******************************************************/

Widget CreateTextField ( Widget parent, char *label, int length,
			 char *value )
{
Widget row;
Widget text;

row=CreateHorizontalContainer ( parent );
CreateLabel ( row, label );
text=XtVaCreateManagedWidget ( label,
				xmTextWidgetClass,
				row,
				XmNrows,1,
				XmNcolumns, length,
				XmNvalue, value,
				XmNeditable, True ,
				NULL );
return text;
}

/******************************************************/

Widget CreateLeftArrow ( Widget parent )
{
return XtVaCreateManagedWidget ( "leftArrow",
				xmArrowButtonWidgetClass,
				parent,
				XmNarrowDirection,
				XmARROW_LEFT,
				NULL);

}

/******************************************************/

Widget CreateRightArrow ( Widget parent )
{
return XtVaCreateManagedWidget ( "rightArrow",
				xmArrowButtonWidgetClass,
				parent,
				XmNarrowDirection,
				XmARROW_RIGHT,
				NULL);
}

/******************************************************/

Widget CreateUpArrow ( Widget parent )
{
return XtVaCreateManagedWidget ( "upArrow",
				xmArrowButtonWidgetClass,
				parent,
				XmNarrowDirection,
				XmARROW_UP,
				NULL);
}

/******************************************************/

Widget CreateDownArrow ( Widget parent )
{
return XtVaCreateManagedWidget ( "downArrow",
				xmArrowButtonWidgetClass,
				parent,
				XmNarrowDirection,
				XmARROW_DOWN,
				NULL);
}

/******************************************************/








/****************************** AUXILIARY FUNCTIONS ***************/





void GetFilenameCB ( Widget w, XtPointer client_data, XtPointer call_data )
{
int action = (int) client_data;
XmFileSelectionBoxCallbackStruct *cbs = (XmFileSelectionBoxCallbackStruct*) call_data;
char *filename;

glRendererState state=glrGetRendererState(Renderer);
glWindow win=glrGetWindow(Renderer);

if (XmStringGetLtoR ( cbs->value, XmFONTLIST_DEFAULT_TAG , &filename ) == 0)
	printf( "\n GetFilenameCB: Error getting XmString !\n");
	else 
	printf ("%s %s %s","GetFilenameCB : Filename : ",filename ,"\n");

XtUnmanageChild(w);

switch (action) {
case LOAD:
  glrRestoreState( Renderer , filename );

  glrSetDialog(Renderer,0);
  glwProcess(win,Renderer);
  glrSetDialog(Renderer,1);

  break;
case SAVEAS:
  glrSaveState( Renderer , filename );

  glrSetDialog(Renderer,0);
  glwProcess(win,Renderer);
  glrSetDialog(Renderer,1);

  break;
case SAVEPICTUREAS:
  {
    FILE *f=fopen(filename,"w");
    glwGrab(glrGetWindow(Renderer),0);
    fclose(f);
  }
  break;
}

}


/******************************************************/


void cancelCB ( Widget w, XtPointer client_data, XtPointer call_data)
{
XtDestroyWidget ( XtParent(w) );
}


/******************************************************/


Widget CreateFileBox ( Widget parent )
{
Widget box;

box = XmCreateFileSelectionDialog ( parent, "box", NULL, 0);
XtVaSetValues ( XtParent(box) , 
		XmNtitle, 
		"Please select filename",
		NULL );
XtAddCallback ( box, XmNcancelCallback, cancelCB, NULL);
XtManageChild (box);

XtUnmanageChild( 
	XmFileSelectionBoxGetChild( box, XmDIALOG_HELP_BUTTON ));

XmUpdateDisplay ( box );

return box;
}


/******************************************************/



Widget CreateInfoDialog (Widget parent, char *string)
{
Widget info;
XmString xms, message;

xms = XmStringCreateLocalized( "Close window" );
message = XmStringCreateLtoR(string , XmSTRING_DEFAULT_CHARSET );

info = XmCreateMessageDialog(parent, "warning", NULL, 0);
XtVaSetValues(info, XmNokLabelString, xms,
			XmNdialogType, XmDIALOG_INFORMATION,
			XmNmessageString, message,
			NULL);
XtVaSetValues ( XtParent(info), XmNtitle, "Info", NULL );
XtManageChild(info);

XtUnmanageChild( 
	XmMessageBoxGetChild( info , XmDIALOG_CANCEL_BUTTON ));

XtUnmanageChild(
	XmMessageBoxGetChild( info, XmDIALOG_CANCEL_BUTTON ));

XtUnmanageChild(
	XmMessageBoxGetChild( info, XmDIALOG_HELP_BUTTON ));

XtAddCallback(info, XmNokCallback, cancelCB, NULL);

XmUpdateDisplay ( info );

return info;
}


/******************************************************/


Widget CreateWarningDialog (Widget parent, char *string)
{
Widget warning;
XmString xms, message;

xms = XmStringCreateLtoR(string , XmSTRING_DEFAULT_CHARSET );
message = XmStringCreateLocalized(string);

warning = XmCreateMessageDialog(parent, "warning", NULL, 0);
XtVaSetValues(warning, XmNdialogType, XmDIALOG_WARNING,
			XmNmessageString, message,
			NULL);
XtVaSetValues ( XtParent(warning), XmNtitle, "Warning", NULL );
XtManageChild(warning);

XtUnmanageChild(
	XmMessageBoxGetChild( warning, XmDIALOG_HELP_BUTTON ));

XtUnmanageChild(
	XmMessageBoxGetChild( warning, XmDIALOG_CANCEL_BUTTON )); 

XmUpdateDisplay ( warning );

return warning;
}


/******************************************************/



Widget CreateQuestionDialog (Widget parent, char *string)
{
Widget question;
XmString xms, message;

xms = XmStringCreateLocalized("Yes");
message = XmStringCreateLocalized(string);

question = XmCreateMessageDialog(parent, "question", NULL, 0);
XtVaSetValues(question, XmNokLabelString, xms,
			XmNdialogType, XmDIALOG_QUESTION,
			XmNmessageString, message,
			NULL);
XtVaSetValues ( XtParent(question), XmNtitle, "Question", NULL );

XtManageChild(question);

XtUnmanageChild(
	XmMessageBoxGetChild( question, XmDIALOG_HELP_BUTTON ));

XtAddCallback(question, XmNcancelCallback, cancelCB, NULL);

XmUpdateDisplay ( question );

return question;
}




/******************** HELPWINDOW **************/




void closeCB(Widget w, XtPointer client_data, XtPointer call_data)
{
  Hide( (Widget) client_data );
}


/******************************************************/



void rowsetCB ( Widget w, XtPointer client_data, XtPointer call_data)
{
   char *value;
   int val;

   XtVaGetValues ( w, XmNvalue, &value, NULL);

   val=atoi(value);

   SetResource( (Widget)client_data, XmNrows, val);
}

/******************************************************/

Widget ShowTextFile ( Widget app , char *filename )
{
  Widget toplevel, main, row, fieldrows, 
  _close, text;
  int fd;
  struct stat status;
  char *buffer;
  
  fd = open(filename, O_RDONLY);
  if (fd == -1) {
	printf("\nCannot open textfile !\n");
	return 0;
	}

  fstat(fd, &status);
  buffer = (char*) malloc(status.st_size* sizeof(char));

  if (read(fd, buffer, status.st_size) == -1) { 
    printf("\nERROR! Cannot reading file !\n"); 
    return  0;
  }

  close(fd); 


  /* create widgets */ 
  toplevel = CreateShell( app, "Helpwindow");
  main=CreateMainContainer(toplevel);
  row=CreateVerticalContainer(main);
  SetResource(row, XmNspacing, 0);
  fieldrows=CreateTextField ( row, "Rows", 2, "");
  CreateHorizontalSeparator(row);
  text=CreateNonEditableText ( row, 30, 50, buffer );
  CreateHorizontalSeparator(row);
  _close=CreatePushButton(row, "Close window");
  Callback(_close, XmNactivateCallback, closeCB, toplevel);
  Callback( fieldrows, XmNactivateCallback, rowsetCB, text);
  Show(toplevel);
  return toplevel;
} 
 


/*************** END WIDGET LIBRARY ******************/ 








void gui(glRenderer rnd )
{
  XtAppContext con;
  Widget appShell;
  int argc=1;
  char *argv[1] = {"render"};
  Atom delete_window;

   /*Renderer-Initialize */
  Renderer=rnd;

  appShell = XtVaAppInitialize ( &con , "appShell",
				 NULL, 0,
				 &argc, argv,
				 NULL,
				 NULL );

  SetResource ( appShell , XmNdeleteResponse , XmDO_NOTHING );

  delete_window = XmInternAtom(XtDisplay(appShell), "WM_DELETE_WINDOW", True);
  XmAddWMProtocolCallback(appShell, delete_window, deleteCB, appShell);

  SetResource ( appShell, XmNallowShellResize, True );
  SetResource ( appShell, XmNx, 520);
  { 
    char xname[32];
    sprintf(xname,"%s GUI",glrGetRendererName(Renderer));
  SetResource ( appShell, XmNtitle, xname );
  }
  CreateCommandWindow ( appShell, rnd );

  /*CreateColorWindow ( appShell, rnd );*/

  Realize  ( appShell );
  MainLoop( con );
} 


/******************************************************/



void UpdateToggles ( all_client_data *stub )
{
int i=0;
glRendererState state=glrGetRendererState(Renderer);
glWindow win=glrGetWindow(Renderer);


if (DEBUG) 
  printf("\nInitialize. Call function.\n");

/*** PULLDOWN ***/
if (state->level_surf) {
  SetResource ( stub->toggleSettingsPD[LEVELSURFACE] , XmNset , True );
  SetResource ( stub->panelRadio[ISOLEVEL] , XmNsensitive , True );
}
else {
  SetResource ( stub->toggleSettingsPD[LEVELSURFACE] , XmNset , False );
  SetResource ( stub->panelRadio[ISOLEVEL] , XmNsensitive , False );
}

if (state->isoline_mode) {
  SetResource ( stub->toggleSettingsPD[ISOLINES] , XmNset , True );
  SetResource ( stub->panelRadio[ISODISTANCE], XmNsensitive, True );
}
else {
  SetResource ( stub->toggleSettingsPD[ISOLINES] , XmNset , False );
  SetResource ( stub->panelRadio[ISODISTANCE], XmNsensitive, False );
}

  
/*if (state->wireframe)
  SetResource ( stub->toggleSettingsPD[WIREFRAME] , XmNset , True );
else
  SetResource ( stub->toggleSettingsPD[WIREFRAME] , XmNset , False );*/


if (state->bg_black)
  SetResource ( stub->toggleSettingsPD[BACKGROUND] , XmNset , True );
else
  SetResource ( stub->toggleSettingsPD[BACKGROUND] , XmNset , False );

if (state->ortho)
  SetResource ( stub->toggleSettingsPD[ORTHOGRAPHIC] , XmNset , True );
else
  SetResource ( stub->toggleSettingsPD[ORTHOGRAPHIC] , XmNset , False );

if (state->gouraud_shading)
  SetResource ( stub->toggleSettingsPD[GOURAUD] , XmNset , True );
else
  SetResource ( stub->toggleSettingsPD[GOURAUD] , XmNset , False );

if  (state->move_model)
  SetResource ( stub->toggleSettingsPD[IMMEDIATE], XmNset , True );
  else
  SetResource ( stub->toggleSettingsPD[IMMEDIATE], XmNset, False );

/*if (state->transparency)
  SetResource ( stub->toggleSettingsPD[TRANSPARENCY] , XmNset , True );
else
  SetResource ( stub->toggleSettingsPD[TRANSPARENCY] , XmNset , False );*/

if (state->show_info)
  SetResource ( stub->toggleSettingsPD[INFO] , XmNset , True );
else
  SetResource ( stub->toggleSettingsPD[INFO] , XmNset , False );


SetResource ( stub->showPlanesRadio[PLANE_X] , XmNset , False );
SetResource ( stub->showPlanesRadio[PLANE_Y] , XmNset , False );
SetResource ( stub->showPlanesRadio[PLANE_Z] , XmNset , False );


/*** PLANE RADIOBOX ***/

switch ( state->plane_dir ) {
	case 'x' : { SetResource ( stub->showPlanesRadio[PLANE_X] , 
				XmNset , 
				True );
	 	   }
		   break;
	case 'y' : { SetResource ( stub->showPlanesRadio[PLANE_Y] , 
				XmNset , 
				True );
		   }
		   break;
	case 'z' : {  SetResource ( stub->showPlanesRadio[PLANE_Z] , 
				XmNset , 
				True );
		   }
		   break;
	}


if (DEBUG)
  printf("\nEnd of initialize.\n");

}


/******************************************************/


UpdateRotTransSlider ( all_client_data *stub )
{
glRendererState state=glrGetRendererState(Renderer);

/* Rotation slider */
SetResource ( stub->sliderRot[ROTX], XmNvalue, (int) state->rotx % ROTMAX );
SetResource ( stub->sliderRot[ROTY], XmNvalue, (int) state->roty % ROTMAX );
SetResource ( stub->sliderRot[ROTZ], XmNvalue, (int) state->rotz % ROTMAX );

/*Translation slider */
SetResource ( stub->sliderTran[TRANX], XmNvalue, (int) (state->tranx*DIVVALUE) );
SetResource ( stub->sliderTran[TRANY], XmNvalue, (int) (state->trany*DIVVALUE) );

}


/******************************************************/


UpdateLightSlider ( all_client_data *stub )
{
glRendererState state=glrGetRendererState(Renderer);

/*Light slider*/
SetResource ( stub->sliderLight[LIGHTX], XmNvalue , (int) state->ltx % ROTMAX);
SetResource ( stub->sliderLight[LIGHTY], XmNvalue , (int) state->lty % ROTMAX);
}

/******************************************************/


UpdateDistanceSlider ( all_client_data *stub )
{
glRendererState state=glrGetRendererState(Renderer);

/* isodistance slider */

SetResource (  stub->sliderIsodistance,
		XmNvalue, 
		(int) (state->isoline_distance*DIVVALUE) );



}

/******************************************************/

UpdateAspectSlider ( all_client_data *stub )
{
glRendererState state=glrGetRendererState(Renderer);

/* aspect slider */

SetResource ( stub->sliderRelation[ASPECTXY], XmNvalue, (int) (state->xyasp*DIVVALUE) );

SetResource ( stub->sliderRelation[ASPECTZ], XmNvalue, (int) (state->zasp*DIVVALUE) );  
}

/******************************************************/


UpdateIsoSlider ( all_client_data *stub )
{
glRendererState state=glrGetRendererState(Renderer);

/* Isosurface slider */

SetResource ( stub->sliderIsolevel, 
	XmNvalue, (int)
	( 
	(state->level - state->fmin) / 
	(state->fmax - state->fmin) 
	* SLIDER_MAX )
	);

}

/******************************************************/


UpdatePlaneSlider ( all_client_data *stub )
{
glRendererState state=glrGetRendererState(Renderer);

/* Plane slider */
switch (state->plane_dir) {
	case 'x' :  {
		   
		    SetResource ( stub->sliderCuttingplane,
		    		  XmNvalue, (int)
				  (
				  (state->plane_d - state->xmin) /
				  (state->xmax - state->xmin)
				  * SLIDER_MAX)
				  );
		    }

		    break;

	case 'y' :  {

		    SetResource ( stub->sliderCuttingplane,
				  XmNvalue, (int)
				  (
				  (state->plane_d - state->ymin) /
				  (state->ymax - state->ymin)
				  * SLIDER_MAX)
				  );


		    }

		    break;

 	case 'z':   {

		    SetResource ( stub->sliderCuttingplane,
				  XmNvalue, (int)
				  (
				  (state->plane_d - state->zmin) /
				  (state->zmax - state->zmin)
				  * SLIDER_MAX)
				  );
		     }
	}
}


/******************************************************/


void UpdateSlider ( all_client_data *stub )
{
glRendererState state=glrGetRendererState(Renderer);

UpdateRotTransSlider ( stub );
UpdateLightSlider ( stub );
UpdateAspectSlider ( stub );
UpdateDistanceSlider ( stub );
UpdateIsoSlider ( stub );
UpdatePlaneSlider ( stub );

}

		    		    
/******************************************************/

void CreateCommandWindow(Widget appShell, glRenderer rnd )
{

int i;

/*pulldown*/
Widget menubar, pdFile, 
		pdActions, 		
		pdSettings, 
		pdExtension,
		pdHelp, 
		cascadeHelp;
Widget buttonFilePD[XtNumber(fileStrings)];
/*Widget buttonActionsPD[XtNumber(actionStrings)];*/
Widget buttonHelpPD[XtNumber(helpStrings)];

/* containers */
Widget mainContainer;
Widget formContainer;
Widget vertContainer, 
  hor0Container,
  hor1Container, 
  hor2Container, 
  hor3Container,
  hor4Container; 
Widget editContainer[XtNumber(panelStrings)];
Widget onecol, twocol, threecol;
Widget radiobox;
Widget dummy;

int width ;


all_client_data *stub;
stub =  (all_client_data*) malloc( sizeof(all_client_data) );



/*************************************************************/
mainContainer = CreateMainContainer ( appShell );
/**************************************************************/


/*************************************************************/
menubar = CreateMenuBar ( mainContainer );
pdFile = CreatePulldownMenu ( menubar );
CreateCascadeButton ( menubar, pdFile, "File", 'F' );
for (i=0; i<XtNumber(fileStrings); i++) {
	buttonFilePD[i] = CreatePushButton ( pdFile, fileStrings[i] );
	Callback ( buttonFilePD[i], XmNactivateCallback, pdFileCB, (XtPointer) i );
	}

/*************************************************************/

pdSettings = CreatePulldownMenu(menubar);
CreateCascadeButton ( menubar, pdSettings, "Settings", 'S');
for (i=0; i<XtNumber(settingStrings); i++) {
	stub->toggleSettingsPD[i]=CreateToggleButton(pdSettings,  settingStrings[i]);
	Callback ( stub->toggleSettingsPD[i], XmNvalueChangedCallback, pdSettingCB, (XtPointer) i );
	Callback( stub->toggleSettingsPD[i], XmNvalueChangedCallback, setAllCB, (XtPointer) stub );
	}

/*************************************************************/

{

  glRendererState state=glrGetRendererState(Renderer);
  if (state->noptions>0) {
    pdExtension = CreatePulldownMenu(menubar);
    CreateCascadeButton ( menubar , pdExtension , "Application" , 'A');
    for (i=0; i<state->noptions; i++) {
        stub->toggleExtensionsPD[i] = 
      CreateToggleButton(pdExtension, state->options[i].key );
      Callback ( stub->toggleExtensionsPD[i], 
		 XmNvalueChangedCallback, pdExtensionCB, (XtPointer) i );
    }
  }
}


/****************************************************************/
pdHelp = CreatePulldownMenu ( menubar );
cascadeHelp = CreateCascadeButton ( menubar, pdHelp, "Help", 'H' );
for (i=0; i<XtNumber(helpStrings); i++) {
	buttonHelpPD[i]=CreatePushButton ( pdHelp, helpStrings[i] );
	Callback(buttonHelpPD[i], XmNactivateCallback, pdHelpCB, (XtPointer) i ); 
}

SetHelpId ( menubar, cascadeHelp );

/*****************************************************************/

formContainer = CreateFormContainer ( mainContainer );

vertContainer = CreateVerticalContainer ( formContainer );

hor0Container = CreateFormContainer ( vertContainer );

/*****************************************************************/

stub->refreshButton = CreatePushButton ( hor0Container , "Refresh" );
SetResource ( XtParent(stub->refreshButton), XmNisAligned, False );
SetResource ( stub->refreshButton, XmNalignment, XmALIGNMENT_CENTER );

SetResource ( stub->refreshButton , XmNleftAttachment , XmATTACH_FORM );
SetResource ( stub->refreshButton , XmNleftOffset , 30  );
SetResource ( stub->refreshButton , XmNtopAttachment , XmATTACH_FORM );
SetResource ( stub->refreshButton , XmNtopOffset , 10 );
SetResource ( stub->refreshButton , XmNbottomAttachment , XmATTACH_FORM );
SetResource ( stub->refreshButton , XmNbottomOffset , 10 );

Callback ( stub->refreshButton , XmNactivateCallback, refreshCB, NULL );

stub->glButton = CreatePushButton ( hor0Container , "Suspend GUI" );
SetResource ( XtParent(stub->glButton), XmNisAligned, False );
SetResource ( stub->glButton, XmNalignment, XmALIGNMENT_CENTER );

SetResource ( stub->glButton , XmNtopAttachment , XmATTACH_FORM );
SetResource ( stub->glButton , XmNtopOffset , 10 );
SetResource ( stub->glButton , XmNrightAttachment , XmATTACH_FORM );
SetResource ( stub->glButton , XmNrightOffset , 30 );
SetResource ( stub->glButton , XmNbottomAttachment , XmATTACH_FORM );
SetResource ( stub->glButton , XmNbottomOffset , 10 );

Callback ( stub->glButton , XmNactivateCallback, glCB, (XtPointer) stub );

CreateHorizontalSeparator ( vertContainer );

/*************************************************************************/

CreateLabel ( vertContainer , "Panels" );

/*************************************************************************/

hor1Container = CreateHorizontalContainer ( vertContainer );

/*************************************************************************/

onecol = CreateVerticalContainer ( hor1Container );

for ( i=0; i<4; i++)
  stub->panelRadio[i] = CreateToggleButton ( onecol, panelStrings[i] );

/************************************************************************/

twocol = CreateVerticalContainer ( hor1Container );

for (i=4; i<XtNumber(panelStrings); i++)
  stub->panelRadio[i] = CreateToggleButton ( twocol, panelStrings[i] );


/************************************************************************/

threecol = CreateVerticalContainer ( hor1Container );
CreateLabel ( threecol, "Show planes" );
radiobox = CreateRadioBox ( threecol );
SetResource ( XtParent(radiobox), XmNspacing, 0 );
for (i=0; i<XtNumber(planesStrings); i++) {
stub->showPlanesRadio[i] = CreateRadioButton ( radiobox, planesStrings[i] );
Callback (stub->showPlanesRadio[i], XmNvalueChangedCallback, showPlanesCB, (XtPointer) stub );
}

/************************************************************************/

CreateHorizontalSeparator ( vertContainer ) ;

/*****************************************************************/

UpdateToggles ( stub );

/*****************************************************************/


/***************** CONTAINERS FOR SLIDER ********************************/


for (i=0; i<XtNumber(panelStrings); i++) {
	editContainer[i]=CreateVerticalContainer(vertContainer);
	Unmanage(editContainer[i]);
	}

hor2Container = CreateVerticalContainer ( editContainer[DISPLAYLISTS] );

dummy = CreateLabel ( hor2Container, panelStrings[DISPLAYLISTS] );

SetResource ( XtParent ( dummy ) , XmNisAligned , False );
SetResource ( dummy , XmNalignment, XmALIGNMENT_CENTER );

hor3Container  = CreateHorizontalContainer ( hor2Container );
SetResource ( hor3Container , XmNspacing , 0 );

for (i=0; i<XtNumber(dpylistStrings); i++) {

  stub->dpyVertContainer[i] = CreateVerticalContainer ( hor3Container );
  SetResource ( stub->dpyVertContainer[i] , XmNspacing , 0 );

  stub->showDpylistToggle[i] = CreateToggleButton ( stub->dpyVertContainer[i] , 
						dpylistStrings[i] );
  
  Callback ( stub->showDpylistToggle[i],
                XmNvalueChangedCallback,
                showDpylistCB,
                (XtPointer) i );

  Callback ( stub->showDpylistToggle[i],
                XmNvalueChangedCallback,
                getActiveDisplayListsCB ,
                (XtPointer) stub );

  stub->storeDpylistButton[i] = CreatePushButton ( stub->dpyVertContainer[i] ,
                               dpylistStrings[i] );

  SetResource (  XtParent ( stub->storeDpylistButton[i] ) , XmNisAligned , False );
  SetResource (  stub->storeDpylistButton[i] , XmNalignment, XmALIGNMENT_CENTER );

  Callback ( stub->storeDpylistButton[i] ,
                XmNactivateCallback ,
                storeDpylistCB ,
                (XtPointer) i );

  Callback ( stub->storeDpylistButton[i] ,
                XmNactivateCallback ,
                getActiveDisplayListsCB ,
                (XtPointer) stub );
}


/*************************************************************

hor4Container = CreateHorizontalContainer ( hor2Container );

for (i=0; i<XtNumber(dpylistStrings); i++) {
  stub->storeDpylistButton[i] = CreatePushButton ( hor4Container , 
				dpylistStrings[i] );

  Callback ( stub->storeDpylistButton[i] , 
		XmNactivateCallback , 
		storeDpylistCB ,
		(XtPointer) i );

  Callback ( stub->storeDpylistButton[i] ,
		XmNactivateCallback ,
		getActiveDisplayListsCB ,
		(XtPointer) stub );

}

CreateLabel ( hor4Container , "Store" );

*************************************************************************/

dummy = CreateLabel ( editContainer[ROTATION], panelStrings[ROTATION]  );
SetResource ( XtParent(dummy), XmNisAligned, False );
SetResource( dummy, XmNalignment, XmALIGNMENT_CENTER );

for (i=0; i<XtNumber(sliderRotStrings); i++) {
stub->sliderRot[i] = CreateHorizontalSlider ( 
			editContainer[ROTATION], 
			sliderRotStrings[i],
			ROTMIN, 
			ROTMAX, 
			0, 
			0);


Callback (stub->sliderRot[i], 
	  XmNdragCallback,
	  sliderRotCB,
	  (XtPointer) i );
Callback (stub->sliderRot[i], 
	  XmNvalueChangedCallback,
	  sliderRotCB,
	  (XtPointer) i );

}





/************************************************************************/

dummy = CreateLabel ( editContainer[TRANSLATION], panelStrings[TRANSLATION] );
SetResource ( XtParent(dummy), XmNisAligned, False );
SetResource( dummy, XmNalignment, XmALIGNMENT_CENTER );


for (i=0; i<XtNumber(sliderTranStrings); i++) {
  stub->sliderTran[i] = CreateHorizontalSlider ( 
						editContainer[TRANSLATION],
						sliderTranStrings[i],
						-DIVVALUE, 
						DIVVALUE, 
						0, 
						2  );

Callback (stub->sliderTran[i], 
	  XmNdragCallback,
	  sliderTranCB,
	  (XtPointer) i );
Callback (stub->sliderTran[i], 
	  XmNvalueChangedCallback,
	  sliderTranCB,
	  (XtPointer) i );

}


/*************************************************************************/ 

dummy = CreateLabel ( editContainer[ZOOM], panelStrings[ZOOM] );
SetResource ( XtParent(dummy), XmNisAligned, False );
SetResource( dummy, XmNalignment, XmALIGNMENT_CENTER );

stub->sliderZoom = CreateHorizontalSlider ( 
			editContainer[ZOOM] , 
			sliderZoomString ,
			0 , 
			300, 
			100, 
			2 );

Callback (stub->sliderZoom, 
	  XmNdragCallback,
	  sliderZoomCB,
	  (XtPointer) stub );
Callback (stub->sliderZoom, 
	  XmNvalueChangedCallback,
	  sliderZoomCB,
	  (XtPointer) stub );


/*************************************************************************/

dummy = CreateLabel ( editContainer[LIGHT], panelStrings[LIGHT] );
SetResource ( XtParent(dummy), XmNisAligned, False );
SetResource( dummy, XmNalignment, XmALIGNMENT_CENTER );

for (i=0; i<XtNumber(sliderLightStrings); i++) {
stub->sliderLight[i] = CreateHorizontalSlider (
					       editContainer[LIGHT], 
					       sliderLightStrings[i],
					       ROTMIN, 
					       ROTMAX, 
					       0, 
					       0);

Callback (stub->sliderLight[i], 
	  XmNdragCallback,
	  sliderLightCB,
	  (XtPointer) i );
Callback (stub->sliderLight[i], 
	  XmNvalueChangedCallback,
	  sliderLightCB,
	  (XtPointer) i );


}

/**************************************************************************/

dummy = CreateLabel ( editContainer[CAMERA], panelStrings[CAMERA] );
SetResource ( XtParent(dummy), XmNisAligned, False );
SetResource( dummy, XmNalignment, XmALIGNMENT_CENTER );

for (i=0; i<XtNumber(sliderCameraStrings); i++) {
stub->sliderCamera[i] = CreateHorizontalSlider ( 
						 editContainer[CAMERA],
						 sliderCameraStrings[i],
						 -180,
						 180,
						 0,
						 0 );

Callback (stub->sliderCamera[i], 
	  XmNdragCallback,
	  sliderCameraCB,
	  (XtPointer) i );
Callback (stub->sliderCamera[i], 
	  XmNvalueChangedCallback,
	  sliderCameraCB,
	  (XtPointer) i );


}

/**************************************************************************/

dummy = CreateLabel ( editContainer[CUTTINGPLANE], panelStrings[CUTTINGPLANE] );
SetResource ( XtParent(dummy), XmNisAligned, False );
SetResource( dummy, XmNalignment, XmALIGNMENT_CENTER );

stub->sliderCuttingplane = CreateHorizontalSlider ( 
						   editContainer[CUTTINGPLANE],
						   sliderCuttingplaneString,
						   0,
						   SLIDER_MAX ,
						   0,
						   0 );

SetResource ( stub->sliderCuttingplane , XmNshowValue , False );

Callback (stub->sliderCuttingplane, 
	  XmNdragCallback,
	  sliderCuttingplaneCB,
	  (XtPointer) stub );
Callback (stub->sliderCuttingplane, 
	  XmNvalueChangedCallback,
	  sliderCuttingplaneCB,
	  (XtPointer) stub );



/***************************************************************************/

dummy = CreateLabel ( editContainer[ISODISTANCE], panelStrings[ISODISTANCE] );
SetResource ( XtParent(dummy), XmNisAligned, False );
SetResource( dummy, XmNalignment, XmALIGNMENT_CENTER );

stub->sliderIsodistance = CreateHorizontalSlider ( 
						  editContainer[ISODISTANCE],
						  sliderIsodistanceString,
						  0,
						  100,
						  50,
						  2 );

SetResource ( stub->sliderIsodistance , XmNshowValue , False );

Callback (stub->sliderIsodistance, 
	  XmNdragCallback,
	  sliderIsodistanceCB,
	  (XtPointer) stub );
Callback (stub->sliderIsodistance, 
	  XmNvalueChangedCallback,
	  sliderIsodistanceCB,
	  (XtPointer) stub );



/****************************************************************************/

dummy = CreateLabel ( editContainer[ISOLEVEL], panelStrings[ISOLEVEL] );
SetResource ( XtParent(dummy), XmNisAligned, False );
SetResource( dummy, XmNalignment, XmALIGNMENT_CENTER );

stub->sliderIsolevel = CreateHorizontalSlider (
						editContainer[ISOLEVEL],
						sliderIsolevelString,
						0,
						SLIDER_MAX,
						0,
						0 );

SetResource ( stub->sliderIsolevel , XmNshowValue , False );

Callback (stub->sliderIsolevel, 
	  XmNdragCallback,
	  sliderIsolevelCB,
	  (XtPointer) stub );
Callback (stub->sliderIsolevel, 
	  XmNvalueChangedCallback,
	  sliderIsolevelCB,
	  (XtPointer) stub );


/****************************************************************************/

dummy = CreateLabel (editContainer[RELATION], panelStrings[RELATION] );
SetResource ( XtParent(dummy), XmNisAligned, False );
SetResource ( dummy, XmNalignment, XmALIGNMENT_CENTER );

for (i=0; i< XtNumber(sliderRelationStrings); i++) {
stub->sliderRelation[i] = CreateHorizontalSlider (
				editContainer[RELATION],
				sliderRelationStrings[i],
				10,
				200,
				50,
				2);
				

Callback( stub->sliderRelation[i],
	XmNdragCallback,
	sliderRelationCB,
	(XtPointer) i );
Callback (stub->sliderRelation[i],
	XmNvalueChangedCallback,
	sliderRelationCB,
	(XtPointer) i );

}



				


/* Callback for pulldown 'panels', because editContainer now created */
for (i=0; i<XtNumber(panelStrings); i++) 
  Callback( stub->panelRadio[i], XmNvalueChangedCallback, pdPanelCB, (XtPointer) editContainer[i] );


UpdateSlider ( stub );

}

/****************************************************************************/

void CreateColorWindow ( Widget appShell, glRenderer rnd )
{
int i=0;
Widget shell;
Widget row;
XColor screenReturn;
XColor defineColor;
Colormap colormap;
GC gc;
int screen;
Display *display ;
char *color[] = {	"red",
			"green",
			"blue",
			"white",
			"black" };
Widget label[XtNumber(color)];

display = XtDisplay (appShell);
screen = DefaultScreen(display);
colormap = DefaultColormap (display, screen);
gc = DefaultGC ( display , screen );

shell = CreateShell( appShell, "Color Window" );
row = CreateVerticalContainer ( shell);

for (i=0; i<XtNumber(color); i++) {
	if (XAllocNamedColor( display,
				colormap,
				color[i],
				&screenReturn,
				&defineColor) == BadColor )
	printf("%s %s %s","\nError allocate color" , color[i] , "!\n");
	else {
	printf("\nCreate Labels ... \n");
	label[i] = CreateLabel(row, color[i]);
	SetResource ( label[i] , XmNbackground , defineColor.pixel );
	}
  }

Show (shell);

}


/****************************************************************************/


/************************************************/
/*************** CALLBACKS **********************/
/************************************************/


void printStatus ( void )
{
glRendererState state=glrGetRendererState(Renderer);

if (DEBUG) printf("\nprintStatus.\n");

printf(" %s %f %f %f %f %f %f" , "\n xmin xmax ymin ymax zmin zmax" , state->xmin , state->xmax , state->ymin , state->ymax , state->zmin , state->zmax );
}

/******************************************************/


void setAllCB (Widget w, XtPointer client_data, XtPointer call_data)
{
if (DEBUG) 
	printf("\nsetAllCB: Callback.\n");
UpdateToggles ( (all_client_data*) client_data );
}

/******************************************************/

void pdFileCB ( Widget w, XtPointer client_data, XtPointer call_data)
{
int i = (int) client_data;
Widget box;
glRendererState state=glrGetRendererState(Renderer);
glWindow win=glrGetWindow(Renderer);

if (DEBUG) printf ( "\npdFileCB: Callback.\n");

switch (i) 
	{
	case LOAD: if (DEBUG) printf ("\nLoad state!\n");
	  box=CreateFileBox ( (Widget) XtParent(w) );
	  Callback(box, XmNokCallback, GetFilenameCB, (XtPointer) i );
	  break;
	case SAVE: if (DEBUG) printf ("\nSave state!\n");
	  glrSaveState( Renderer , NULL );
	  break;
	case SAVEAS: if (DEBUG) printf("\nSave state as!\n");
	  box=CreateFileBox ( (Widget) XtParent(w) );
	  Callback(box, XmNokCallback, GetFilenameCB, (XtPointer) i );
	  break;
	case SAVEPICTURE:
	case PRINT: if (DEBUG) printf ("\nPrint!\n");
	  glwGrabAndPrint( glrGetWindow(Renderer));
	  break;
	case SAVEPICTUREAS:  if (DEBUG) printf("\nSave Picture as!\n");
	  box=CreateFileBox ( (Widget) XtParent(w) );
	  Callback(box, XmNokCallback, GetFilenameCB, (XtPointer) i );
	  break;
	case QUIT: if (DEBUG)  printf("\nQuit!\n");
	  box=CreateQuestionDialog ( (Widget) XtParent(w), "Do you want really exit ?");
	  Callback(box, XmNokCallback, exitCB, NULL);
	  break; 
	}
}

/******************************************************/
		
void pdSettingCB ( Widget w, XtPointer client_data, XtPointer call_data)
{

int i = (int) client_data;
XmToggleButtonCallbackStruct *cbs = (XmToggleButtonCallbackStruct*) call_data;
glRendererState state=glrGetRendererState(Renderer);
glWindow win=glrGetWindow(Renderer);

if (DEBUG) printf ("\npdSettingCB: Callback.\n");

switch (i) 

	{
	case LEVELSURFACE: if (cbs->set) {
	  if (DEBUG) printf("\nLevel surface on !\n");
	  state->level_surf = TRUE;
	  state->isoline_mode = FALSE;
	} else {
	  if (DEBUG) printf ("\nLevel surface off!\n");	
	  state->level_surf = FALSE;
	}
	glrSetDataValid ( Renderer , 0 );
	break;



	case ISOLINES: if (cbs->set) {
	  if (DEBUG) printf("\nIsolines on !\n");
	  state->isoline_mode = TRUE;
	  state->level_surf = FALSE;
	} else {
	  if (DEBUG) printf ("\nIsolines off!\n");
	  state->isoline_mode = FALSE;
	}
	glrSetDataValid ( Renderer , 0 );
	break;



	/*case WIREFRAME: if (cbs->set) {
	  if (DEBUG) printf("\nShow data as wireframe on!\n");
	  state->wireframe=TRUE;
	} else {
	  if (DEBUG) printf("\nShow data as wireframe off\n");
	  state->wireframe=FALSE;
	}
	break;*/



	case BACKGROUND: if (cbs->set) {
	  if (DEBUG) printf("\nSwitch background to black!\n");
	  state->bg_black = TRUE;
	} else {
	  if (DEBUG) printf("\n Switch background to white!\n");
	  state->bg_black=FALSE;
	}
	break;



	case ORTHOGRAPHIC: if (cbs->set) {
	  if (DEBUG) printf("\nOrthographic projection on!\n");
	  state->ortho=TRUE;
	} else {
	  if (DEBUG) printf("\nOrthographic projection off!\n");
	  state->ortho=FALSE;
	}
	break;



	case GOURAUD: if (cbs->set) {
	  if (DEBUG) printf("\nGouraud shading on!\n");
	  state->gouraud_shading=TRUE;
	} else {
	  if (DEBUG) printf("\nGouraud Shading off!\n");
	  state->gouraud_shading=FALSE;
	}
	break;


	case IMMEDIATE: if (cbs->set) {
	  if (DEBUG) printf("\nImmediate Mode on!\n");
	  /*immediateMode=TRUE;*/
	  state->move_model=TRUE;
	  } else {
		if (DEBUG) printf("\nImmediate Mode off!\n");
		/*immediateMode=FALSE;*/
		state->move_model=FALSE;
		}
	break;
		


	/*case TRANSPARENCY: if (cbs->set) {
	  if (DEBUG) printf("\nTransparency on!\n");
	  state->transparency=TRUE;
	} else {
	  if (DEBUG) printf("\nTransparency off!\n");
	  state->transparency=FALSE;
	}
	break;*/




	case INFO: if (cbs->set) {
	  if (DEBUG) printf("\nInfo on!\n");
	  state->show_info=TRUE;
	} else {
	  if (DEBUG) printf("\nInfo off!\n");
	  state->show_info=FALSE;
	}
	break;
	}



glrSetDialog(Renderer,0);
glwProcess(win,Renderer);
glrSetDialog(Renderer,1);

}


/******************************************************/


void pdPanelCB ( Widget w, XtPointer client_data, XtPointer call_data )
{
XmToggleButtonCallbackStruct *cbs = (XmToggleButtonCallbackStruct*)call_data;

if (DEBUG) 
	printf("%s" , "\npdPanelCB . Callback.\n");

if (cbs->set) { if (DEBUG) printf("editContainer on.\n");
		Manage( (Widget) client_data ); }
		else {
		if (DEBUG) printf("editContainer off.\n");
		Unmanage( (Widget) client_data);
		}

}

/******************************************************/

void pdExtensionCB ( Widget w, XtPointer client_data, XtPointer call_data )
{
XmToggleButtonCallbackStruct *cbs = (XmToggleButtonCallbackStruct*) call_data;
int i = (int) client_data;
glRendererState state=glrGetRendererState(Renderer);
glWindow win=glrGetWindow(Renderer);
if (cbs->set) 
  state->options[i].val=1;
else
  state->options[i].val=0;
  
if (DEBUG)
	printf("%s", "\npdExtensionCB. Callback.\n");

if (cbs->set) { if (DEBUG) printf("%s %d %s","\non extension ", i+1 , "\n" );
	} else {
	if (DEBUG) printf("%s %d %s","\noff extension ", i+1 , "\n" );
	}
glrSetDialog(Renderer,0);
glwProcess(win,Renderer);
glrSetDialog(Renderer,1);


}

/******************************************************/

		
void pdHelpCB( Widget w, XtPointer client_data, XtPointer call_data )
{
int i= (int) client_data;

char *glToolsInfo = 
	"GLTools is an OpenGL based toolbox\n"
	"for online visualisation.\n"
	"Author: Juergen Fuhrmann \n"
	"<fuhrmann@wias-berlin.de>";

char *guiInfo =
	"glrgui is a Motif based gui.\n"
	"Author: Christian Steinborn \n"
	"<s3497@fb1.fhtw-berlin.de>";	
	

if (DEBUG)
	printf ("\nhelpCB : Callback!\n");

switch (i) 
	{ 
	case RENDER: if (DEBUG) printf ("\nTo render !\n");
		CreateInfoDialog (GetApplShell(w), 
			glToolsInfo );
		break;
	case GUI: if (DEBUG) printf ("\nInfo!\n");
		CreateInfoDialog ( GetApplShell(w), guiInfo );
		break;
	case HELP: if (DEBUG) printf ("\nLegend!\n");
		ShowTextFile ( GetApplShell(w), "READ.ME");
		break;
	}
}


/******************************************************/


void refreshCB ( Widget w, XtPointer client_data, XtPointer call_data )
{
glRendererState state=glrGetRendererState(Renderer);
glWindow win=glrGetWindow(Renderer);

if (DEBUG) printf("\nrefreshCB : Callback.\n");

glrSetDialog(Renderer,0);
glwProcess(win,Renderer);
glrSetDialog(Renderer,1);
}


/******************************************************/

void glCB ( Widget w, XtPointer client_data, XtPointer call_data )
{
all_client_data *stub = (all_client_data*) client_data;
glRendererState state=glrGetRendererState(Renderer);
glWindow win=glrGetWindow(Renderer);

if (DEBUG) printf("\nglCB : Callback.\n");

state->what_done=GLR_ROTATE;

glrSetDialog(Renderer,1);
glwSetControlMode(win,GLW_EVENT_DRIVEN);
glwProcess(win,Renderer);
glwSetControlMode(win,GLW_APPLICATION_DRIVEN);

printf("\nBack from GL-window.\n");

CreateInfoDialog ( GetApplShell(w) , "GUI is now activ !" );


UpdateToggles ( stub );
UpdateSlider ( stub );

}

/******************************************************/



void showPlanesCB ( Widget w, XtPointer client_data, XtPointer call_data )
{
XmToggleButtonCallbackStruct *cbs = (XmToggleButtonCallbackStruct*) call_data;
all_client_data *stub = (all_client_data*) client_data;
glRendererState state=glrGetRendererState(Renderer);
glWindow win=glrGetWindow(Renderer);

if (DEBUG) printf ( "\nshowPlanesCB\n");

if (cbs->set) {

if streq(w,"X-Plane") {
	if (DEBUG) printf("\nshowPlanesCB: Level X on!\n");
  	glrXSetPlane(Renderer,'x',state->plane_d);
  	state->plane_dir='x'; }

else if streq(w,"Y-Plane") {
	if (DEBUG) printf("\nshowPlanesCB: Level Y on!\n");
  	glrXSetPlane(Renderer,'y',state->plane_d);
  	state->plane_dir='y'; }

else if streq(w,"Z-Plane") {
	if (DEBUG) printf("\nshowPlanesCB: Level Z on!\n");
  	glrXSetPlane(Renderer,'z',state->plane_d);
  	state->plane_dir='z'; }

	}

UpdatePlaneSlider ( stub );

glrSetDataValid ( Renderer , 0 );
	
glrSetDialog(Renderer,0);
glwProcess(win,Renderer);
glrSetDialog(Renderer,1);

}

/******************************************************/


void showDpylistCB (  Widget w, XtPointer client_data, XtPointer call_data )
{
XmToggleButtonCallbackStruct *cbs = (XmToggleButtonCallbackStruct*) call_data;
int i = (int) client_data;
glRendererState state=glrGetRendererState(Renderer);
glWindow win=glrGetWindow(Renderer);

if (cbs->set) {
  if (DEBUG) printf("%s %d %s" , "\ndpyCB : Show display list ", i+1 ,".\n");
  /* SHOW i+1 ! */
  state->show_object[i+1]=TRUE;
} else {
  if (DEBUG) printf("%s %d %s" , "\ndpyCB : Delete display list ", i+1 ,".\n");
  /* DELETE i+1 ! */
  state->show_object[i+1]=FALSE;
}

glrSetDialog(Renderer,0);
glwProcess(win,Renderer);
glrSetDialog(Renderer,1);
    
}


/******************************************************/


void storeDpylistCB ( Widget w, XtPointer client_data, XtPointer call_data )
{
int i = (int) client_data;
glRendererState state=glrGetRendererState(Renderer);
glWindow win=glrGetWindow(Renderer);

if (DEBUG) 
	printf ( "%s %d %s","rememberDpylistCB : Remember display list ",i+1,".\n");

/* STORE LIST i+1 */

glrRememberObject(Renderer,i+1);

glrSetDialog(Renderer,0);
glwProcess(win,Renderer);
glrSetDialog(Renderer,1);
}


/******************************************************/


void getActiveDisplayListsCB ( Widget w, XtPointer client_data, XtPointer call_data )
{
all_client_data *stub = (all_client_data*) client_data;
int i=0;
unsigned char set;

for (i=0; i<XtNumber(dpylistStrings); i++) {
	XtVaGetValues ( stub->showDpylistToggle[i], XmNset, &set, NULL);
	if (set) {
		stub->dpylistActive[i] = TRUE;
		if (DEBUG) printf("%s %d %s","\ngetActiveDisplayListsCB : Active Display Lists : ", i+1, ".\n");
	}
    }
}	


/******************************************************/

void GetStatus ( int status )
{
switch (status) {
	case XmCR_DRAG: if (DEBUG) printf("\nValue changed.\n");
				 glrShowModel ( Renderer, 0 );
				 break;
	case XmCR_VALUE_CHANGED : if (DEBUG) printf("\nButton released.\n");
				  glrShowModel ( Renderer , 1 );
				  break;
	}
}

/******************************************************/

void sliderRotCB ( Widget w, XtPointer client_data, XtPointer call_data )
{
int i = (int) client_data;
XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct*) call_data;
glRendererState state=glrGetRendererState(Renderer);
glWindow win=glrGetWindow(Renderer);
int value;
short status;

XtVaGetValues ( w, XmNvalue, &value, NULL);

GetStatus (cbs->reason);

switch (i) {
case ROTX: if (DEBUG) printf("\nX-Rotation\n");
  state->rotx = (double) value;
  break;
case ROTY: if (DEBUG) printf("\nY-Rotation\n");
  state->roty = (double) value;
  break;
case ROTZ: if (DEBUG) printf("\nZ-Rotation\n");
  state->rotz = (double) value;
  break;
}

state->what_done=GLR_ROTATE;
glrSetDialog(Renderer,0);
glwProcess(win,Renderer);
glrSetDialog(Renderer,1);
}


/******************************************************/


void sliderTranCB ( Widget w, XtPointer client_data, XtPointer call_data )
{
int i = (int) client_data;
XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct*) call_data;
glRendererState state = glrGetRendererState(Renderer);
glWindow win=glrGetWindow(Renderer);
int value;

XtVaGetValues ( w , XmNvalue, &value, NULL ); 

GetStatus (cbs->reason);

switch (i) {
case TRANX: if (DEBUG) printf("\nTranslation X\n");
  state->tranx = (double) value/DIVVALUE;
  break;
case TRANY: if (DEBUG) printf("\nTranslation Y\n");
  state->trany = (double) value/DIVVALUE;
  break;
}

state->what_done=GLR_TRANSLATE;
glrSetDialog(Renderer,0);
glwProcess(win,Renderer);
glrSetDialog(Renderer,1);
}

/******************************************************/


void sliderZoomCB ( Widget w, XtPointer client_data, XtPointer call_data )
{
XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct*) call_data;
glRendererState state = glrGetRendererState(Renderer);
glWindow win=glrGetWindow(Renderer);
int value;

XtVaGetValues ( w , XmNvalue, &value, NULL );

GetStatus (cbs->reason);

state->zoom = (double) value/DIVVALUE;

glrSetDialog(Renderer,0);
glwProcess(win,Renderer);
glrSetDialog(Renderer,1);
}

/******************************************************/


void sliderLightCB ( Widget w, XtPointer client_data, XtPointer call_data )
{
XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct*) call_data;
int i = (int) client_data;
glRendererState state = glrGetRendererState(Renderer);
glWindow win=glrGetWindow(Renderer);
int value;

XtVaGetValues ( w , XmNvalue, &value, NULL );

GetStatus (cbs->reason);

switch (i) {
case LIGHTX: if (DEBUG) printf("\nLight X\n");
  state->ltx = (double) value;
  break;
case LIGHTY: if (DEBUG) printf("\nLight Y\n");
  state->lty = (double) value;
  break;
}

state->what_done=GLR_LIGHT;

glrSetDialog(Renderer,0);
glwProcess(win,Renderer);
glrSetDialog(Renderer,1);
}


/******************************************************/

void sliderCameraCB ( Widget w, XtPointer client_data, XtPointer call_data )
{
XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct*) call_data;
int i = (int) client_data;

glRendererState state = glrGetRendererState(Renderer);
glWindow win=glrGetWindow(Renderer);
int value;

XtVaGetValues ( w , XmNvalue, &value, NULL );

GetStatus (cbs->reason);

switch (i) {
case CAMX:
case CAMY:
case CAMZ:
  break;
}

glrSetDialog(Renderer,0);
glwProcess(win,Renderer);
glrSetDialog(Renderer,1);
}


/******************************************************/



void sliderCuttingplaneCB ( Widget w, XtPointer client_data, XtPointer call_data )
{  
XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct*) call_data;
all_client_data *stub = (all_client_data*) client_data;
glRendererState state = glrGetRendererState(Renderer);
glWindow win=glrGetWindow(Renderer);
double slidervalue;

GetStatus (cbs->reason);

if (DEBUG) printf("\nCuttingplane Factor D Change!\n");

slidervalue = (double) cbs->value;

switch (state-> plane_dir) {
	case 'x' : {
		   if (DEBUG) printf("\nChange X Plane!\n");
		   state->plane_d = -(slidervalue/SLIDER_MAX *
				    (state->xmax - state->xmin)+
				    state->xmin);
		   }
		   break;




	case 'y' : {
		   if (DEBUG) printf("\nChange Y Plane!\n");
		   state->plane_d = -(slidervalue/SLIDER_MAX *
				    (state->ymax - state->ymin)+
				    state->ymin);
		   }
		   break;



	case 'z' : {
		   if (DEBUG) printf("\nChange Z Plane!\n");
		   state->plane_d = -(slidervalue/SLIDER_MAX *
				    (state->zmax - state->zmin)+
				    state->zmin);
		   }
		   break;
	}



glrSetDataValid ( Renderer , 0 );
state->what_done=GLR_PLANE;

glrSetDialog(Renderer,0);
glwProcess(win,Renderer);
glrSetDialog(Renderer,1);
}

/******************************************************/


void sliderIsodistanceCB ( Widget w, XtPointer client_data, XtPointer call_data)
{
XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct*) call_data;
glRendererState state = glrGetRendererState(Renderer);
glWindow win=glrGetWindow(Renderer);

GetStatus (cbs->reason);

state->isoline_distance = (double) cbs->value / DIVVALUE;
				   
glrSetDataValid ( Renderer , 0 );

state->what_done=GLR_ILDIST;

glrSetDialog(Renderer,0);
glwProcess(win,Renderer);
glrSetDialog(Renderer,1);
}


/******************************************************/


void sliderIsolevelCB ( Widget w, XtPointer client_data, XtPointer call_data)
{
all_client_data *stub = (all_client_data*) client_data;
XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct*) call_data;
glRendererState state = glrGetRendererState(Renderer);
glWindow win=glrGetWindow(Renderer);
double slidervalue;

slidervalue = (double) cbs->value;

GetStatus (cbs->reason);

state->level =  (slidervalue / SLIDER_MAX *
		(state->fmax - state->fmin) +
		state->fmin);

glrSetDataValid ( Renderer , 0 );
state->what_done=GLR_ISOLEVEL;

glrSetDialog(Renderer,0);
glwProcess(win,Renderer);
glrSetDialog(Renderer,1);
}


/******************************************************/

void sliderRelationCB ( Widget w, XtPointer client_data, XtPointer call_data)
{
XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct*) call_data;
glRendererState state = glrGetRendererState(Renderer);
glWindow win=glrGetWindow(Renderer);

if (DEBUG) printf("\nsliderRelationCB\n");

/*GetStatus (cbs->reason);*/

if streq(w,"Aspect XY-Axis")  /*Slider XYAxis*/
	state->xyasp = (double) cbs->value/100;
else if streq(w,"Aspect Z-Axis") /*Slider ZAxis*/
	  state->zasp = (double) cbs->value/100;

glrSetDialog(Renderer,0);
glwProcess(win,Renderer);
glrSetDialog(Renderer,1);

}

/******************************************************/

void exitCB ( Widget w, XtPointer client_data, XtPointer call_data)
{
Widget appShell = GetApplShell(w);

XtDestroyWidget(appShell);
exit (0);
}

/******************************************************/

void deleteCB (Widget w,XtPointer client_data, XtPointer call_data )
{
Widget box;
if (DEBUG) 
  printf ( "\ndeleteCB: Callback.\n");
box=CreateQuestionDialog ( w, "Do you want really exit ?");
Callback(box, XmNokCallback, exitCB, NULL);
}

/******************************************************/
/*
 * $Log: glrgui.c,v $
 * Revision 1.8  1998/05/18 18:52:59  fuhrmann
 * new glwgrab
 *
 * Revision 1.7  1997/06/18  09:49:20  fuhrmann
 * window mode in suspend
 *
 * Revision 1.6  1997/06/05  17:39:50  fuhrmann
 * Ch. Steinborn, 5.6., slider scales
 *
 * Revision 1.3  1997/04/25  16:02:33  fuhrmann
 * Steinborn 24.4.
 *
 * Revision 1.1  1997/04/21  09:41:20  fuhrmann
 * Initial revision
 *	
 */
