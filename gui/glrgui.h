/*
 * $Id: glrgui.h,v 1.6 1998/05/18 18:52:59 fuhrmann Exp $
 */
#include <Xm/XmAll.h>
#define NUMOFDPYLISTS 9

/* TYPES FOR GUI */

#define DEBUG 0

#define SLIDER_MAX 65535

#define ROTMAX 360
#define ROTMIN -360
#define DIVVALUE 100

/* containers */
#define DISPLAYLISTS 0
#define ROTATION 1
#define TRANSLATION 2
#define ZOOM 3
#define LIGHT 4
#define CAMERA 5
#define CUTTINGPLANE 6
#define ISODISTANCE 7
#define ISOLEVEL 8
#define RELATION 9

/* filepulldown */
#define LOAD 0
#define SAVE 1
#define SAVEAS 2
#define SAVEPICTURE 3
#define SAVEPICTUREAS 4
#define PRINT 5
#define QUIT 6

/* actionpulldown */
#define ROTATE 0
#define TRANSLATE 1
#define MOVELIGHT 2
#define REDRAW 3

/* settingspulldown */
#define LEVELSURFACE 0
#define ISOLINES 1
#define BACKGROUND 2
#define ORTHOGRAPHIC 3
#define GOURAUD 4
#define IMMEDIATE 5
#define INFO 6

/* helppulldown */
#define RENDER 0
#define GUI 1
#define HELP 2

/* planes radiobox */
#define PLANE_X 0
#define PLANE_Y 1
#define PLANE_Z 2

/*slider rotation */
#define ROTX 0
#define ROTY 1
#define ROTZ 2

/* slider translation */
#define TRANX 0
#define TRANY 1

/* slider light */
#define LIGHTX 0
#define LIGHTY 1

/* slider aspect */
#define ASPECTXY 0
#define ASPECTZ 1

/* slider camera */
#define CAMX 0
#define CAMY 1
#define CAMZ 2

/* radio for displaylists */
#define LISTONE 0
#define LISTTWO 1
#define LISTTHREE 2
#define LISTFOUR 3
#define LISTFIVE 4
#define LISTSIX 5
#define LISTSEVEN 6
#define LISTEIGHT 7
#define LISTNINE 8

/************************************************************************/

/* COMMAND.H */


char *fileStrings[] = { 	"Load state",
				"Save state",
				"Save state as",
				"Save picture",
				"Save picture as",
				"Print",
				"Quit" };


/** char *actionStrings[] = {	"Rotate",
				"Translate",
				"Move light source",
				"Refresh window" }; **/


char *helpStrings[] = {	"About GLTools",
			"About glrGUI", 
			"Help"};


char *settingStrings[] = {    "Isosurface",
			      "Isolines" ,
			      "Black",
			      "Orthogonal",
			      "Gouraud",
			      "Immediate Mode",
			      "Information" };

char *extensionStrings[] = { "Extension One",
			"Extension Two",
			"Extension Three" };
			       
char *planesStrings[] = {     "X-Plane",
			      "Y-Plane",
			      "Z-Plane" };

char *sliderRotStrings[] = {	"Rotation X",
				"Rotation Y",
				"Rotation Z" };

char *sliderTranStrings[] = {	"Translation X",
				"Translation Y" };


char *sliderLightStrings[] = { 	"Lightsource X",
				"Lightsource Y" };

char *sliderCameraStrings[] = {	"Camera X rotation",
				"Camera Y rotation",
				"Camera Z rotation" };

char *sliderRelationStrings[] = { "Aspect XY-Axis",
			          "Aspect Z-Axis" };

char *panelStrings[] = {	"Objects",
				"Rotation",
				"Translation",
				"Zoom",
				"Light",
				"Camera",
				"Plane",
				"Isolines",
				"Isosurface",
				"Relation" };

char *dpylistStrings[] = {"1",
			  "2",
			  "3",
			  "4",
			  "5",
			  "6",
			  "7",
			  "8",
			  "9" };

int immediateMode=FALSE;

char *sliderCuttingplaneString = {"Plane Section" };
char *sliderZoomString =	{"Zoom"};
char *sliderIsolevelString = { "Isosurface Level" };
char *sliderIsodistanceString = { "Isoline Distance" };

