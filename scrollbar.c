/**********************************************************************
 * scrollbar.c
 *
 * Copyright 1993, David Nedde
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/

/* System Headers */
#ifdef MOTIF
#include <Xm/Scale.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#endif

#ifdef ATHENA
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Scrollbar.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Text.h>
#include <X11/Xaw/Form.h>
#endif

/* Local headers */
#include "scrollbar.h"
#include "misc.h"

/* Structures */
/* A node in the scrollbar callbacks list */
typedef struct {
    scrollbarCallbackProc proc;
    caddr_t               closure;
} callback_node;

/* Functions */
void create_widgets(/* scrollbar, parent, name*/);
static void call_callbacks(/* scrollbar, value*/);
static int scale_set_value(/*scrollbar, value */);


/* Public object methods */

/* Creates a scrollbar object.  Consists of a label, a scrollbar and a
   label giving feedback to the user */
scrollbar_type scrollbar__create( parent, name)
Widget   parent;
char *   name;
{
    scrollbar_type scrollbar;


    scrollbar = XtNew( scrollbar_struct);

    /* We need to have our own callback list, because callbacks from
       Motif and Athea are so different */
    scrollbar->callbacks = list__create();

    create_widgets( scrollbar, parent, name);

    return scrollbar;
}


/* Destroy the scrollbar object */
void scrollbar__destroy( scrollbar)
scrollbar_type scrollbar;
{
    int i;


    for (i = 0; i < list__get_count( scrollbar->callbacks); i++)
    {
        free( list__get_cell( scrollbar->callbacks, i));
    }
    list__destroy( scrollbar->callbacks);

    XtDestroyWidget( scrollbar->scale);
    XtDestroyWidget( scrollbar->label);
    XtFree( (char *)scrollbar);
}


/* Set the value of the scrollbar and make any callbacks */
void scrollbar__set_value( scrollbar, value)
scrollbar_type scrollbar;
int value;
{
    scrollbar->value = value;

    scale_set_value( scrollbar, value);

    call_callbacks( scrollbar, value);
}


/* Add a scrollbar callback.  When the scrollbar value changes, the
   proc will be called back with a scrollbarCallbackStruct. */
void scrollbar__add_callback( scrollbar, proc, closure)
scrollbar_type scrollbar;
XtCallbackProc proc;
XtPointer      closure;
{
    callback_node *node_ptr = (callback_node *)malloc( sizeof( callback_node));
    

    node_ptr->proc    = proc;
    node_ptr->closure = closure;

    list__add_end( scrollbar->callbacks, node_ptr);
}



/* Private object methods */

/* Make the scrollbar callbacks */
static void call_callbacks( scrollbar, value)
scrollbar_type scrollbar;
int            value;
{
    int                     i;
    callback_node *         node_ptr;
    scrollbarCallbackStruct scrollbar_call_data;


    scrollbar_call_data.reason = 0;
    scrollbar_call_data.event  = 0;
    scrollbar_call_data.pixel  = 0;
    scrollbar_call_data.value  = value;

    for (i = 0; i < list__get_count( scrollbar->callbacks); i++)
    {
        node_ptr = (callback_node *)list__get_cell( scrollbar->callbacks, i);
        (*node_ptr->proc)( scrollbar->scale, node_ptr->closure, 
                           &scrollbar_call_data);
    }
}


#ifdef MOTIF

/* Called when the motif scrollbar moves.  Makes all registered
   scrollbar callbacks */
static void scrollbar_callback( w, scrollbar, call_data)
Widget         w;
scrollbar_type scrollbar;
caddr_t        call_data;
{
    call_callbacks( scrollbar, 
		    ((XmScaleCallbackStruct *)call_data)->value);
}


#ifdef NOTUSED

/* Returns the scrollbar widget's value */
static int scrollbar_get_value( scrollbar)
scrollbar_type scrollbar;
{
    int value;

    XmScaleGetValue( scrollbar->scale, &value);

    return value;
}

#endif


/* Sets the scrollbar widget's value */
static int scale_set_value( scrollbar, value)
scrollbar_type scrollbar;
int value;
{
    XmScaleSetValue( scrollbar->scale, value);
}


/* Creates the scrollbar widgets */
void create_widgets( scrollbar, parent, name)
scrollbar_type scrollbar;
Widget         parent;
char *         name;
{
    scrollbar->form =
        XtVaCreateManagedWidget(name, xmFormWidgetClass, parent,
                                NULL);
        
    scrollbar->label = 
        XtVaCreateManagedWidget("label", xmLabelWidgetClass, scrollbar->form,
                                NULL);
    scrollbar->scale = 
        XtVaCreateManagedWidget("bar", xmScaleWidgetClass, scrollbar->form, 
                                XmNleftAttachment, XmATTACH_WIDGET,
                                XmNleftWidget,     scrollbar->label,
                                NULL);

    XmScaleGetValue( scrollbar->scale, &scrollbar->value);

    XtAddCallback( scrollbar->scale, XmNvalueChangedCallback, 
                   scrollbar_callback, (XtPointer)scrollbar);
    XtAddCallback( scrollbar->scale, XmNdragCallback, 
                   scrollbar_callback, (XtPointer)scrollbar);
}

#endif


#ifdef ATHENA

