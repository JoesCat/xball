/**********************************************************************
 * room.c - Room object definition
 *
 * Copyright 1993, David Nedde
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/

/* System Headers */
#include <stdio.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/cursorfont.h>

#ifdef MOTIF
#include <Xm/DrawingA.h>
#endif

#ifdef ATHENA
#include <X11/Xaw_d/DrawingA.h>
#endif

/* Local headers */
#include "room.h"
#include "misc.h"
#include "miscx.h"
#include "scrollbar.h"

/* Macros */
/* This is the spacing between the balls representing pixels on the bitmap */
#define SF 1.5  /* FIX - make a resource */

/* Functions */
static void redisplay_cb(/*w, room, call_data*/);
static void resize_cb(/*w, datap_ptr, call_data*/);
static void process_input_cb(/*w, room, call_data*/);
static void wait_for_resize(/* room*/);
static Widget create_canvas(/* parent, name*/);


/* Object public methods */

/* Create the room object */
room_type room__create( form_w, items, file_sel)
Widget        form_w;
items_type    items;
file_sel_type file_sel;
{
    Dimension right_wall;
    Dimension floor;


    Widget canvas;

    room_type room = XtNew( room_struct_type);

    room->item       = (item_type)0;
    room->items      = items;
    room->file_sel   = file_sel;
    room->left_wall  = int2intf(0);
    room->ceiling    = int2intf(0);
    /* room->gravity is set in xball_sys via scrollbar callback */

    room->button1 = False;
    room->button2 = False;
    room->button3 = False;


    room->canvas = canvas = create_canvas( room, form_w, "canvas");

    XtVaGetValues(canvas, 
		  XtNbackground,  &room->background, 
		  XtNwidth,	  &right_wall,
		  XtNheight, 	  &floor,
		  NULL);

    room->right_wall = int2intf( right_wall);
    room->floor      = int2intf( floor);

    return room;
}


/* Destroy the room object */
void room__destroy( room)
room_type room;
{
    XtDestroyWidget( room->canvas);
    XtFree( (char *)room);
}


/* Called when the scrollbar changes.  Sets the gravity value */
void room__set_gravity_cb(w, room, call_data)
Widget                    w;
room_type                 room;
scrollbarCallbackStruct * call_data;
{
    room->gravity = call_data->value;
}


/* A menu callback that clears the room of all balls */
void room__clear_mcb(w, room, call_data, extra)
Widget    w;
room_type room;
caddr_t   call_data;
char *    extra;
{
    items__destroy_all( room->items);
}


/* A menu callback that randomizes all balls in the room */
void room__randomize_mcb(w, room, call_data, extra)
Widget    w;
room_type room;
caddr_t   call_data;
char *    extra;
{
    item_type item;


    for (item = items__get_first( room->items);
         item != (item_type)0;
         item = items__get_next( room->items))
    {
        item__randomize( item, -16, 16, -16, 16);
    }
}


/* A menu callback that queries the user for a bitmap and then loads it */
void room__load_bitmap_mcb(w, room, call_data, extra)
Widget    w;
room_type room;
caddr_t   call_data;
char *    extra;
{
    char *file_name = file_sel__display( room->file_sel, "Load Bitmap");

    if (file_name != (char *)0)
        room__load_bitmap( room, file_name, 10, 10);
}


/* Loads the specified bitmap file at the specified position in the room */
void room__load_bitmap(room, filename, pos_x, pos_y)
room_type room;
char *    filename;
int       pos_x;
int       pos_y;
{
    unsigned  width, height;  /* Bitmap dimensions */
    int       x_hot, y_hot;
    Pixmap    bitmap;         /* The bitmap as a pixmap */
    XImage *  ximage;         /* The bitmap as an image */
    int       x, y;
    int       status;
    Dimension new_width, new_height;


    status = XReadBitmapFile(XtDisplay(room->canvas), XtWindow(room->canvas),
                             filename, &width, &height, &bitmap, 
                             &x_hot, &y_hot);
    switch (status)
    {
        case BitmapOpenFailed: 
        fprintf(stderr,
                "room__load_bitmap: Could not open bitmap file %s\n",filename);
        return;
        break;

        case BitmapFileInvalid: 
        fprintf(stderr,
             "room__load_bitmap: Bitmap file %s does not contain valid data\n",
             filename);
        return;
        break;

        case BitmapNoMemory: 
        fprintf(stderr,
              "room__load_bitmap: Not enough memory to open bitmap file %s\n",
              filename);
        return;
        break;
    }

    /* Get a local copy of the pixmap so we can query the pixel values */
    ximage = XGetImage(XtDisplay(room->canvas),
                       bitmap,0,0,width,height,AllPlanes,XYPixmap);
    if (ximage == NULL)
    {
        fprintf(stderr,"room__load_bitmap: Problem getting bitmap image\n");
        return;
    }

    new_width  = MAX( intf2int(room->right_wall), 
                      pos_x + width  * item__get_width()  * SF+10);
    new_height = MAX( intf2int(room->floor),      
                      pos_y + height * item__get_height() * SF+10);

    if (new_width  != intf2int(room->right_wall) ||
        new_height != intf2int(room->floor))
    {
        room->resized = False;

        /* Set the window width and height so the balls fit on it */
        XtVaSetValues(room->canvas,
                      XtNwidth,  new_width,
                      XtNheight, new_height,
                      NULL);

        wait_for_resize( room);
    }

    /* Create a ball for each pixel set in the bitmap */
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            if (XGetPixel(ximage, x, y))
                items__new_item(room->items, 
                                (int)(x * item__get_width()  * SF + pos_x),
                                (int)(y * item__get_height() * SF + pos_y),
                                /*x_vel*/0, /*y_vel*/0);
        }
    }
}



