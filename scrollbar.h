/**********************************************************************
 * scrollbar.h - declares scrollbar object
 *
 * Copyright 1993, David Nedde
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/

#ifndef __SCROLLBAR_H__
#define __SCROLLBAR_H__

#include <X11/Intrinsic.h>

#include "list.h"

typedef struct {
    Widget    form;
    Widget    label;
    Widget    scale;
#ifdef ATHENA
    Widget    feedback;   /* Used to display value like Motif scale widget */
#endif

    int       value;      /* Current value of the scrollbar */
    list_type callbacks;  /* List of callbacks on this scrollbar */
} scrollbar_struct, *scrollbar_type;


/* We need to make the callbacks ourself, because the callback data is
   so different between Motif and Athena */
typedef struct {
    int      reason;
    XEvent * event;
    int      value;
    int      pixel;
} scrollbarCallbackStruct;


typedef void (*scrollbarCallbackProc)(/* Widget, caddr_t closure,
                                         scrollbarCallbackStruct call_data */);


/* Inline functions */
#define         scrollbar__get_value( scrollbar) ((scrollbar)->value)

/* Regular functions */
scrollbar_type  scrollbar__create(/* Widget w */);
void            scrollbar__destroy(/* scrollbar_type scrollbar */);
void            scrollbar__set_value(/* scrollbar_type *, int value */);
void            scrollbar__add_callback(/* scrollbar, proc, closure*/);


#endif