/* Functions */
static void set_feedback_value(/* scrollbar, value*/);
static void move_thumb(/* scrollbar, thumb, shown*/);
static void set_label_value(/* label_w, value*/);


/* Macros */
#define SB_MAX 100.0
#define SB_MIN   0.0

#define thumb_to_value( thumb, shown) \
            ((int)((thumb) / (1-(shown))* (SB_MAX - SB_MIN) + 0.5 - SB_MIN))
#define value_to_thumb( value, shown) \
            ((value)*(1-(shown) + SB_MIN) / (SB_MAX - SB_MIN))


/* Called when the athena scrollbar is scrolled incrementally.
   Updates the feedback value and makes all registered callbacks. */
static void scrollbar_scroll_callback( w, scrollbar, position_ptr)
Widget         w;
scrollbar_type scrollbar;
XtPointer      position_ptr;
{
    Dimension     length;
    float         thumb;
    float         shown;
    int           position = (int)position_ptr;
    float         change; /* 0.0 .. 1.0 - percentage of full-page change 
                             we should do */


    /* Position is the number of pixels from left of scrollbar pointer is.
       Position > 0 indicates button 1, position < 0 indicates button 3 */


    XtVaGetValues( scrollbar->scale, 
                   XtNlength,     &length,
                   XtNtopOfThumb, &thumb,
                   XtNshown,      &shown,
                   NULL);

    change = (float)position / length;

    /* Scroll by the amount of thumb shown * amount of change we
       should for the given pointer position */
    thumb += shown * change;

    move_thumb( scrollbar, thumb, shown);
}


/* Called when the athena scrollbar is moved in a jumpy fashion.
   Updates the feedback value and makes all registered callbacks. */
static void scrollbar_jump_callback( w, scrollbar, percent_ptr)
Widget         w;
scrollbar_type scrollbar;
float *        percent_ptr;
{
    float shown;
    float thumb = *percent_ptr;

    XtVaGetValues( scrollbar->scale, 
                   XtNshown,      &shown,
                   NULL);

    move_thumb( scrollbar, thumb, shown);
}


/* Moves thumb within valid values and updates the feedback value */
static void move_thumb( scrollbar, thumb, shown)
scrollbar_type scrollbar;
double         thumb;
double         shown;
{
    int value;


    /* Keep the value within the proper range */
    if (thumb >= 1 - shown)
        /* Keep below 1, otherwise thumb stops redrawing due to Athena bug */
        thumb = 1 - shown;
    else
    if (thumb < 0)
        thumb = 0;

    /* Set the thumb to the new position */
    XawScrollbarSetThumb( scrollbar->scale, thumb, -1.0);

    value = thumb_to_value( thumb, shown);

    set_feedback_value( scrollbar, value);

    call_callbacks( scrollbar, value);
}


/* Returns the value of the scrollbar */
static int scrollbar_get_value( scrollbar)
scrollbar_type scrollbar;
{
    float thumb;
    float shown;


    XtVaGetValues( scrollbar->scale,
                   XtNtopOfThumb, &thumb,
                   XtNshown,      &shown,
                   NULL);

    return thumb_to_value( thumb, shown);
}


/* Sets the value of the scrollbar - updates thumb and feedback value */
static int scale_set_value( scrollbar, value)
scrollbar_type scrollbar;
int value;
{
    float shown;


    XtVaGetValues( scrollbar->scale, XtNshown, &shown, NULL);

    /* Set the thumb to the new position */
    XawScrollbarSetThumb( scrollbar->scale, 
                          value_to_thumb( value, shown), -1.0);
             

    set_feedback_value( scrollbar, value);
}


/* Updates the feedback label to the passed value */
static void set_feedback_value( scrollbar, value)
scrollbar_type scrollbar;
int            value;
{
    set_label_value( scrollbar->feedback, value);
}


/* Sets an athena label so it displays the passed value */
static void set_label_value( label_w, value)
Widget label_w;
int    value;
{
    char buff[15];


    sprintf(buff,"%d", value);


    XtVaSetValues( label_w, XtNlabel, buff, NULL);
}


/* Create Athena widgets to simulate a scale witget with a label */
void create_widgets( scrollbar, parent, name)
scrollbar_type scrollbar;
Widget         parent;
char *         name;
{
    scrollbar->form = XtVaCreateWidget(name, formWidgetClass, parent, NULL);

    scrollbar->label = 
        XtVaCreateManagedWidget("label", labelWidgetClass, scrollbar->form,
                                NULL);

    scrollbar->feedback = 
        XtVaCreateManagedWidget("feedback",
                                labelWidgetClass, scrollbar->form,
                                XtNfromHoriz, scrollbar->label,
                                NULL);

    scrollbar->scale = 
        XtVaCreateManagedWidget("bar",
                                scrollbarWidgetClass, scrollbar->form, 
                                XtNfromVert,  scrollbar->label,
                                XtNfromHoriz, scrollbar->label,
                                NULL);

    scrollbar->value = scrollbar_get_value( scrollbar);

    set_feedback_value( scrollbar, scrollbar->value);

    XtAddCallback( scrollbar->scale, XtNscrollProc, 
                   scrollbar_scroll_callback, (XtPointer)scrollbar);
    XtAddCallback( scrollbar->scale, XtNjumpProc, 
                   scrollbar_jump_callback, (XtPointer)scrollbar);

    XtManageChild( scrollbar->form);
}

#endif