/* Set the room's width and wait for the window to resize before returning */
void room__set_width(room, width)
room_type room;
int       width;
{
    Dimension d_width = width;


    if (intf2int(room->right_wall) == width)
        return;

    room->resized = False;

    XtVaSetValues(room->canvas,
                  XtNwidth,       d_width,
                  NULL);

    /* room->right_wall = int2intf(width); Set in window resize callback */

    wait_for_resize( room);
}


/* Set the height of the room, resize the actual room and wait for it
   to resize before returning. */
void room__set_height(room, height)
room_type room;
Dimension height;
{
    Dimension d_height = height;


    if (intf2int(room->floor) == height)
        return;

    room->resized = False;

    XtVaSetValues(room->canvas,
                  XtNheight,       d_height,
                  NULL);

    /* room->floor = int2intf(height); set in the window resize callback */

    wait_for_resize( room);
}


/* Private object methods */


/**********************************************************************
 * redisplay - Called when DrawingArea canvas gets an expose event
 *             Redraws all balls in room
 **********************************************************************/
static void redisplay(room, event)
room_type room;
XEvent *  event;
{
    Display * display = XtDisplay( room->canvas);
    Window    window  = XtWindow ( room->canvas);


    if (!item__initted())
    {
        Cursor cursor = XCreateFontCursor( display, XC_watch );
        XDefineCursor( display, window, cursor);

        item__init( get_toplevel( room->canvas), display, window, 
		    room->background);

	XFreeCursor( display, cursor);
	XUndefineCursor( display, window);
    }


    if (event->xexpose.count == 0) /* Wait until last expose event*/
    {
        item_type item;

        XClearWindow( display, window);

        for (item = items__get_first( room->items);
             item != (item_type)0;
             item = items__get_next( room->items))
        {
            item__redraw( item);
        }
    }
}


/**********************************************************************
 * resize_cb - Called when DrawingArea canvas is resized
 *             Adjusts the right wall and floor
 **********************************************************************/
static void resize_cb(w, room, call_data)
Widget    w;
room_type room;
caddr_t   call_data;
{
    Dimension     width,height;

    XtVaGetValues(w,
                  XtNwidth,       &width, 
                  XtNheight,      &height,        
                  NULL);

    room->right_wall = int2intf(width);
    room->floor      = int2intf(height);

    /* Set flag in case someone is waiting for the resize */
    room->resized = True;
}
 

/* Waits for the window to actually resize.  This depends on the
   resize callback code setting the room->resized flag to True.  Callers
   should set the room->resized flag to False before calling.  */
static void wait_for_resize( room)
room_type room;
{
    XEvent        event;
    XtAppContext  app_context;


    app_context = XtWidgetToApplicationContext( room->canvas);

    /* Wait for the window resize to occur */
    while (!room->resized)
    {
        XtAppNextEvent(app_context, &event);
        XtDispatchEvent(&event);
    }
}


/**********************************************************************
 * process_input - Called when DrawingArea canvas has input
 *                 Handles mouse button events
 **********************************************************************/
