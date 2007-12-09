/**********************************************************************
 * menu,h - Menu system declarations
 *
 * Copyright 1993, David Nedde
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/

#ifndef __MENU_H__
#define __MENU_H__

#include <X11/Intrinsic.h>

#include "table.h"

typedef struct {
    table_type mc_table;      /* Table of callbacks avalibale for use */
    table_type closure_table; /* Table of callbacks added */
    Widget     menubar;
#ifdef ATHENA
    Pixmap     on_bitmap;
    Pixmap     off_bitmap;
#endif
} menu_struct_type, *menu_type;


typedef struct {
    int      set;
} menuCallbackStruct;


typedef void (*menuCallbackProc)(/* Widget, caddr_t, 
                                    menuCallbackStruct *, char * */);


menu_type menu__create(/* form */);
void      menu__destroy(/*menu*/);
void      menu__add_callback(/* menu, action_name, 
                             menu_callback_proc, user_data */);
void      menu__set_toggle(/* menu, widget_name, set*/);
Bool      menu__get_toggle(/* menu, widget_name*/);
void      menu__set_sensitivity(/* menu, widget_name, sensitive*/);

#endif
