/**********************************************************************
 * demo.h - declares the demo object
 *
 * Copyright 1993, David Nedde
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/

#ifndef __DEMO_H__
#define __DEMO_H__

#include <stdio.h>
#include <X11/Intrinsic.h>

#include "menu.h"
#include "scrollbar.h"
#include "file_sel.h"
#include "items.h"
#include "room.h"


typedef struct {
    Bool interactive;           /* Demo 'exit' will not exit program if 
                                   interactive is True */
    scrollbar_type gravity;     /* Gravity scrollbar */
    scrollbar_type elasticity;  /* Elasticity acrollbar */
    menu_type      menu;
    file_sel_type  file_sel;    /* To query user for demo file */
    items_type     items;
    room_type      room;

    FILE *   file_ptr;      /* The file pointer for the current demo file */
    char *   file_name;     /* The name of the current demo file */
    unsigned iterations;    /* Number of iterations we are currently pausing */
} demo_struct_type, *demo_type;


/* Public methods */
demo_type demo__create(/* w, menu, gravity, elasticity, file_sel, items, 
                       room*/);
void      demo__destroy(/* demo*/);
void      demo__run_mcb(/* w, demo, call_data, extra*/);
void      demo__stop_mcb(/* w, demo, call_data, extra*/);
void      demo__stop(/* demo*/);
Bool      demo__running(/*demo*/);
void      demo__process(/*demo*/);

/* Inline functions */
#define demo__set_interactive( demo, inter) ((demo)->interactive = (inter))


#endif