static void process_input( room, event)
room_type room;
XEvent *  event;
{
    switch (event->type)
    {
        /* Process mouse button events */
        case ButtonPress:
        switch (event->xbutton.button) 
        {
            case 1:
            /* Button 1 - Create 1 item under the pointer */

            room->oldx = room->newx = event->xbutton.x;
            room->oldy = room->newy = event->xbutton.y;

            /* Create an item under the pointer */
            room->item = item__create(event->xbutton.x, 
                                      event->xbutton.y, 
                                      /*x,y velocity*/ 0, 0);

            room->button1 = True;
            break;

            /* Remember the button we have pressed (used in mouse motion) */
            case 2:  room->button2 = True; break;
            case 3:  room->button3 = True; break;
        }
        break;

        case ButtonRelease:
        switch (event->xbutton.button) 
        {
            /* button 1 release - move item to items list */
            case 1: 
            item__set_x_vel( room->item, room->newx - room->oldx);
            item__set_y_vel( room->item, room->newy - room->oldy);

            items__add( room->items, room->item);
            room->item = (item_type)0;
                               
            room->button1 = False;
            break;

            /* Remember the button we have unpressed (used in mouse motion) */
            case 2:  room->button2 = False; break;
            case 3:  room->button3 = False; break;
        }       
    }
}


/**********************************************************************
 * process_motion - Called when DrawingArea canvas have pointer motion events
 **********************************************************************/
static  void process_motion( room, event)
room_type room;
XEvent *  event;
{
    if (room->button1)
    {
        /* When button 1 pressed during movement */
        /* Save 'speed' of pointer for when the button is released */
        room->oldx = room->newx;  
        room->oldy = room->newy;
        room->newx = event->xmotion.x;
        room->newy = event->xmotion.y;

        item__set_x_vel( room->item, (room->newx - room->oldx));
        item__set_y_vel( room->item, (room->newy - room->oldy));

        item__move_pos( room->item, 
                        event->xmotion.x, event->xmotion.y);
    } 
    else 
    if (room->button2)
    {
        /* Create a random velocity ball */
        items__new_item(room->items, event->xmotion.x, event->xmotion.y,
                        rand_range( -8, 8),
                        rand_range( -8, 0));
    }
    else
    if (room->button3)
    {
        items__new_item(room->items, event->xmotion.x, event->xmotion.y,
                        0, 0);
    }
}


#ifdef MOTIF

static void process_motion_act(/*w, datap_ptr, event*/);


/* Create motif canvas as a DrawingArea Widget */
static Widget create_canvas( room, parent, name)
room_type room;
Widget    parent;
char *    name;
{
    Widget canvas =
        XtCreateManagedWidget("canvas", xmDrawingAreaWidgetClass, parent, 
                              NULL, 0);

    XtAddCallback(canvas, XmNexposeCallback, redisplay_cb,    (XtPointer)room);
    XtAddCallback(canvas, XmNinputCallback,  process_input_cb,(XtPointer)room);
    XtAddCallback(canvas, XmNresizeCallback, resize_cb,       (XtPointer)room);
    XtAddEventHandler(canvas, ButtonMotionMask, FALSE, 
                      process_motion_act, (XtPointer)room);

    return canvas;
}


static void redisplay_cb(w, room, call_data)
Widget                         w;
room_type                      room;
XmDrawingAreaCallbackStruct *  call_data;
{
    redisplay( room, call_data->event);
}


static void process_input_cb(w, room, call_data)
Widget                        w;
room_type                     room;
XmDrawingAreaCallbackStruct * call_data;
{
    process_input( room, call_data->event);
}


static void process_motion_act(w, room, event)
Widget    w;
room_type room;
XEvent *  event;
{
    process_motion( room, event);
}

#endif


#ifdef ATHENA


static void process_motion_cb(/*w, room, call_data*/);


/* Create canvas as a DrawingArea widget */
static Widget create_canvas( room, parent, name)
room_type room;
Widget    parent;
char *    name;
{
    Widget canvas =
        XtCreateManagedWidget("canvas", drawingAreaWidgetClass, parent, 
                              NULL, 0);

    XtAddCallback(canvas,XtNexposeCallback,redisplay_cb,      (XtPointer)room);
    XtAddCallback(canvas,XtNinputCallback, process_input_cb,  (XtPointer)room);
    XtAddCallback(canvas,XtNresizeCallback,resize_cb,         (XtPointer)room);
    XtAddCallback(canvas,XtNmotionCallback,process_motion_cb, (XtPointer)room);

    return canvas;
}


static void redisplay_cb(w, room, call_data)
Widget                         w;
room_type                      room;
XawDrawingAreaCallbackStruct * call_data;
{
    redisplay( room, call_data->event);
}


static void process_input_cb(w, room, call_data)
Widget                         w;
room_type                      room;
XawDrawingAreaCallbackStruct * call_data;
{
    process_input( room, call_data->event);
}


static void process_motion_cb(w, room, call_data)
Widget                         w;
room_type	 	       room;
XawDrawingAreaCallbackStruct * call_data;
{
    process_motion( room, call_data->event);
}


#endif

