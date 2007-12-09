/**********************************************************************
 * demo.c - Demo the drawing area widget
 *
 * Copyright 1993, David Nedde
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/

#include <stdio.h>

#include <X11/StringDefs.h>
#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>
#include "X11/Xaw_d/DrawingA.h"
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Command.h>

GC gc = 0;

/*** Start of main program ***/
main(argc, argv)
int    argc;
char **argv;
{
    XtAppContext app_context;
    Widget   toplevel;          /* The core widget */

    Arg      wargs[20];         /* Used to set widget resources */
    int      n;

    toplevel = XtAppInitialize(&app_context,"Demo", 
                               NULL, 0, 
                               (Cardinal *)&argc, argv,     
                               NULL,
                               /*args*/(ArgList)NULL, /*num_args*/(Cardinal)0);

    setup_widgets( toplevel);

    XtRealizeWidget(toplevel);

    XtAppMainLoop(app_context);
}


static void redisplay_cb(w, closure, call_data)
Widget                         w;
XtPointer                      closure;
XawDrawingAreaCallbackStruct * call_data;
{
    int x = 10;
    int y = 10;
    int width = 30;
    int height = 30;

    if (gc == 0)
    {
        gc = XCreateGC(XtDisplay(w), XtWindow(w), 0, 0);
        XSetForeground(XtDisplay(w), gc, 
                       BlackPixel(XtDisplay(w),      
                                  DefaultScreen(XtDisplay(w))));
    }

    XDrawArc(XtDisplay(w), XtWindow(w), gc, x, y, width, height, 0, 360*64);
    XFillArc(XtDisplay(w), XtWindow(w), gc, x, y, width, height, 0, 360*64);
}


static void quit_cb(w, closure, call_data)
Widget    w;
XtPointer closure;
XtPointer call_data;
{
    exit(0);
}



setup_widgets( toplevel )
Widget toplevel;
{
  Widget button;
  Widget form_widget =
      XtCreateManagedWidget("form", formWidgetClass, toplevel, NULL,0);

 
  Widget canvas = XtVaCreateManagedWidget("canvas", drawingAreaWidgetClass, 
                                          form_widget, 
                                          XtNwidth, 200,
                                          XtNheight, 200,
                                          NULL);

  XtAddCallback(canvas,XtNexposeCallback,redisplay_cb, NULL);


  button = XtVaCreateManagedWidget("Quit", commandWidgetClass, 
                                   form_widget, 
                                   XtNfromVert,   canvas,
                                   NULL);
  
  XtAddCallback(button,XtNcallback,quit_cb, NULL);
}