typedef struct {
  Widget toggleSettingsPD[XtNumber(settingStrings)];
  Widget toggleExtensionsPD[XtNumber(extensionStrings)];
  Widget showPlanesRadio[XtNumber(planesStrings)];
  Widget panelRadio[XtNumber(panelStrings)];
  Widget dpyVertContainer[XtNumber(dpylistStrings)];
  Widget dpylistLabel[XtNumber(dpylistStrings)];
  Widget showDpylistToggle[XtNumber(dpylistStrings)];
  Widget storeDpylistButton[XtNumber(dpylistStrings)];
  int dpylistActive[XtNumber(dpylistStrings)];
  Widget sliderRot[XtNumber(sliderRotStrings)];
  Widget sliderTran[XtNumber(sliderTranStrings)];
  Widget sliderZoom;
  Widget sliderIsodistance;
  Widget sliderIsolevel;
  Widget sliderLight[XtNumber(sliderLightStrings)];
  Widget sliderCamera[XtNumber(sliderCameraStrings)];
  Widget sliderCuttingplane;
  Widget sliderRelation[XtNumber(sliderRelationStrings)];
  Widget refreshButton;
  Widget glButton;

 } all_client_data;

/***********************************************************************/


void gui ( glRenderer rnd );

void setAllCB ( Widget w, XtPointer client_data, XtPointer call_data);

void InitializeWidgets ( all_client_data *clientdata );

void CreateCommandWindow(Widget appShell, glRenderer rnd );

void CreateColorWindow ( Widget appShell, glRenderer rnd );

void pdFileCB (Widget w, XtPointer client_data, XtPointer call_data);

void pdSettingCB (Widget w, XtPointer client_data, XtPointer call_data);

void pdPanelCB (Widget w, XtPointer client_data, XtPointer call_data);

/*void pdActionCB (Widget w, XtPointer client_data, XtPointer call_data);*/

void pdExtensionCB ( Widget w, XtPointer client_data, XtPointer call_data);

void pdHelpCB (Widget w, XtPointer client_data, XtPointer call_data);

void refreshCB (Widget w, XtPointer client_data, XtPointer call_data); 

void glCB (Widget w, XtPointer client_data, XtPointer call_data);

void zoomCB (Widget w, XtPointer client_data, XtPointer call_data);

void showPlanesCB (Widget w, XtPointer client_data, XtPointer call_data);

void exitCB ( Widget w, XtPointer client_data, XtPointer call_data );

void dpyCB ( Widget w, XtPointer client_data, XtPointer call_data );

void showDpylistCB  ( Widget w, XtPointer client_data, XtPointer call_data );

void deleteDpyCB  ( Widget w, XtPointer client_data, XtPointer call_data );

void storeDpylistCB ( Widget w, XtPointer client_data, XtPointer call_data );

void getActiveDisplayListsCB ( Widget w, XtPointer client_data, XtPointer call_data );

void GetStatus ( int value );

void sliderRotCB ( Widget w, XtPointer client_data, XtPointer call_data );

void sliderTranCB ( Widget w, XtPointer client_data, XtPointer call_data );

void sliderZoomCB ( Widget w, XtPointer client_data, XtPointer call_data );

void sliderLightCB ( Widget w, XtPointer client_data, XtPointer call_data );

void sliderCameraCB ( Widget w, XtPointer client_data, XtPointer call_data );

void sliderCuttingplaneCB ( Widget w, XtPointer client_data, XtPointer call_data );

void sliderIsodistanceCB ( Widget w, XtPointer client_data, XtPointer call_data );

void sliderIsolevelCB ( Widget w, XtPointer client_data, XtPointer call_data );

void sliderRelationCB ( Widget w, XtPointer client_data, XtPointer call_data );

void exitCB ( Widget w, XtPointer client_data, XtPointer call_data );

void deleteCB ( Widget w, XtPointer client_data, XtPointer call_data);

Widget XInfo ( Widget parent, char* string);

Widget XWarning ( Widget parent, char *string );

Widget XQuestion ( Widget parent, char *string );

Widget CreateFileBox ( Widget parent );

void GetFilenameCB ( Widget w, XtPointer client_data, XtPointer call_data);

Widget ShowTextFile ( Widget app , char *filename );

/***********************************************************************/
/*
 * $Log: glrgui.h,v $
 * Revision 1.6  1998/05/18 18:52:59  fuhrmann
 * new glwgrab
 *
 * Revision 1.5  1997/06/05  17:39:50  fuhrmann
 * Ch. Steinborn, 5.6., slider scales
 *
 * Revision 1.3  1997/04/25  16:02:51  fuhrmann
 * Steinborn 24.4.
 *
 * Revision 1.1  1997/04/21  09:41:24  fuhrmann
 * Initial revision
 *	
 */
